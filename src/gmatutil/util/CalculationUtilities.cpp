//$Id$
//------------------------------------------------------------------------------
//                              CalculationUtilities
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Created: 2011.08.12
//
/**
 * This namespace provides Calculation Utilities that can be used to compute the
 * GMAT Calculation Objects.
 */
//------------------------------------------------------------------------------
#include <math.h>
#include "CalculationUtilities.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "StateConversionUtil.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "AngleUtil.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define __COMPUTE_LONGITUDE_OLDWAY__

//#define DEBUG_CALC_UTIL
//#define DEBUG_ALTITUDE_LATITUDE
//#define DEBUG_ANGULAR_DATA

#ifdef DEBUG_CALC_UTIL
#include "MessageInterface.hpp"
#endif

using namespace GmatMathUtil;
using namespace GmatMathConstants;


//------------------------------------------------------------------------------
//  Real  CalculateBPlaneData(const std::string &item, const Rvector6 &state,
//                            const Real originMu)
//------------------------------------------------------------------------------
/**
 * This method calculates the BPlane Calculation Objects.
 *
 * @param item       string indicating which item to compute.
 * @param state      input state in the desired coordinate system
 * @param originMu   gravitational constant for the origin of the
 *                   coordinate system (assumes origin is a Celestial Body)
 *
 * @return           Real value requested
 *
 */
//------------------------------------------------------------------------------
Real GmatCalcUtil::CalculateBPlaneData(const std::string &item, const Rvector6 &state,
                                       const Real originMu)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);

   Real rMag       = pos.GetMagnitude();
   Real vMag       = vel.GetMagnitude();

   // Compute eccentricity related information
   Rvector3 eVec   = ((vMag*vMag - originMu/rMag) * pos - (pos*vel)*vel) / originMu;

   Real eMag        = eVec.GetMagnitude();

   // if eMag <= 1, then the method fails, orbit should be hyperbolic
   if (eMag <= 1.0)
      return GmatMathConstants::QUIET_NAN;
//      throw UtilityException
//         ("CalculationUtilities::CalculateBPlaneData() : ERROR - eccentricity magnitude is <= 1.0. eMag: " +
//          GmatRealUtil::ToString(eMag));

   eVec.Normalize();

   // Compute the angular momentum and orbit normal vectors
   Rvector3 hVec    = Cross(pos, vel);
   Real hMag        = hVec.GetMagnitude();
   hVec.Normalize();
   Rvector3 nVec    = Cross(hVec, eVec);

   // Compute semiminor axis, b
   Real b           = (hMag*hMag) / (originMu * Sqrt(eMag*eMag - 1.0));

   // Compute incoming asymptote
   Real oneOverEmag = 1.0/eMag;
   Real temp        = Sqrt(1.0 - oneOverEmag*oneOverEmag);
   Rvector3 sVec    = (eVec/eMag) + (temp*nVec);

   // Compute the B-vector
   Rvector3 bVec    = b * (temp * eVec - oneOverEmag*nVec);

   // Compute T and R vector
   Rvector3 sVec1(sVec[1], -sVec[0], 0.0);
   Rvector3 tVec = sVec1 / Sqrt(sVec[0]*sVec[0] + sVec[1]*sVec[1]);
   Rvector3 rVec = Cross(sVec, tVec);

   Real bDotT = bVec * tVec;
   Real bDotR = bVec * rVec;

   if (item == "BDotR")
   {
      return bDotR;
   }
   else if (item == "BDotT")
   {
      return bDotT;
   }
   else if (item == "BVectorMag")
   {
      return Sqrt(bDotT*bDotT + bDotR*bDotR);
   }
   else if (item == "BVectorAngle")
   {
      return ATan(bDotR, bDotT) * GmatMathConstants::DEG_PER_RAD;
   }
   else
   {
      throw UtilityException
         ("CalculationUtilities::CalculateBPlaneData() Unknown item: " + item);
   }

}


