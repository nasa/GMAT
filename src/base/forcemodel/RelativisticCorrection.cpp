//$Id:  $
//------------------------------------------------------------------------------
//                              RelativisticCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: W. Shoan
// Created: 2011.07.06
///
/**
 * This is the RelativisticCorrection class.
 *
 * @note original prolog information included at end of file prolog.
 */
#include "gmatdefs.hpp"
#include "PhysicalModel.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "RealUtilities.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "RelativisticCorrection.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RELATIVISTIC_CORRECTION
//#define DEBUG_DERIVATIVES
//#define DEBUG_REL_CORR_SET_GET


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string RelativisticCorrection::PARAMETER_TEXT[RelativisticCorrectionParamCount - PhysicalModelParamCount] =
{
      "Radius",
      "Mu",
};
const Gmat::ParameterType RelativisticCorrection::PARAMETER_TYPE[RelativisticCorrectionParamCount - PhysicalModelParamCount] =
{
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RelativisticCorrection(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection::RelativisticCorrection(const std::string &name, const std::string &forBodyName) :
  PhysicalModel          (Gmat::PHYSICAL_MODEL, "RelativisticCorrection", name),
  theSun                 (NULL),
  bodyRadius             (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
  bodyMu                 (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
  sunMu                  (GmatSolarSystemDefaults::STAR_MU),
  bodySpinRate           (0.0),
  satCount               (0),
  bodyInertial           (NULL),
  bodyFixed              (NULL),
  eop                    (NULL)
{
   objectTypeNames.push_back("RelativisticCorrection");
   parameterCount = RelativisticCorrectionParamCount;
   dimension      = 6 * satCount;
   body           = NULL;
   bodyName       = forBodyName;

   // create default body
   bodyName       = SolarSystem::EARTH_NAME;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}

//------------------------------------------------------------------------------
// ~RelativisticCorrection()
//------------------------------------------------------------------------------
/**
 * Destructor for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection::~RelativisticCorrection()
{
   if (bodyInertial) delete bodyInertial;
   if (bodyFixed)    delete bodyFixed;
}

//------------------------------------------------------------------------------
// RelativisticCorrection(const RelativisticCorrection &rc)
//------------------------------------------------------------------------------
/**
 * Copy constructor for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection::RelativisticCorrection(const RelativisticCorrection &rc) :
   PhysicalModel(rc),
   theSun         (NULL),
   bodyRadius     (rc.bodyRadius),
   bodyMu         (rc.bodyMu),
   sunMu          (rc.sunMu),
   bodySpinRate   (rc.bodySpinRate),
   now            (rc.now),
   satCount       (rc.satCount),
   bodyInertial   (NULL),
   bodyFixed      (NULL),
   eop            (rc.eop)
{
   objectTypeNames.push_back("RelativisticCorrection");
   parameterCount = RelativisticCorrectionParamCount;
   dimension      = rc.dimension;
   epoch          = rc.epoch;
   isInitialized    = false;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}

//------------------------------------------------------------------------------
//  operator=(const RelativisticCorrection &rc)
//------------------------------------------------------------------------------
/**
 * operator= for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection& RelativisticCorrection::operator=(const RelativisticCorrection &rc)
{
   if (&rc == this)
      return *this;

   PhysicalModel::operator=(rc);

   theSun         = NULL;
   bodyRadius     = rc.bodyRadius;
   bodyMu         = rc.bodyMu;
   bodySpinRate   = rc.bodySpinRate;
   now            = rc.now;
   satCount       = rc.satCount;
   bodyInertial   = (CoordinateSystem*) rc.bodyInertial->Clone();
   bodyFixed      = (CoordinateSystem*) rc.bodyFixed->Clone();
   eop            = rc.eop;

   return *this;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method to initialize the RelativisticCorrection object
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::Initialize()
{
   PhysicalModel::Initialize();

   if (!eop) throw ODEModelException(
             "EOP file is undefined for RelativisticCorrection " + instanceName);

   if (solarSystem != NULL)
   {
      body                 = solarSystem->GetBody(bodyName);
      theSun               = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);

      if (body == NULL)
      {
         isInitialized = false;
         throw ODEModelException("RelativisticCorrection::Initialize() body \"" +
            bodyName + "\" is not in the solar system\n");
      }
      if (theSun == NULL)
      {
         isInitialized = false;
         throw ODEModelException("RelativisticCorrection::Initialize() Sun not found in the Solar System.\n");
      }
   }
   else
   {
      isInitialized = false;
      throw ODEModelException(
         "RelativisticCorrection::Initialize() solarSystem is NULL\n");
   }

   // Create the coordinate systems needed (Earth only)
//   if (body->GetName() == GmatSolarSystemDefaults::EARTH_NAME)
//   {
      bodyInertial = CoordinateSystem::CreateLocalCoordinateSystem("bodyInertial", "MJ2000Eq", body,
                                       NULL, NULL, body->GetJ2000Body(), solarSystem);
      bodyFixed    = CoordinateSystem::CreateLocalCoordinateSystem("bodyFixed", "BodyFixed", body,
                                       NULL, NULL, body->GetJ2000Body(), solarSystem);
//   }

   return true;
}


//------------------------------------------------------------------------------
//  bool GetDerivatives(Real *state, Real dt = 0.0, Integer order = 1,
//                      const Integer id = -1)
//------------------------------------------------------------------------------
/**
 * Method to return the derivative array
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::GetDerivatives(Real *state, Real dt, Integer order, const Integer id)
{
   if ((order > 2) || (order < 1))
      return false;

   if ((cartesianCount < 1)  && (stmCount < 1) && (aMatrixCount < 1))
      throw ODEModelException(
         "RelativisticCorrection requires at least one spacecraft.");

   now         = epoch + (elapsedTime + dt)/GmatTimeConstants::SECS_PER_DAY;

   if (fillCartesian)
   {
      Real      c      = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM;
      sunMu            = theSun->GetGravitationalConstant();
      bodyMu           = body->GetGravitationalConstant();  // this is passed in by the ODEModel

      #ifdef DEBUG_RELATIVISTIC_CORRECTION
         MessageInterface::ShowMessage("now                    = %12.10f\n", now.Get());
         MessageInterface::ShowMessage("c                      = %12.10f\n", c);
         MessageInterface::ShowMessage("bodyMu                 = %12.10f\n", bodyMu);
      #endif

      Rvector6  dummy(0.0, 1.0, 2.0, 3.0, 4.0, 5.0), dummyResult;
      Real      r, v, s1, rvDotvvX4, s2_1, lt1, lt2;
      Rmatrix33 R;    // fixed to inertial rotation matrix
      Rmatrix33 Rdot; // fixed to inertial rotation Dot matrix
      Real      ar[3], geodesic[3], s2[3], s3[3], schwarzschild[3], J1[3], J[3], rvCrossvv[3], vvCrossJ[3];
      Real      rv[3], vv[3], omega[3], lenseThirring[3];
      Rvector3  bodySpinVector;

      J[0]             = J[1]             = J[2]             = 0.0;
      omega[0]         = omega[1]         = omega[2]         = 0.0;
      geodesic[0]      = geodesic[1]      = geodesic[2]      = 0.0;
      lenseThirring[0] = lenseThirring[1] = lenseThirring[2] = 0.0;


      // compute quantities needed for geodesic (for non-Sun only) and lense-thirring terms
      if (body->GetName() != GmatSolarSystemDefaults::SUN_NAME)
      {
         Rvector6  stateWRTSun;
         Real      posWRTSun[3], velWRTSun[3], vel[3], pos[3];
         Real      posMag, muCBc2r3;
         Real      threeOver2 = 3.0 / 2.0;

         stateWRTSun  = body->GetMJ2000State(now) - theSun->GetMJ2000State(now);
         posWRTSun[0] = stateWRTSun[0];
         posWRTSun[1] = stateWRTSun[1];
         posWRTSun[2] = stateWRTSun[2];
         velWRTSun[0] = stateWRTSun[3];
         velWRTSun[1] = stateWRTSun[4];
         velWRTSun[2] = stateWRTSun[5];
         posMag       = GmatMathUtil::Sqrt(posWRTSun[0] * posWRTSun[0] + posWRTSun[1] * posWRTSun[1] + posWRTSun[2] * posWRTSun[2]);

         muCBc2r3     = sunMu/ (c * c * posMag * posMag * posMag);

         vel[0]       = threeOver2 * velWRTSun[0];
         vel[1]       = threeOver2 * velWRTSun[1];
         vel[2]       = threeOver2 * velWRTSun[2];
         pos[0]       = -muCBc2r3 * posWRTSun[0];
         pos[1]       = -muCBc2r3 * posWRTSun[1];
         pos[2]       = -muCBc2r3 * posWRTSun[2];
         // Compute cross product
         omega[0]     = vel[1]*pos[2] - vel[2]*pos[1];
         omega[1]     = vel[2]*pos[0] - vel[0]*pos[2];
         omega[2]     = vel[0]*pos[1] - vel[1]*pos[0];
         #ifdef DEBUG_RELATIVISTIC_CORRECTION
            MessageInterface::ShowMessage("muCBc2r3  = %le\n", muCBc2r3);
            MessageInterface::ShowMessage("posMag    = %le\n", posMag);
            MessageInterface::ShowMessage("pos       = %le   %le   %le\n", pos[0], pos[1], pos[2]);
            MessageInterface::ShowMessage("vel       = %le   %le   %le\n", vel[0], vel[1], vel[2]);
            MessageInterface::ShowMessage("posWRTSun = %le   %le   %le\n", posWRTSun[0], posWRTSun[1], posWRTSun[2]);
            MessageInterface::ShowMessage("velWRTSun = %le   %le   %le\n", velWRTSun[0], velWRTSun[1], velWRTSun[2]);
            MessageInterface::ShowMessage("big Omega = %le   %le   %le\n", omega[0], omega[1], omega[2]);
         #endif
      }

      bodyRadius   = body->GetEquatorialRadius();
      // We want the body's fixed to inertial rotation matrix
      cc.Convert(now, dummy, bodyFixed, dummyResult, bodyInertial);
      R            = cc.GetLastRotationMatrix();
      Rdot         = cc.GetLastRotationDotMatrix();

      // Compute the body spin rate
      bodySpinVector[0] = (-R(0,2) * Rdot(0,1)) - (R(1,2) * Rdot(1,1)) - (R(2,2) * Rdot(2,1));
      bodySpinVector[1] = ( R(0,2) * Rdot(0,0)) + (R(1,2) * Rdot(1,0)) + (R(2,2) * Rdot(2,0));
      bodySpinVector[2] = (-R(0,1) * Rdot(0,0)) - (R(1,1) * Rdot(1,0)) - (R(2,1) * Rdot(2,0));
      bodySpinRate = GmatMathUtil::Sqrt(bodySpinVector[0] * bodySpinVector[0] + bodySpinVector[1] * bodySpinVector[1] + bodySpinVector[2] * bodySpinVector[2]);
      J1[0] = 0.0;
      J1[1] = 0.0;
      J1[2] = (2.0 / 5.0) * bodyRadius * bodyRadius * bodySpinRate;
      J[0] = R(0,0)*J1[0] + R(0,1)*J1[1] + R(0,2)*J1[2];
      J[1] = R(1,0)*J1[0] + R(1,1)*J1[1] + R(1,2)*J1[2];
      J[2] = R(2,0)*J1[0] + R(2,1)*J1[1] + R(2,2)*J1[2];

      #ifdef DEBUG_RELATIVISTIC_CORRECTION
         MessageInterface::ShowMessage("R    = %s\n", R.ToString().c_str());
         MessageInterface::ShowMessage("Rdot = %s\n", Rdot.ToString().c_str());
         MessageInterface::ShowMessage("J                      = %le   %le   %le\n", J[0], J[1], J[2]);
         MessageInterface::ShowMessage("bodySpinVector         = %le   %le   %le\n", bodySpinVector[0], bodySpinVector[1], bodySpinVector[2]);
         MessageInterface::ShowMessage("bodySpinRate           = %le\n", bodySpinRate);
      #endif

      Integer nOffset;
      for (Integer n = 0; n < cartesianCount; ++n)
      {
         nOffset = cartesianStart + n * 6;   // stateSize;
         for (Integer i = 0; i < 3; ++i)
         {
            rv[i] = state[i+nOffset];
            vv[i] = state[i+nOffset+3];
         }

         // Compute the Schwarzschild solution
         r             = GmatMathUtil::Sqrt(rv[0] * rv[0] + rv[1] * rv[1] + rv[2] * rv[2]);
         v             = GmatMathUtil::Sqrt(vv[0] * vv[0] + vv[1] * vv[1] + vv[2] * vv[2]);
         s1            = bodyMu / (c * c * r * r * r);
         s2_1          = (4.0 * bodyMu / r) - (v * v);
         s2[0]         = s2_1 * rv[0];
         s2[1]         = s2_1 * rv[1];
         s2[2]         = s2_1 * rv[2];
         rvDotvvX4     = 4.0 * (rv[0] * vv[0] + rv[1] * vv[1] + rv[2] * vv[2]);
         s3[0]         = rvDotvvX4 * vv[0];
         s3[1]         = rvDotvvX4 * vv[1];
         s3[2]         = rvDotvvX4 * vv[2];
         schwarzschild[0] = s1 * (s2[0] + s3[0]);
         schwarzschild[1] = s1 * (s2[1] + s3[1]);
         schwarzschild[2] = s1 * (s2[2] + s3[2]);

         // IF the body is not the Sun, compute the geodesic term
         if (body->GetName() != GmatSolarSystemDefaults::SUN_NAME)
         {
            // Compute the geodesic precession
            geodesic[0]       =  2.0 * (omega[1]*vv[2] - omega[2]*vv[1]);
            geodesic[1]       =  2.0 * (omega[2]*vv[0] - omega[0]*vv[2]);
            geodesic[2]       =  2.0 * (omega[0]*vv[1] - omega[1]*vv[0]);
         }
         else // if it is the Sun, we don't compute the geodesic term
         {
            geodesic[0]       =  0.0;
            geodesic[1]       =  0.0;
            geodesic[2]       =  0.0;
         }

         // Compute the Lense-Thirring precession
         rvCrossvv[0] = rv[1]*vv[2] - rv[2]*vv[1];
         rvCrossvv[1] = rv[2]*vv[0] - rv[0]*vv[2];
         rvCrossvv[2] = rv[0]*vv[1] - rv[1]*vv[0];
         vvCrossJ[0]  = vv[1]*J[2]  - vv[2]*J[1];
         vvCrossJ[1]  = vv[2]*J[0]  - vv[0]*J[2];
         vvCrossJ[2]  = vv[0]*J[1]  - vv[1]*J[0];
         lt1          = 2.0 * s1;
         lt2          = (3.0 / (r * r)) * (rv[0] * J[0] + rv[1] * J[1] + rv[2] * J[2]);

         lenseThirring[0]  =  lt1 * ((lt2 * rvCrossvv[0]) + vvCrossJ[0]);
         lenseThirring[1]  =  lt1 * ((lt2 * rvCrossvv[1]) + vvCrossJ[1]);
         lenseThirring[2]  =  lt1 * ((lt2 * rvCrossvv[2]) + vvCrossJ[2]);

         //Add the terms together
         ar[0]  = schwarzschild[0] + geodesic[0] + lenseThirring[0];
         ar[1]  = schwarzschild[1] + geodesic[1] + lenseThirring[1];
         ar[2]  = schwarzschild[2] + geodesic[2] + lenseThirring[2];

         #ifdef DEBUG_RELATIVISTIC_CORRECTION
            MessageInterface::ShowMessage("spacecraft position = %le   %le   %le\n", rv[0], rv[1], rv[2]);
            MessageInterface::ShowMessage("spacecraft velocity = %le   %le   %le\n", vv[0], vv[1], vv[2]);
            MessageInterface::ShowMessage("schwarzschild       = %le   %le   %le\n", schwarzschild[0], schwarzschild[1], schwarzschild[2]);
            MessageInterface::ShowMessage("geodesic            = %le   %le   %le\n", geodesic[0], geodesic[1], geodesic[2]);
            MessageInterface::ShowMessage("lenseThirring       = %le   %le   %le\n", lenseThirring[0], lenseThirring[1], lenseThirring[2]);
         #endif

         // Fill Derivatives
         switch (order)
         {
            case 1:
               deriv[0+nOffset] = 0.0;
               deriv[1+nOffset] = 0.0;
               deriv[2+nOffset] = 0.0;
               deriv[3+nOffset] = ar[0];
               deriv[4+nOffset] = ar[1];
               deriv[5+nOffset] = ar[2];
               break;

            case 2:
               deriv[0+nOffset] = ar[0];
               deriv[1+nOffset] = ar[1];
               deriv[2+nOffset] = ar[2];
               deriv[3+nOffset] = 0.0;
               deriv[4+nOffset] = 0.0;
               deriv[5+nOffset] = 0.0;
               break;
         }

      }
   }   // fillCartesian


   if (fillSTM)
   {
		Integer i6 = stmStart;
      Integer element;
      for (Integer i = 0; i < stmCount; ++i)
      {
			Integer ix;
         
			// Get Spacecraft object
			Spacecraft* sc = (Spacecraft*)scObjs[i];

			// Create aTilde matrix
			stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
			Integer stmSize = stmRowCount * stmRowCount;
			Real *aTilde = new Real[stmSize];

			// Calculate A-tilde
			for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix+k] = 0.0;
         }

			
         for (Integer j = 0; j < stmRowCount; j++)
         {
            for (Integer k = 0; k < stmRowCount; k++)
            {
               element = j * stmRowCount + k;
               #ifdef DEBUG_DERIVATIVES
                  MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", (i6+element), aTilde[element]);
               #endif
               deriv[i6+element] = aTilde[element];
            }
         }

			delete[] aTilde;
			i6 = i6 + stmSize;
      }
   }
   if (fillAMatrix)
   {
      Integer i6 = aMatrixStart;
      Integer element;
      for (Integer i = 0; i < stmCount; ++i)
      {
			Integer ix;

			// Get Spacecraft object
			Spacecraft* sc = (Spacecraft*)scObjs[i];

         // Create aTilde matrix
			stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
			Integer stmSize = stmRowCount * stmRowCount;
			Real *aTilde = new Real[stmSize];

			// Calculate A-tilde
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix+k] = 0.0;
         }

         for (Integer j = 0; j < stmRowCount; j++)
         {
            for (Integer k = 0; k < stmRowCount; k++)
            {
               element = j * stmRowCount + k;
               #ifdef DEBUG_DERIVATIVES
                  MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", (i6+element), aTilde[element]);
               #endif
               deriv[i6+element] = aTilde[element];
            }
         }

			delete[] aTilde;
			i6 = i6 + stmSize;
		}
   }

   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft* sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 RelativisticCorrection::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;

   Real *j2kState = sc->GetState().GetState();
   Real state[6];
   Real now = sc->GetEpoch();
   GmatTime nowgt = sc->GetEpochGT();

   if (hasPrecisionTime)
      BuildModelStateGT(nowgt, state, j2kState);
   else
      BuildModelState(now, state, j2kState);

   Real      c      = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM *
         GmatMathConstants::M_TO_KM;
   sunMu            = theSun->GetGravitationalConstant();
   bodyMu           = body->GetGravitationalConstant();  // from ODEModel

   Rvector6  dummy(0.0, 1.0, 2.0, 3.0, 4.0, 5.0), dummyResult;
   Real      r, v, s1, rvDotvvX4, s2_1, lt1, lt2;
   Rmatrix33 R;    // fixed to inertial rotation matrix
   Rmatrix33 Rdot; // fixed to inertial rotation Dot matrix
   Real      ar[3], geodesic[3], s2[3], s3[3], schwarzschild[3], J1[3], J[3],
             rvCrossvv[3], vvCrossJ[3];
   Real      rv[3], vv[3], omega[3], lenseThirring[3];
   Rvector3  bodySpinVector;

   J[0]             = J[1]             = J[2]             = 0.0;
   omega[0]         = omega[1]         = omega[2]         = 0.0;
   geodesic[0]      = geodesic[1]      = geodesic[2]      = 0.0;
   lenseThirring[0] = lenseThirring[1] = lenseThirring[2] = 0.0;

   // needed for geodesic (for non-Sun only) and lense-thirring terms
   if (body->GetName() != GmatSolarSystemDefaults::SUN_NAME)
   {
      Rvector6  stateWRTSun;
      Real      posWRTSun[3], velWRTSun[3], vel[3], pos[3];
      Real      posMag, muCBc2r3;
      Real      threeOver2 = 3.0 / 2.0;

      if (hasPrecisionTime)
         stateWRTSun = body->GetMJ2000State(nowgt) - theSun->GetMJ2000State(nowgt);
      else
         stateWRTSun  = body->GetMJ2000State(now) - theSun->GetMJ2000State(now);

      posWRTSun[0] = stateWRTSun[0];
      posWRTSun[1] = stateWRTSun[1];
      posWRTSun[2] = stateWRTSun[2];
      velWRTSun[0] = stateWRTSun[3];
      velWRTSun[1] = stateWRTSun[4];
      velWRTSun[2] = stateWRTSun[5];
      posMag       = GmatMathUtil::Sqrt(posWRTSun[0] * posWRTSun[0] +
                                        posWRTSun[1] * posWRTSun[1] +
                                        posWRTSun[2] * posWRTSun[2]);

      muCBc2r3     = sunMu/ (c * c * posMag * posMag * posMag);

      vel[0]       = threeOver2 * velWRTSun[0];
      vel[1]       = threeOver2 * velWRTSun[1];
      vel[2]       = threeOver2 * velWRTSun[2];
      pos[0]       = -muCBc2r3 * posWRTSun[0];
      pos[1]       = -muCBc2r3 * posWRTSun[1];
      pos[2]       = -muCBc2r3 * posWRTSun[2];
      // Compute cross product
      omega[0]     = vel[1]*pos[2] - vel[2]*pos[1];
      omega[1]     = vel[2]*pos[0] - vel[0]*pos[2];
      omega[2]     = vel[0]*pos[1] - vel[1]*pos[0];
      #ifdef DEBUG_RELATIVISTIC_CORRECTION
         MessageInterface::ShowMessage("muCBc2r3  = %le\n", muCBc2r3);
         MessageInterface::ShowMessage("posMag    = %le\n", posMag);
         MessageInterface::ShowMessage("pos       = %le   %le   %le\n", pos[0],
               pos[1], pos[2]);
         MessageInterface::ShowMessage("vel       = %le   %le   %le\n", vel[0],
               vel[1], vel[2]);
         MessageInterface::ShowMessage("posWRTSun = %le   %le   %le\n",
               posWRTSun[0], posWRTSun[1], posWRTSun[2]);
         MessageInterface::ShowMessage("velWRTSun = %le   %le   %le\n",
               velWRTSun[0], velWRTSun[1], velWRTSun[2]);
         MessageInterface::ShowMessage("big Omega = %le   %le   %le\n",
               omega[0], omega[1], omega[2]);
      #endif
   }

   bodyRadius   = body->GetEquatorialRadius();
   // We want the body's fixed to inertial rotation matrix
   if (hasPrecisionTime)
      cc.Convert(nowgt, dummy, bodyFixed, dummyResult, bodyInertial);
   else
      cc.Convert(now, dummy, bodyFixed, dummyResult, bodyInertial);

   R            = cc.GetLastRotationMatrix();
   Rdot         = cc.GetLastRotationDotMatrix();

   // Compute the body spin rate
   bodySpinVector[0] = (-R(0,2) * Rdot(0,1)) - (R(1,2) * Rdot(1,1)) -
         (R(2,2) * Rdot(2,1));
   bodySpinVector[1] = ( R(0,2) * Rdot(0,0)) + (R(1,2) * Rdot(1,0)) +
         (R(2,2) * Rdot(2,0));
   bodySpinVector[2] = (-R(0,1) * Rdot(0,0)) - (R(1,1) * Rdot(1,0)) -
         (R(2,1) * Rdot(2,0));
   bodySpinRate = GmatMathUtil::Sqrt(bodySpinVector[0] * bodySpinVector[0] +
                                     bodySpinVector[1] * bodySpinVector[1] +
                                     bodySpinVector[2] * bodySpinVector[2]);
   J1[0] = 0.0;
   J1[1] = 0.0;
   J1[2] = (2.0 / 5.0) * bodyRadius * bodyRadius * bodySpinRate;
   J[0] = R(0,0)*J1[0] + R(0,1)*J1[1] + R(0,2)*J1[2];
   J[1] = R(1,0)*J1[0] + R(1,1)*J1[1] + R(1,2)*J1[2];
   J[2] = R(2,0)*J1[0] + R(2,1)*J1[1] + R(2,2)*J1[2];

   #ifdef DEBUG_RELATIVISTIC_CORRECTION
      MessageInterface::ShowMessage("R    = %s\n", R.ToString().c_str());
      MessageInterface::ShowMessage("Rdot = %s\n", Rdot.ToString().c_str());
      MessageInterface::ShowMessage("J                      = %le   %le   "
            "%le\n", J[0], J[1], J[2]);
      MessageInterface::ShowMessage("bodySpinVector         = %le   %le   "
            "%le\n", bodySpinVector[0], bodySpinVector[1], bodySpinVector[2]);
      MessageInterface::ShowMessage("bodySpinRate           = %le\n",
            bodySpinRate);
   #endif

   for (Integer i = 0; i < 3; ++i)
   {
      rv[i] = state[i];
      vv[i] = state[i+3];
   }

   // Compute the Schwarzschild solution
   r        = GmatMathUtil::Sqrt(rv[0] * rv[0] + rv[1] * rv[1] + rv[2] * rv[2]);
   v        = GmatMathUtil::Sqrt(vv[0] * vv[0] + vv[1] * vv[1] + vv[2] * vv[2]);
   s1       = bodyMu / (c * c * r * r * r);
   s2_1     = (4.0 * bodyMu / r) - (v * v);
   s2[0]    = s2_1 * rv[0];
   s2[1]    = s2_1 * rv[1];
   s2[2]    = s2_1 * rv[2];
   rvDotvvX4 = 4.0 * (rv[0] * vv[0] + rv[1] * vv[1] + rv[2] * vv[2]);
   s3[0]    = rvDotvvX4 * vv[0];
   s3[1]    = rvDotvvX4 * vv[1];
   s3[2]    = rvDotvvX4 * vv[2];
   schwarzschild[0] = s1 * (s2[0] + s3[0]);
   schwarzschild[1] = s1 * (s2[1] + s3[1]);
   schwarzschild[2] = s1 * (s2[2] + s3[2]);

   // IF the body is not the Sun, compute the geodesic term
   if (body->GetName() != GmatSolarSystemDefaults::SUN_NAME)
   {
      // Compute the geodesic precession
      geodesic[0]       =  2.0 * (omega[1]*vv[2] - omega[2]*vv[1]);
      geodesic[1]       =  2.0 * (omega[2]*vv[0] - omega[0]*vv[2]);
      geodesic[2]       =  2.0 * (omega[0]*vv[1] - omega[1]*vv[0]);
   }
   else // if it is the Sun, we don't compute the geodesic term
   {
      geodesic[0]       =  0.0;
      geodesic[1]       =  0.0;
      geodesic[2]       =  0.0;
   }

   // Compute the Lense-Thirring precession
   rvCrossvv[0] = rv[1]*vv[2] - rv[2]*vv[1];
   rvCrossvv[1] = rv[2]*vv[0] - rv[0]*vv[2];
   rvCrossvv[2] = rv[0]*vv[1] - rv[1]*vv[0];
   vvCrossJ[0]  = vv[1]*J[2]  - vv[2]*J[1];
   vvCrossJ[1]  = vv[2]*J[0]  - vv[0]*J[2];
   vvCrossJ[2]  = vv[0]*J[1]  - vv[1]*J[0];
   lt1          = 2.0 * s1;
   lt2          = (3.0 / (r*r)) * (rv[0] * J[0] + rv[1] * J[1] + rv[2] * J[2]);

   lenseThirring[0]  =  lt1 * ((lt2 * rvCrossvv[0]) + vvCrossJ[0]);
   lenseThirring[1]  =  lt1 * ((lt2 * rvCrossvv[1]) + vvCrossJ[1]);
   lenseThirring[2]  =  lt1 * ((lt2 * rvCrossvv[2]) + vvCrossJ[2]);

   //Add the terms together
   ar[0]  = schwarzschild[0] + geodesic[0] + lenseThirring[0];
   ar[1]  = schwarzschild[1] + geodesic[1] + lenseThirring[1];
   ar[2]  = schwarzschild[2] + geodesic[2] + lenseThirring[2];

   dv[0] = 0.0;
   dv[1] = 0.0;
   dv[2] = 0.0;
   dv[3] = ar[0];
   dv[4] = ar[1];
   dv[5] = ar[2];

   return dv;
}


//------------------------------------------------------------------------------
//  void SetEopFile(EopFile *eopF)
//------------------------------------------------------------------------------
/**
 * Method to set EopFile for this system.
 *
 * @param eopF  the eopFile.
 *
 */
//------------------------------------------------------------------------------
void RelativisticCorrection::SetEopFile(EopFile *eopF)
{
   eop = eopF;
}


//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
 /**
* This method returns a clone of the RelativisticCorrection.
 *
 * @return clone of the RelativisticCorrection.
 *
 */
//------------------------------------------------------------------------------
GmatBase* RelativisticCorrection::Clone() const
{
   return (new RelativisticCorrection(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string RelativisticCorrection::GetParameterText(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < RelativisticCorrectionParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer RelativisticCorrection::GetParameterID(const std::string &str) const
{
   for (Integer i = PhysicalModelParamCount; i < RelativisticCorrectionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType RelativisticCorrection::GetParameterType(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < RelativisticCorrectionParamCount))
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string RelativisticCorrection::GetParameterTypeString(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < RelativisticCorrectionParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::IsParameterReadOnly(const Integer id) const
{
   return true;  // all read-only for now
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real RelativisticCorrection::GetRealParameter(const Integer id) const
{
   #ifdef DEBUG_REL_CORR_SET_GET
      MessageInterface::ShowMessage("RelativisticCorrection: Getting Real parameter %d\n", id);
   #endif
   if (id == BODY_RADIUS)
      return bodyRadius;

   if (id == BODY_MU)
      return bodyMu;

   return PhysicalModel::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real RelativisticCorrection::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_REL_CORR_SET_GET
      MessageInterface::ShowMessage("RelativisticCorrection: Setting Real parameter %d to value %le\n",
            id, value);
   #endif
   if (id == BODY_RADIUS)
   {
      bodyRadius = value;
      return true;
   }

   if (id == BODY_MU)
   {
      bodyMu = value;
      return true;
   }

   return PhysicalModel::SetRealParameter(id, value);
}


// Methods used by the ODEModel to set the state indexes, etc
//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative
 * information for a specified type.
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::SupportsDerivative(Gmat::StateElementId id)
{
   if (id == Gmat::CARTESIAN_STATE)
      return true;

   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;

   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id         State Element ID for the derivative type
 * @param index      Starting index in the state vector for this type of derivative
 * @param quantity   Number of objects that supply this type of data
 * @param totalSize  For sizable types, the size to use.  For example, for STM,
 *                   this is the number of elements in all STMs
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::SetStart(Gmat::StateElementId id, Integer index,
                                      Integer quantity, Integer totalSize)
{
   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartesianStart = index;
         cartesianCount = quantity;
         fillCartesian = true;
         retval = true;
         break;

      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount = quantity;
         stmStart = index;
         fillSTM = true;
			totalSTMSize = totalSize;
         retval = true;
         break;

      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
			totalSTMSize = totalSize;
			retval = true;
         break;

      default:
         break;
   }

   return retval;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// N/A

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// N/A


