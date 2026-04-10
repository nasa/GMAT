//------------------------------------------------------------------------------
//                           Propagator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.06
//
/**
 * Implementation of the propagator class
 */
//------------------------------------------------------------------------------

#include <cmath>            // for INFINITY
#include "gmatdefs.hpp"
#include "Propagator.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "TATCException.hpp"
#include "StateConversionUtil.hpp"
#include "MessageInterface.hpp"
#include "bessel.hpp"
#include "ExponentialAtmosphere.hpp"
#include "Earth.hpp"

//#define DEBUG_DRAG

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const Real Propagator::MU_FOR_EARTH = 398600.4415;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Propagator(Spacecraft *sat)
//------------------------------------------------------------------------------
/**
 * Default constructor for Propagator.
 *
 * @param sat The spacecraft object
 * 
 */
//------------------------------------------------------------------------------
Propagator::Propagator(Spacecraft *sat) :
   sc                     (sat),
   J2                     (1.0826269e-003),
   mu                     (MU_FOR_EARTH),
   eqRadius               (6.3781363e+003),
   applyDrag              (false),
   refJd                  (GmatTimeConstants::JD_OF_J2000),
   SMA                    (0.0),
   ECC                    (0.0),
   INC                    (0.0),
   RAAN                   (0.0),
   AOP                    (0.0),
   TA                     (0.0),
   MA                     (0.0),
   meanMotionRate         (0.0),
   argPeriapsisRate       (0.0),
   rightAscensionNodeRate (0.0),
   semiLatusRectum        (0.0),
   meanMotion             (0.0)
{
   OrbitState *orbSt = sc->GetOrbitState();
   SetOrbitState(orbSt);
   refJd = sc->GetOrbitEpoch()->GetJulianDate();
   ComputeOrbitRates();
   densityModel = new ExponentialAtmosphere("ExpDensity");
   lastDragUpdateEpoch = refJd;
   Rvector6 kepElem = orbSt->GetKeplerianState();
   Real sma = kepElem[0];
   orbitPeriod = GmatMathConstants::TWO_PI / GmatMathUtil::Sqrt(mu /
                 (sma*sma*sma));
}

//------------------------------------------------------------------------------
//  Propagator(const Propagator &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor for Propagator.
 *
 * @param copy The propagator to copy
 * 
 */
//------------------------------------------------------------------------------
Propagator::Propagator(const Propagator &copy) :
   sc                     (copy.sc),   // correct?
   J2                     (copy.J2),
   mu                     (copy.mu),
   eqRadius               (copy.eqRadius),
   applyDrag              (copy.applyDrag),
   refJd                  (copy.refJd),
   propStart              (copy.propStart),
   propEnd                (copy.propEnd),
   lastDragUpdateEpoch    (copy.lastDragUpdateEpoch),
   orbitPeriod            (copy.orbitPeriod),
   SMA                    (copy.SMA),
   ECC                    (copy.ECC),
   INC                    (copy.INC),
   RAAN                   (copy.RAAN),
   AOP                    (copy.AOP),
   TA                     (copy.TA),
   MA                     (copy.MA),
   meanMotionRate         (copy.meanMotionRate),
   argPeriapsisRate       (copy.argPeriapsisRate),
   rightAscensionNodeRate (copy.rightAscensionNodeRate),
   semiLatusRectum        (copy.semiLatusRectum),
   meanMotion             (copy.meanMotion)
{
	// TODO: add orbit state 
	densityModel = new ExponentialAtmosphere("ExpDensity");
}

//------------------------------------------------------------------------------
//  Propagator& operator=(const Propagator &copy)
//------------------------------------------------------------------------------
/**
 * operator= for Propagator.
 *
 * @param copy The propagator to copy
 * 
 */