//------------------------------------------------------------------------------
//  Real  CalculateAngularData(const std::string &item, const Rvector6 &state,
//                             const Real originMu, const Rvector3 &originToSunUnit)
//------------------------------------------------------------------------------
/**
 * This method calculates the Angular (Orbit) Calculation Objects.
 *
 * @param item       string indicating which item to compute.
 * @param state      input state in the desired coordinate system;
 *                   for SemilatusRectum and HMag, is the relative Cartesian State
 * @param originMu   gravitational constant for the origin of the
 *                   coordinate system (assumes origin is a Celestial Body)
 * @param originToSunUnit
 *                   for Beta Angle, is the origin-to-sun unit vector
 *
 * @return           Real value requested
 *
 */
//------------------------------------------------------------------------------
Real GmatCalcUtil::CalculateAngularData(const std::string &item, const Rvector6 &state,
                                        const Real &originMu, const Rvector3 &originToSunUnit)
{
   #ifdef DEBUG_ANGULAR_DATA
   MessageInterface::ShowMessage
      ("Entering CalculateAngularData with item = %s, originMu = %12.10f\n   state = %s   "
       "originToSunUnit = %s", item.c_str(), originMu, state.ToString().c_str(),
       originToSunUnit.ToString().c_str());
   #endif
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   
   Rvector3 hVec3 = Cross(pos, vel);
   // Changed to sue GetMagnitude() (LOJ: 2012.10.23)
   //Real     hMag  = Sqrt(hVec3 * hVec3);
   Real     hMag  = hVec3.GetMagnitude();
   
   #ifdef DEBUG_ANGULAR_DATA
      MessageInterface::ShowMessage("   pos = %s   vel = %s   hVec3 = %s   hMag = %12.10f\n",
            pos.ToString().c_str(), vel.ToString().c_str(), hVec3.ToString().c_str(), hMag);
   #endif
   if (item == "SemilatusRectum")
   {
      if (hMag < GmatOrbitConstants::KEP_TOL)
         return 0.0;
      else
         return (hMag / originMu) * hMag;
   }
   else if (item == "HMag")
   {
      return hMag;
   }
   else if (item == "HX")
   {
      return hVec3[0];
   }
   else if (item == "HY")
   {
      return hVec3[1];
   }
   else if (item == "HZ")
   {
      return hVec3[2];
   }
   else if (item == "BetaAngle")
   {
      hVec3.Normalize();
      Real betaAngleRad = ASin(hVec3*originToSunUnit);
      Real betaAngleDeg = betaAngleRad * GmatMathConstants::DEG_PER_RAD;
      #ifdef DEBUG_ANGULAR_DATA
      MessageInterface::ShowMessage
         ("   hVec3Unit = %s   betaAngleRad = %.12f, betaAngleDeg = %.12f\n",
          hVec3.ToString().c_str(), betaAngleRad, betaAngleDeg);
      #endif
      return betaAngleDeg;
   }
   else if ((item == "RLA") || (item == "DLA"))
   {
      // Compute the eccentricity vector
      Real     r     = pos.GetMagnitude();
      Real     v     = vel.GetMagnitude();
      Rvector3 e     = ((((v * v) - originMu / r) * pos) - (pos * vel) *vel) / originMu;
      Real     ecc   = e.GetMagnitude();
      if (Abs(ecc) < 1.0 + GmatOrbitConstants::KEP_ECC_TOL)
         return GmatMathConstants::QUIET_NAN;
      
      // It is already computed above, so commented out (LOJ: 2012.10.23)
      // Compute orbit normal unit vector
      //Rvector3 hVec3 = Cross(pos, vel);
      //Real     hMag  = hVec3.GetMagnitude();
      
      // Compute C3
      Real     C3    = v * v - (2.0 * originMu) / r;
      Real     s_1   = 1.0 / (1.0 + C3 * (hMag / originMu) * (hMag / originMu));
      Rvector3 s     = s_1 * ((Sqrt(C3) / originMu) * Cross(hVec3, e) - e);
      if (item == "RLA")
         return ATan2(s[1], s[0]) * GmatMathConstants::DEG_PER_RAD;
      else // DLA
         return ASin(s[2]) * GmatMathConstants::DEG_PER_RAD;
   }
   else
   {
      throw UtilityException
         ("CalculationUtilities::CalculateAngularData() Unknown item: " + item);
   }

}

