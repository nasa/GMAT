//$Id: SolarSailForce.cpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            SolarSailForce
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2008/07/03
//
/**
 * Implementation code for the SolarSailForce class, which extends the SRP model 
 * for solar sailing.
 * 
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------


#include "SolarSailForce.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// SolarSailForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 * 
 * Creates a solar sail force instance with a given name.
 * 
 * @param name The name used for the force.  This is usually the empty string.
 */
//------------------------------------------------------------------------------
SolarSailForce::SolarSailForce(const std::string &name) :
   SolarRadiationPressure  (name),
   firedOnce               (false),
   satCount                (1),
   norm                    (NULL)
{
   MessageInterface::ShowMessage("Constructed SailForce\n");
   typeName = "SailForce";
}


//------------------------------------------------------------------------------
// ~SolarSailForce()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SolarSailForce::~SolarSailForce()
{
   if (norm)
      delete [] norm;

}


//------------------------------------------------------------------------------
// SolarSailForce(const SolarSailForce& ssf)
//------------------------------------------------------------------------------
/**
 * Creates a solar sail force with settings matching the input force.
 * 
 * @param ssf The solar sail force that this one needs to match.
 */
//------------------------------------------------------------------------------
SolarSailForce::SolarSailForce(const SolarSailForce& ssf) :
   SolarRadiationPressure  (ssf),
   firedOnce               (false),
   satCount                (ssf.satCount),
   norm                    (NULL)
{
}


//------------------------------------------------------------------------------
// SolarSailForce& operator=(const SolarSailForce& ssf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * Sets this force to match the settings for the input force.
 * 
 * @param ssf The solar sail force that this one needs to match.
 * 
 * @return Reference to this instance.
 */
