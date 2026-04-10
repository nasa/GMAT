//$Id$
//------------------------------------------------------------------------------
//                               ShadowState
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
// Author: Wendy C. Shoan
// Created: 2014.05.06
//
/**
 * Base class implementation for the ShadowState.
 */
//------------------------------------------------------------------------------

#include "ShadowState.hpp"
#include "SolarSystem.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "SolarSystemException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SHADOW_STATE
//#define DEBUG_SHADOW_STATE1
//#define DEBUG_SHADOW_STATE_INIT
//#define DEBUG_SHADOW_STATE_SUN_VECTOR
//#define DEBUG_SHADOW_STATE_2


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
ShadowState::ShadowState() :
   solarSystem  (NULL),
   sun          (NULL)//,
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ShadowState::ShadowState(const ShadowState &copy) :
   solarSystem  (NULL),
   sun          (NULL)//,
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ShadowState& ShadowState::operator=(const ShadowState &copy)
{
   if (&copy != this)
   {
      solarSystem  = NULL;
      sun          = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ShadowState::~ShadowState()
{
   solarSystem  = NULL;
   sun          = NULL;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void ShadowState::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_SHADOW_STATE_INIT
      MessageInterface::ShowMessage("Setting solar system on ShadowState\n");
   #endif
   solarSystem  = ss;
   sun          = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);
}



//------------------------------------------------------------------------------
// Real ShadowState::FindShadowState(Real *state,  Real *cbSun,
//                                   Real* sunSat, Real sunRad,
//                                   Real bodyRad)
//------------------------------------------------------------------------------
/**
 * Determines lighting conditions at the input location
 *
 * @param state    Current spacecraft state relative to its Shadow Body
 * @param cbSun    Shadow Body-to-Sun vector
 * @param sunSat   Sun-to-SC vector
 * @param sunRad   Sun radius
 * @param bodyRad  Radius of body
 *
 * NOTE: this code was adapted from original SRP Shadow code. Shadow body
 * refers to the occulting body. This code implments the "Shadow Function"
 * in Montenbruck and Gil section 3.4.2
 */
//------------------------------------------------------------------------------
Real ShadowState::FindShadowState(Real *state,  Real *sbSun,
                                  Real *sunSat, Real sunRad,
                                  Real bodyRad)
{
   Real percentSun    = 1.00;   // default is full sun
   Real mag = sqrt(sbSun[0]*sbSun[0] +
                   sbSun[1]*sbSun[1] +
                   sbSun[2]*sbSun[2]);
   
   Real unitsun[3];
   unitsun[0] = sbSun[0] / mag;
   unitsun[1] = sbSun[1] / mag;
   unitsun[2] = sbSun[2] / mag;
   
   Real rdotsun = state[0]*unitsun[0] +
                  state[1]*unitsun[1] +
                  state[2]*unitsun[2];
   
   if (rdotsun > 0.0) // Sunny side of central body is always fully lit
   {
       percentSun = 1.0;
       #ifdef DEBUG_SHADOW_STATE
         MessageInterface::ShowMessage(
               "ShadowState::FindShadowState **** Body is fully lit\n");
       #endif
       return percentSun;
   }
   else
   {
      //  Compute apparent quantities like body radii and distances between bodies
      Real satToSunDist;
      Real satToBodyDist;
      Real satToSunVec[3], unitSatToSun[3], unitBodyToSat[3];
      
      satToSunVec[0] = -sunSat[0];
      satToSunVec[1] = -sunSat[1];
      satToSunVec[2] = -sunSat[2];
      
      satToSunDist = sqrt(satToSunVec[0]*satToSunVec[0]+
      satToSunVec[1]*satToSunVec[1]+satToSunVec[2]*satToSunVec[2]);
      satToBodyDist = sqrt(state[0]*state[0]+
      state[1]*state[1]+state[2]*state[2]);
      
      if (sunRad >= satToSunDist)
      {
         percentSun = 1.0;
         return percentSun;
      }
      
      if (bodyRad >= satToBodyDist)
      {
         percentSun = 0.0;
         return percentSun;
      }
      
      //Equation 3.85 in Montenbruck 3.4.2
      Real a  = asin(sunRad  / satToSunDist);
      //Equation 3.86 in Montenbruck 3.4.2
      Real b  = asin(bodyRad / satToBodyDist);
      
      unitBodyToSat[0] = state[0]/satToBodyDist;
      unitBodyToSat[1] = state[1]/satToBodyDist;
      unitBodyToSat[2] = state[2]/satToBodyDist;
      
      unitSatToSun[0] = satToSunVec[0]/satToSunDist;
      unitSatToSun[1] = satToSunVec[1]/satToSunDist;
      unitSatToSun[2] = satToSunVec[2]/satToSunDist;
      
      //Based on Equation 3.87 in Montenbruck 3.4.2 where apparentDistFromSunToBody is c
      //This equation calculates the angle between the Sun and the shadow body as seen from the SC
      //It has been adapted for efficiency and availability of parameters
      Real c = acos(-unitBodyToSat[0]*unitSatToSun[0]
      -unitBodyToSat[1]*unitSatToSun[1]-unitBodyToSat[2]*unitSatToSun[2]);
      
      if (a + b <= c)
      {
         // This is the full sun light case
         percentSun = 1.0;
         return percentSun;
      }
      else if (c < b - a)
      {
         // This is the umbra case
         percentSun = 0.0;
         return percentSun;
      }
      //Equation 3.89 in Montenbruck 3.4.2
      else if ((fabs(a - b) < c)  &&
               (     a + b  > c))
      {
          // This is the penumbra case
         Real pcbrad  = asin(bodyRad/satToBodyDist);
         Real psunrad = asin(sunRad/satToSunDist);
         
         #ifdef DEBUG_SHADOW_STATE1
            MessageInterface::ShowMessage("  apparentDistFromSunToBody = %.12lf     apparentSunRadius = %.12lf  apparentBodyRadius = %.12lf\n", apparentDistFromSunToBody, apparentSunRadius, apparentBodyRadius);
         #endif
         percentSun = GetPercentSunInPenumbra(state, pcbrad, psunrad, unitSatToSun);
         return percentSun;
      }
      else
      {
      	// This is the anteumbra case
         percentSun =  1 - b*b/(a*a);
      	return percentSun;
      }
   }
   return percentSun;
   
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Real GetPercentSunInPenumbra(Real *state,
//                              Real *sunSat, Real *force,
//                              Real pcbrad, Real psunrad)
//------------------------------------------------------------------------------
/**
 * Determines the percent sun given the input epoch, state, sun vector,
 * and radii.
 *
 * @param state    Current spacecraft state relative to its origin
 * @param unitSunToSat   Sat-to-Sun unit vector
 * @param pcbrad   Central body radius over distance
 * @param psunrad  Sun radius over distance
 *
 * NOTE: this code was adapted from original SRP Shadow code
 */
//------------------------------------------------------------------------------
Real ShadowState::GetPercentSunInPenumbra(Real *state,
                  Real pcbrad, Real psunrad, Real *unitSunToSat)
{
   Real mag = sqrt(state[0]*state[0] +
                   state[1]*state[1] +
                   state[2]*state[2]);

   // Montenbruck and Gill, eq. 3.87
   Real c = acos((-state[0]*unitSunToSat[0] -
                   state[1]*unitSunToSat[1] -
                   state[2]*unitSunToSat[2]) / mag);

   Real a2 = psunrad*psunrad;
   Real b2 = pcbrad*pcbrad;

   // Montenbruck and Gill, eq. 3.93
   Real x = (c*c + a2 - b2) / (2.0 * c);
   #ifdef DEBUG_SHADOW_STATE1
         MessageInterface::ShowMessage("   sqrt in penumbra calc = %12.10f,  x = %.12lf  c = %.12lf  a = %.12lf  b = %.12lf\n",a2 - x*x, x, c, psunrad, pcbrad);
   #endif

   Real y = sqrt(a2 - x*x);

   // Montenbruck and Gill, eq. 3.92
   Real area = a2*acos(x/psunrad) +
               b2*acos((c-x)/pcbrad)
               - c*y;

   // Montenbruck and Gill, eq. 3.94
   return 1.0 - area / (GmatMathConstants::PI * a2);
}