//------------------------------------------------------------------------------
//  Real  CalculateKeplerianData(const std::string &item, const Rvector6 &state,
//                               const Real originMu)
//------------------------------------------------------------------------------
/**
 * This method calculates the Keplerian Calculation Objects.
 *
 * @param item       string indicating which item to compute.
 * @param state      input state in the desired coordinate system
 * @param originMu   gravitational constant for the origin of the
 *                   coordinate system (assumes origin is a Celestial Body)
 *
 * @return           Real value requested
 *
 */
//------------------------------------------------------------------------------
Real GmatCalcUtil::CalculateKeplerianData(const std::string &item, const Rvector6 &state,
                   const Real originMu)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);

   Real sma = StateConversionUtil::CartesianToSMA(originMu, pos, vel);
   Real ecc = StateConversionUtil::CartesianToECC(originMu, pos, vel);

   if (GmatMathUtil::Abs(1.0 - ecc) <= GmatOrbitConstants::KEP_ECC_TOL)
   {
      throw UtilityException
         ("In CalculateKeplerianData, Error in conversion to Keplerian state: "
          "The state results in an orbit that is nearly parabolic.\n");
   }

  if (sma*(1 - ecc) < .001)
   {
      throw UtilityException
         ("In CalculateKeplerianData, Error in conversion to Keplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }

  if (item == "MeanMotion")
  {
     if (ecc < (1.0 - GmatOrbitConstants::KEP_ECC_TOL))      // Ellipse
        return Sqrt(originMu / (sma*sma*sma));                               // unit: radian/s
     else if (ecc > (1.0 + GmatOrbitConstants::KEP_ECC_TOL)) // Hyperbola
        return Sqrt(-(originMu / (sma*sma*sma)));                            // unit: radian/s
     else
        return 2.0 * Sqrt(originMu); // Parabola             // It should be 0.0 for parabola ????                      
  }
  else if (item == "VelApoapsis")
  {
     if ( (ecc < 1.0 - GmatOrbitConstants::KEP_ECC_TOL) || (ecc > 1.0 + GmatOrbitConstants::KEP_ECC_TOL))  //Ellipse and Hyperbola
        return Sqrt( (originMu/sma)*((1-ecc)/(1+ecc)) );                       // unit: km/s
     else
        return 0.0; // Parabola                                                // unit: km/s
  }
  else if (item == "VelPeriapsis")
  {
     return Sqrt( (originMu/sma)*((1+ecc)/(1-ecc)) );                          // unit: km/s
  }
  else if (item == "OrbitPeriod")
  {
     if (sma < 0.0)
        return 0.0;                                                            // unit: s
     else
        return GmatMathConstants::TWO_PI * Sqrt((sma * sma * sma)/ originMu);  // unit: s
  }
  else if (item == "RadApoapsis")
  {
      if ( (ecc < 1.0 - GmatOrbitConstants::KEP_ECC_TOL) || (ecc > 1.0 + GmatOrbitConstants::KEP_ECC_TOL)) //Ellipse and Hyperbola
          return sma * (1.0 + ecc);
      else
          return 0.0;   // Parabola
  }
  else if (item == "RadPeriapsis")
  {
     return sma * (1.0 - ecc);
  }
  else if (item == "C3Energy")
  {
     return -originMu / sma;                                    // unit: (km/s)^2
  }
  else if (item == "Energy")
  {
     return -originMu / (2.0 * sma);
  }
  else
  {
     throw UtilityException
        ("CalculationUtilities::CalculateKeplerianData() Unknown item: " + item);
  }
}


//----------------------------------------------------------------
// Real GmatCalcUtil::CalculatePlanetData(const std::string &item, 
//      const Rvector6 &state, const Real originRadius, 
//      const Real originFlattening, const Real originHourAngle)
//----------------------------------------------------------------
/**
*     This function is used to calculate plenet data
* @param item              Name of planet data element
* @param state             State in the planet's body fixed coordinate system
* @param originRadius      Radius of the planet
* @param originFlattening  Flattening of the planet
*
* @return      longitude in degree     for item = "Longitude"
*/
//----------------------------------------------------------------
Real GmatCalcUtil::CalculatePlanetData(const std::string &item, const Rvector6 &state,
   const Real originRadius, const Real originFlattening, const Real originHourAngle)
{
   if (item == "MHA")
   {
      return originHourAngle;   // is this call even really necessary???
   }
   else if (item == "Longitude")
   {
      // The input state is in the origin-centered BodyFixed Coordinate System
      Real longitude = ATan(state[1], state[0]) * GmatMathConstants::DEG_PER_RAD;
      longitude = AngleUtil::PutAngleInDegRange(longitude, -180.0, 180.0);

      return longitude;
   }
   else if ((item == "Latitude") || (item == "Altitude"))
   {

      // Reworked to match Vallado algorithm 12 (Vallado, 2nd ed, p. 177)

      // Note -- using cmath here because I know it better -- may want to change
      // to GmatMath

      Real rxy               = sqrt(state[0]*state[0] + state[1]*state[1]);
      Real geolat            = atan2(state[2], rxy);
      Real delta             = 1.0;
      Real geodeticTolerance = 1.0e-7;    // Better than 0.0001 degrees
      Real ecc2              = 2.0 * originFlattening - originFlattening*originFlattening;

      #ifdef DEBUG_ALTITUDE_LATITUDE
            MessageInterface::ShowMessage("originRadius = %12.10f, rxy = %12.10f, geolat = %12.10f, ecc2 = %12.10f\n",
                  originRadius, rxy, geolat, ecc2);
      #endif

      Real cFactor, oldlat, sinlat;
      while (delta > geodeticTolerance)
      {
         oldlat  = geolat;
         sinlat  = sin(oldlat);
         cFactor = originRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
         geolat  = atan2(state[2] + cFactor*ecc2*sinlat, rxy);
         delta   = fabs(geolat - oldlat);
      }

      if (item == "Latitude")
      {
         //return geolat * 180.0 / PI;
         // put latitude between -90 and 90
         geolat = geolat * 180.0 / GmatMathConstants::PI;
         geolat = AngleUtil::PutAngleInDegRange(geolat, -90.0, 90.0);
         #ifdef DEBUG_ALTITUDE_LATITUDE
               MessageInterface::ShowMessage("   returning latitude = %12.10f\n", geolat);
         #endif
         return geolat;
      }
      else  // item == "Altitude"
      {
         sinlat = sin(geolat);
         cFactor = originRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
         #ifdef DEBUG_ALTITUDE_LATITUDE
            MessageInterface::ShowMessage("cFactor = %12.10f\n", cFactor);
            MessageInterface::ShowMessage("   returning altitude = %12.10f\n", (rxy / cos(geolat) - cFactor));
         #endif
         return rxy / cos(geolat) - cFactor;
      }
   }
   else if (item == "LST")
   {
      // compute Local Sidereal Time (LST = GMST + Longitude)
      // according to Vallado Eq. 3-41
      // The input state is in the origin-centered BodyFixed Coordinate System
      Real longitude = ATan(state[1], state[0]) * GmatMathConstants::DEG_PER_RAD;
      longitude      = AngleUtil::PutAngleInDegRange(longitude, -180.0, 180.0);
      Real lst       = originHourAngle + longitude;
      lst            = AngleUtil::PutAngleInDegRange(lst, 0.0, 360.0);

      return lst;
   }
   else
   {
      throw UtilityException
         ("CalculationUtilities::CalculatePlanetData() Unknown item: " + item);
   }
}