//------------------------------------------------------------------------------
SolarSailForce& SolarSailForce::operator=(const SolarSailForce& ssf)
{
   if (&ssf != this)
   {
      SolarRadiationPressure::operator=(ssf);
      
      firedOnce   = false;
      satCount    = ssf.satCount;
      norm        = NULL;
      initialized = false;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// bool IsUserForce()
//------------------------------------------------------------------------------
/**
 * Checks to see if this force is a user defined (aka plug-in) force.  
 * 
 * The SolarSailForce is user defined, so this method returns true.
 * 
 * @return true for user defined forces.
 */
//------------------------------------------------------------------------------
bool SolarSailForce::IsUserForce()
{
   return true;
}


//------------------------------------------------------------------------------
// bool SolarRadiationPressure::Initialize(void)
//------------------------------------------------------------------------------
/**
 * Prepares the force model data structures for use.
 * 
 * @return true if initialization was successful, false is not. 
 */
//------------------------------------------------------------------------------
bool SolarSailForce::Initialize()
{
   bool retval = SolarRadiationPressure::Initialize();
   
   if (norm)
      delete [] norm;
   norm = new Real[dimension];
   
   return retval;
}

//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate derivatives
 * 
 * This method is invoked to fill the deriv array with derivative information 
 * for the system that is being integrated.  It uses the state and elapsedTime 
 * parameters, along with the time interval dt passed in as a parameter, to 
 * calculate the derivative information at time \f$t=t_0+t_{elapsed}+dt\f$.
 *
 * @param dt         Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state      Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during 
 *                      propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order      The order of the derivative to be taken (first 
 *                      derivative, second derivative, etc)
 *
 * @return           true if the call succeeds, false on failure.  This default 
 *                      implementation always returns false.
 */
//------------------------------------------------------------------------------
bool SolarSailForce::GetDerivatives(Real * state, Real dt, Integer order)
{
   if (!initialized)
       return false;

   if (order > 2)
       return false;

   if (!firedOnce)
      CheckParameters();
   
   Integer i6;
   Real distancefactor = 1.0, absmag, refmag, common, absFactor, refFactor, 
        cosTheta;
   bool inSunlight = true, inShadow = false;

   Real ep = epoch + dt / 86400.0;
   sunrv = theSun->GetState(ep);
    
   // Rvector6 is initialized to all 0.0's; only change it if the body is not 
   // the Sun
   if (!bodyIsTheSun)
      cbrv = body->GetState(ep);
   cbSunVector[0] = sunrv[0] - cbrv[0];
   cbSunVector[1] = sunrv[1] - cbrv[1];
   cbSunVector[2] = sunrv[2] - cbrv[2];
   
   Real sunSat[3];

   for (Integer i = 0; i < satCount; ++i) 
   {
      i6 = i*6;

      // Build vector from the Sun to the current spacecraft
      sunSat[0] = state[i6] - cbSunVector[0];
      sunSat[1] = state[i6+1] - cbSunVector[1];
      sunSat[2] = state[i6+2] - cbSunVector[2];
      sunDistance = sqrt(sunSat[0]*sunSat[0] + sunSat[1]*sunSat[1] + 
                         sunSat[2]*sunSat[2]);
      if (sunDistance == 0.0)
         sunDistance = 1.0;
      // Make a unit vector for the force direction
      forceVector[0] = sunSat[0] / sunDistance;
      forceVector[1] = sunSat[1] / sunDistance;
      forceVector[2] = sunSat[2] / sunDistance;
        
      distancefactor = nominalSun / sunDistance;
      // Factor of 0.001 converts m/s^2 to km/s^2
      distancefactor *= distancefactor * 0.001;
      
      #ifdef DEBUG_SRP_ORIGIN
         if (shadowModel == 0) 
            shadowModel = CONICAL_MODEL;
      #endif
        
      // Test shadow condition for current spacecraft (only if body isn't Sol)
      if (!bodyIsTheSun)
      {
         psunrad = asin(sunRadius / sunDistance);
         FindShadowState(inSunlight, inShadow, &state[i6]);
      }
        
      if (!inShadow)
      {
         // Need to use attitude here; for now, point norm parallel to the 
         // vector from the Sun to the spacecraft (so it's really opposite to 
         // the normal vector)
         norm[0] = forceVector[0]; 
         norm[1] = forceVector[1]; 
         norm[2] = forceVector[2]; 
           
         // cosTheta takes into account the angle between the reflecting 
         // surface and the vector to the Sun
         cosTheta = norm[0] * forceVector[0] +
                    norm[1] * forceVector[1] +
                    norm[2] * forceVector[2];
           
         absFactor = 2.0 - cr[i];
         refFactor = 2.0 * (cr[i] - 1.0) * cosTheta;
           
         // Reflection and absorption both use this prefactor:
         common = percentSun * fluxPressure * area[i] / mass[i] * 
                  distancefactor * cosTheta;

         absmag = common * absFactor;
         refmag = common * refFactor;

         // Finally, calculate the acceleration.  Note that the minus sighn is 
         // missing from the spec, because the forceVector and norm both are 
         // set in the code to point away from the Sun rather than towards it.
         if (order == 1) 
         {
            deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 0.0;
            deriv[i6 + 3] = absmag * forceVector[0] + refmag * norm[0];
            deriv[i6 + 4] = absmag * forceVector[1] + refmag * norm[1];
            deriv[i6 + 5] = absmag * forceVector[2] + refmag * norm[2];
         }
         else 
         {
            deriv[ i6 ] = absmag * forceVector[0] + refmag * norm[0];
            deriv[i6+1] = absmag * forceVector[1] + refmag * norm[1];
            deriv[i6+2] = absmag * forceVector[2] + refmag * norm[2];
            deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
         }
      }
      else 
      {
         deriv[i6] = deriv[i6 + 1] = deriv[i6 + 2] = 
         deriv[i6 + 3] = deriv[i6 + 4] = deriv[i6 + 5] = 0.0;
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Makes a copy of this object.
 * 
 * @return A GmatBase pointer to the copy.
 */
//------------------------------------------------------------------------------
GmatBase* SolarSailForce::Clone() const
{
   return new SolarSailForce(*this);
}

//--------------------------------------
// Protected Methods
//--------------------------------------


//------------------------------------------------------------------------------
// void CheckParameters()
//------------------------------------------------------------------------------
/**
 * Checks to be sure that everything needed for calculation is set.
 * 
 * Throws an exception if a problem is detected.
 */
//------------------------------------------------------------------------------
void SolarSailForce::CheckParameters()
{
    satCount = dimension / 6;

    if (!theSun)
       throw ForceModelException("The Sun is not set in SRP::GetDerivatives");

    if (!body)
       throw ForceModelException(
          "The central body is not set in SRP::GetDerivatives");
    
    if (!cbSunVector)
       throw ForceModelException(
          "The sun vector is not initialized in SRP::GetDerivatives");

    if (cr.size() != satCount)
    {
       std::stringstream msg;
       msg << "Mismatch between satellite count (" << satCount 
           << ") and radiation coefficient count (" << cr.size() << ")"; 
       throw ForceModelException(msg.str());
    }

    if (area.size() != satCount)
    {
       std::stringstream msg;
       msg << "Mismatch between satellite count (" << satCount   
           << ") and area count (" << area.size() << ")"; 
       throw ForceModelException(msg.str());
    }

    if (mass.size() != satCount)
    {
       std::stringstream msg;
       msg << "Mismatch between satellite count (" << satCount   
           << ") and mass count (" << mass.size() << ")"; 
       throw ForceModelException(msg.str());
    }
    
    firedOnce = true;
}