//------------------------------------------------------------------------------
Propagator& Propagator::operator=(const Propagator &copy)
{
   if (&copy == this)
      return *this;
   
   sc                     = copy.sc; // correct?
   J2                     = copy.J2;
   mu                     = copy.mu;
   eqRadius               = copy.eqRadius;
   applyDrag              = copy.applyDrag;
   refJd                  = copy.refJd;
   propStart              = copy.propStart;
   propEnd                = copy.propEnd;
   lastDragUpdateEpoch    = copy.lastDragUpdateEpoch;
   orbitPeriod            = copy.orbitPeriod;
   SMA                    = copy.SMA;
   ECC                    = copy.ECC;
   INC                    = copy.INC;
   RAAN                   = copy.RAAN;
   AOP                    = copy.AOP;
   TA                     = copy.TA;
   MA                     = copy.MA;
   meanMotionRate         = copy.meanMotionRate;
   argPeriapsisRate       = copy.argPeriapsisRate;
   rightAscensionNodeRate = copy.rightAscensionNodeRate;
   semiLatusRectum        = copy.semiLatusRectum;
   meanMotion             = copy.meanMotion;

   return *this;
}

//------------------------------------------------------------------------------
//  ~Propagator()
//------------------------------------------------------------------------------
/**
 * destructor for Propagator.
 *
 */
//------------------------------------------------------------------------------
Propagator::~Propagator()
{
   if (densityModel)
      delete densityModel;
}

//------------------------------------------------------------------------------
// void SetPhysicalConstants(Real bodyMu, Real bodyJ2,
//                           Real bodyRadius)
//------------------------------------------------------------------------------
/**
 * Sets physical constant values for the Propagator.
 *
 * @param bodyMu     gravitational parameter to use
 * @param bodyJ2 J2  term to use
 * @param bodyRadius radius of the body
 * 
 */
//------------------------------------------------------------------------------
void Propagator::SetPhysicalConstants(Real bodyMu, Real bodyJ2,
                                      Real bodyRadius)
{
   mu       = bodyMu;
   J2       = bodyJ2;
   eqRadius = bodyRadius;
}


//------------------------------------------------------------------------------
// Rvector6 Propagate(const AbsoluteDate &toDate)
//------------------------------------------------------------------------------
/**
 * Propagates to the input time.
 *
 * @param toDate     date to which to propagate
 * 
 */
//------------------------------------------------------------------------------
Rvector6 Propagator::Propagate(const AbsoluteDate &toDate)
{
   // Propgate and return cartesian state given AbsoluteDate
   Real propDuration = (toDate.GetJulianDate() -
                        refJd) * GmatTimeConstants::SECS_PER_DAY;
   Real durationFromDragAdjust = (toDate.GetJulianDate() -
	                              lastDragUpdateEpoch) *
                                 GmatTimeConstants::SECS_PER_DAY;
   Rvector6 orbElem  = PropagateOrbitalElements(propDuration);

   // Apply drag effects if we have propagated more than one rev since 
   // last drag effect update
   Real numRevs = durationFromDragAdjust / orbitPeriod;
   if (applyDrag && (GmatMathUtil::Abs(numRevs) > 1))
   {
	   Real deltaSMAPerRev = 0.0;
	   Real deltaECCPerRev = 0.0;
	 
	   Real altitude = ComputePeriapsisAltitude(orbElem,sc->GetJulianDate());
	   ComputeDragEffects(SMA, ECC, altitude, deltaSMAPerRev, deltaECCPerRev);
	   lastDragUpdateEpoch = toDate.GetJulianDate();
	   refJd      = lastDragUpdateEpoch;
	   SMA        = SMA + deltaSMAPerRev * numRevs;
	   ECC        = ECC + deltaECCPerRev * numRevs;
	   orbElem(0) = SMA;
	   orbElem(1) = ECC;
      #ifdef DEBUG_DRAG
	      MessageInterface::ShowMessage("OrbitPeriod %12.10f \n", orbitPeriod);
	      MessageInterface::ShowMessage(
			  "Duration Form Last Drag Adjust %16.14f \n", durationFromDragAdjust);
	      MessageInterface::ShowMessage("SMA After Drag Adjust %12.10f \n", SMA);
		   MessageInterface::ShowMessage("ECC After Drag Adjust %12.10f \n", ECC);
      #endif
   }

   sc->SetOrbitState(toDate, orbElem);
   
   // Save the initial prop time
   if (propStart.GetJulianDate() == GmatTimeConstants::JD_OF_J2000)
      propStart = toDate;
   // Save the current (end) prop time
   propEnd = toDate;
   
   return sc->GetCartesianState();
}

//------------------------------------------------------------------------------
// void GetPropStartEnd(AbsoluteDate &pStart, AbsoluteDate &pEnd)
//------------------------------------------------------------------------------
/**
 * Returns the propagator start and end times
 *
 * @param pStart     [out] start time
 * @param pEnd       [out] end time
 *
 */
//------------------------------------------------------------------------------
void Propagator::GetPropStartEnd(AbsoluteDate &pStart, AbsoluteDate &pEnd)
{
   pStart = propStart;
   pEnd   = propEnd;
}


//------------------------------------------------------------------------------
// void SetApplyDrag(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the flag indicating whether or not to apply drag
 *
 * @param flag     apply drag flag
 *
 */
//------------------------------------------------------------------------------
void Propagator::SetApplyDrag(bool flag)
{
	applyDrag = flag;
}

//------------------------------------------------------------------------------
// bool GetApplyDrag()
//------------------------------------------------------------------------------
/**
 * Returns the flag indicating whether or not to apply drag
 *
 * @return  apply drag flag
 *
 */
//------------------------------------------------------------------------------
bool Propagator::GetApplyDrag()
{
	return applyDrag;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void SetOrbitState(OrbitState &orbState)
//------------------------------------------------------------------------------
/**
 * Sets the orbit state on the Propagator.
 *
 * @param orbState     orbit state
 * 
 */
//------------------------------------------------------------------------------
void Propagator::SetOrbitState(OrbitState *orbState)
{
   // Set orbit state given OrbitState object.
   
   // Done at intialization for performance reasons.
   Rvector6 kepElements = orbState->GetKeplerianState();
   SMA  = kepElements(0);
   ECC  = kepElements(1);
   INC  = kepElements(2);
   RAAN = kepElements(3);
   AOP  = kepElements(4);
   TA   = kepElements(5);
   MA   = StateConversionUtil::TrueToMeanAnomaly(TA,ECC);
}

//------------------------------------------------------------------------------
// Real ComputePeriapsisAltitude(Rvector6 orbElem, Real julianDate)
//------------------------------------------------------------------------------
/**
 * Computes the periapsis altitude
 *
 * @param orbElem     input orbital elements
 * @param julianDate  the date at which to compute the periapsis altitude
 * 
 * @return the periapsis altitude
 */
//------------------------------------------------------------------------------
Real Propagator::ComputePeriapsisAltitude(Rvector6 orbElem, Real julianDate)
{

	// Extract needed components
	Real sma = orbElem[0];
	Real ecc = orbElem[1];

	// Trivial if circular orbit
	if (ecc <= 1.0e-7)
   {
		return sma * (1 - ecc) - eqRadius;
	}

	// Compute the eccentrity vector (vector pointing towards perigee)
	OrbitState orbState;
	orbState.SetKeplerianState(orbElem[0],orbElem[1],orbElem[2],orbElem[3],
                              orbElem[4],orbElem[5]);
	Rvector6 cartState = orbState.GetCartesianState();
	Rvector3 posVec(cartState[0], cartState[1], cartState[2]);
	Rvector3 velVec(cartState[3], cartState[4], cartState[5]);
	Real rMag       = posVec.GetMagnitude();
	Real vMag       = velVec.GetMagnitude();
	Real dotProd    = (posVec[0] * velVec[0]) + (posVec[1] * velVec[1]) +
		               (posVec[2] * velVec[2]);
	Rvector3 eVec   = ((vMag * vMag - MU_FOR_EARTH / rMag) *
                       posVec - dotProd * velVec) / mu;
	Rvector3 perVec = eVec.Normalize() * sma * (1 - ecc);

	// Convert from intertial to ellipsoid at epoch
	Earth CentralBody;
	Rvector3 bodyFixedState = CentralBody.InertialToBodyFixed(perVec, julianDate,
                             "Ellipsoid");

	// Return the altitude
	return bodyFixedState[2];
}



//------------------------------------------------------------------------------
// Rvector6 PropagateOrbitalElements(Real propDuration)
//------------------------------------------------------------------------------
/**
 * Propagates the orbital elements for the specificed duration.
 *
 * @param propDuration     duration over which to propagate
 * 
 * @return  the propagated orbital elements
 */
//------------------------------------------------------------------------------
Rvector6 Propagator::PropagateOrbitalElements(Real propDuration)
{
   // Propagate and return orbital elements
   Rvector6 orbElements; // all zeros by default
   orbElements(0) = SMA;
   orbElements(1) = ECC;
   orbElements(2) = INC;
   orbElements(3) = GmatMathUtil::Mod(RAAN +
                                      rightAscensionNodeRate * propDuration,
                                      GmatMathConstants::TWO_PI);
   orbElements(4) = GmatMathUtil::Mod(AOP + argPeriapsisRate * propDuration,
                                      GmatMathConstants::TWO_PI);
   Real newMA = GmatMathUtil::Mod(MA + meanMotionRate * propDuration,
                                  GmatMathConstants::TWO_PI);
   orbElements(5) = StateConversionUtil::MeanToTrueAnomaly(newMA,ECC);
   return orbElements;
}

//------------------------------------------------------------------------------
// Real MeanMotion()
//------------------------------------------------------------------------------
/**
 * Computes the mean motion.
 *
 * @return Mean Motion
 * 
 */
//------------------------------------------------------------------------------
Real Propagator::MeanMotion()
{
  // Computes the orbital mean motion
   Real meanMotion = GmatMathUtil::Sqrt(mu / (SMA*SMA*SMA));
   return meanMotion;
}

//------------------------------------------------------------------------------
// Real SemiParameter()
//------------------------------------------------------------------------------
/**
 * Computes the semi parameter.
 *
 * @return SemiParameter
 * 
 */
//------------------------------------------------------------------------------
Real Propagator::SemiParameter()
{
   // Computes the orbital semi parameter
   Real semiParameter = SMA * (1 - ECC*ECC);
   return semiParameter;
}

//------------------------------------------------------------------------------
// void ComputeOrbitRates()
//------------------------------------------------------------------------------
/**
 * Computes the orbit rates.
 *
 */
//------------------------------------------------------------------------------
void Propagator::ComputeOrbitRates()
{
   // Compute orbit element rates
   ComputeMeanMotionRate();
   ComputeArgumentOfPeriapsisRate();
   ComputeRightAscensionNodeRate();
}

//------------------------------------------------------------------------------
// void ComputeMeanMotionRate()
//------------------------------------------------------------------------------
/**
 * Computes the mean motion rate.
 * 
 */
//------------------------------------------------------------------------------
void Propagator::ComputeMeanMotionRate()
{
   // Computes the orbital mean motion Rate
   // Vallado, 3rd. Ed.  Eq9-41
   Real n      = MeanMotion();
   Real tmpJ2  = J2;
   Real e      = ECC;
   Real p      = SemiParameter();
   Real sinInc = GmatMathUtil::Sin(INC);
   meanMotionRate = n - 0.75 * n * tmpJ2 * (eqRadius/p) * (eqRadius/p) *
                    GmatMathUtil::Sqrt(1.0 - e*e) * (3.0 * sinInc*sinInc - 2.0);
}

//------------------------------------------------------------------------------
// void ComputeArgumentOfPeriapsisRate()
//------------------------------------------------------------------------------
/**
 * Computes the argument of periapsis rate.
 * 
 */
//------------------------------------------------------------------------------
void Propagator::ComputeArgumentOfPeriapsisRate()
{
   // Computes the argument of periapsis rate
   // Vallado, 3rd. Ed.  Eq9-39
   Real n      = MeanMotion();
   Real s      = SemiParameter();
   Real sinInc = GmatMathUtil::Sin(INC);
   argPeriapsisRate = 3.0 * n * (eqRadius*eqRadius) * J2 /
                      4.0 / (s*s) * (4.0 - 5.0 * (sinInc*sinInc));
}

//------------------------------------------------------------------------------
// void ComputeRightAscensionNodeRate()
//------------------------------------------------------------------------------
/**
 * Computes the right ascension node rate.
 * 
 */
//------------------------------------------------------------------------------
void Propagator::ComputeRightAscensionNodeRate()
{
   // Computes rate right ascension of node rate
   // Vallado, 3rd. Ed.  Eq9-37
   Real n = MeanMotion();
   Real s = SemiParameter();
   rightAscensionNodeRate = -3.0 * n * (eqRadius*eqRadius) * J2 /
                             2.0 / (s*s) * GmatMathUtil::Cos(INC);
}

//------------------------------------------------------------------------------
// void ComputeDragEffects(Real sma, Real ecc, Real altitude,
//                         Real &deltaSMAPerRev,
//                         Real &deltaECCperRev)
//------------------------------------------------------------------------------
/**
 * Computes the drag effects
 *
 * @param sma              [in]  semimajor axis
 * @param ecc              [in]  eccentricity
 * @param altitude         [in]  altitude
 * @param deltaSMAPerRev   [out] the delta SMA
 * @param deltaECCperRev   [out[ the delta ECC
 *
 */
//------------------------------------------------------------------------------
void Propagator::ComputeDragEffects(Real sma, Real ecc, Real altitude,
                                    Real &deltaSMAPerRev,
                                    Real &deltaECCperRev)
{
	
	// Extract data required for computation
	Real Cd          = sc->GetDragCoefficient();
	Real totalMass   = sc->GetTotalMass();
	Real dragAreaKm2 = sc->GetDragArea() / 1.0e6;
	
	// Compute the density
	Real density     = densityModel->Density(altitude);
	Real scaleHeight = densityModel->GetScaleHeight(altitude);

	if (ecc <= 1e-7)
   {
		// Compute delta sma and ecc.
		// Eq. 6-24 and 6-27 from SMAD 2nd Edition, J.R. Wertz
		deltaSMAPerRev = - GmatMathConstants::TWO_PI * (Cd * dragAreaKm2 /
			                totalMass) * density * sma*sma;
		deltaECCperRev = 0.0;
		return;
	}
	
	// Compute common terms
	Real c  = sma * ecc / scaleHeight;
	Real I0 = boost::math::cyl_bessel_i(0, c);
	Real I1 = boost::math::cyl_bessel_i(1, c);
	Real I2 = boost::math::cyl_bessel_i(2, c);
	Real commonFac = -2 * GmatMathConstants::PI *
                    (Cd * dragAreaKm2 / totalMass) *
                    sma * density * GmatMathUtil::Exp(-c);

	// Compute delta sma and ecc.
	// Eq. 6-22 and 6-23 from SMAD 2nd Edition, J.R. Wertz
	deltaSMAPerRev = commonFac * sma * (I0 + 2 * ecc * I1);
	deltaECCperRev = commonFac * (I1 + ecc / 2 * (I0 + I2));

    #ifdef DEBUG_DRAG
	   MessageInterface::ShowMessage("Altitude %12.10f \n", altitude);
	   MessageInterface::ShowMessage("Scale Height %12.10f \n", scaleHeight);
	   MessageInterface::ShowMessage("Density %16.14f \n", density);
	   MessageInterface::ShowMessage("Bessel Function Input %12.10f \n", c);
	   MessageInterface::ShowMessage("Delta SMA %12.10f \n", deltaSMAPerRev);
	   MessageInterface::ShowMessage("Delta ECC %12.10f \n", deltaECCperRev);
    #endif
}


