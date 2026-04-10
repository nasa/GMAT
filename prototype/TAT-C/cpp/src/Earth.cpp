//------------------------------------------------------------------------------
//                           Earth
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
// Created: 2016.05.03
//
/**
 * Implementation of the Earth class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Earth.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Earth()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 */
//------------------------------------------------------------------------------
Earth::Earth() :
   J2               (1.0826269e-003),
   mu               (3.986004415e+5),
   radius           (6.3781363e+003),
   flattening       (0.0033527),
   lastRotationTime (0.0)
{
}

//------------------------------------------------------------------------------
// Earth(const Earth &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param  copy  the Earth object to copy
 */
//------------------------------------------------------------------------------
Earth::Earth(const Earth &copy) :
   J2               (copy.J2),
   mu               (copy.mu),
   radius           (copy.radius),
   flattening       (copy.flattening),
   lastRotationTime (copy.lastRotationTime)
{
}

//------------------------------------------------------------------------------
// Earth & operator=(const Earth &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the Earth.
 *
 * @param  copy  the Earth object to copy
 */
//------------------------------------------------------------------------------
Earth& Earth::operator=(const Earth &copy)
{
   if (&copy == this)
      return *this;
   
   J2               = copy.J2;
   mu               = copy.mu;
   radius           = copy.radius;
   flattening       = copy.flattening;
   lastRotationTime = copy.lastRotationTime;
   
   return *this;
}

//------------------------------------------------------------------------------
// ~Earth ()
//------------------------------------------------------------------------------
/**
 * Detructor
 */
//------------------------------------------------------------------------------
Earth::~Earth()
{
}

//------------------------------------------------------------------------------
//  Real  GetRadius()
//------------------------------------------------------------------------------
/**
* Returns the mean equatorial radius.
 *
 * @return  the mean equatorial radius
*/
//------------------------------------------------------------------------------
Real Earth::GetRadius()
{
    return radius;
}


//------------------------------------------------------------------------------
//  Rmatrix33  GetInertialToFixedRotation(Real jd)
//------------------------------------------------------------------------------
/**
 * Returns the inertial-to-fixed rotation matrix.
 *
 * @param  jd  the Julian date at which to compute the rotation matrix.
 * 
 * @return  inertial-to-fixed rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 Earth::GetInertialToFixedRotation(Real jd)
{
   if (!GmatMathUtil::IsEqual(jd, lastRotationTime))
   {
      Real gmt = ComputeGMT(jd);
      Real cG  = GmatMathUtil::Cos(gmt);
      Real sG  = GmatMathUtil::Sin(gmt);
      rotationResult.Set( cG,  sG, 0.0,
                         -sG,  cG, 0.0,
                         0.0, 0.0, 1.0);
   }
   lastRotationTime = jd;
   return rotationResult;
}

//------------------------------------------------------------------------------
//  Real ComputeGMT(Real jd)
//------------------------------------------------------------------------------
/**
 * Returns the GMT.
 *
 * @param  jd  the Julian date at which to compute the GMT.
 * 
 * @return  GMT
 */
//------------------------------------------------------------------------------
Real Earth::ComputeGMT(Real jd)
{
   Real timeUT1 = (jd - GmatTimeConstants::JD_OF_J2000) /
                  GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   Real GMT     = 67310.54841 +(876600.0 * 3600.0+8640184.812866) * timeUT1 +
                  0.093104 * (timeUT1*timeUT1) -
                  6.2e-6 * (timeUT1*timeUT1*timeUT1);
   
   GMT = GmatMathUtil::Mod(GMT,GmatTimeConstants::SECS_PER_DAY)/240.0
         * GmatMathConstants::RAD_PER_DEG;
   return GMT;
}

//------------------------------------------------------------------------------
//  Rvector3 GetBodyFixedState(Rvector3 inertialState,
//                             Real      jd)
//------------------------------------------------------------------------------
/**
 * Returns the body-fixed state given the inertial stat and the time.
 *
 * @param  inertialState  the inertial state.
 * @param  jd             the Julian date at which to compute the
 *                        body-fixed state.
 * 
 * @return  body-fixed state
 */
//------------------------------------------------------------------------------
Rvector3 Earth::GetBodyFixedState(Rvector3 inertialState,
                                  Real      jd)
{
   return GetInertialToFixedRotation(jd) * inertialState;
}

//------------------------------------------------------------------------------
// Rvector3 Convert (const Rvector3 &origValue, const std::string &fromType
//                   const std::string &toType);
//------------------------------------------------------------------------------
/**
 * Conversion method between body fixed representations. Valid values for
 * &origvalue are "Cartesian", "Spherical", and "Ellipsoid"
 *
 * @param origValue  data in given representation
 * @param fromType   representation from which to convert
 * @param toType     representation to which to convert
 *
 * @return Converted state from the specified "from" representation to the
 *         specified "to" representation
 */
//------------------------------------------------------------------------------
Rvector3 Earth::Convert(const Rvector3 &origValue, const std::string &fromType,
                        const std::string &toType)
{
    return BodyFixedStateConverterUtil::Convert(origValue, fromType, toType,
                                                flattening, radius);
}

//------------------------------------------------------------------------------
// Rvector3 InertialToBodyFixed (const Rvector3 &inertialVector,const Real jd,
//                               const std::string &toType);
//------------------------------------------------------------------------------
/**
* Conversion method between inertial and body fixed representations.
* Valid values for &toType are "Cartesian", "Spherical", and "Ellipsoid"
*
* @param inertialVector  intertial Cartesian vector
* @param jd              Julian date associated with <inertialVector>
* @param toType          representation to which to convert
*
* @return Converted state from the inertial Cartesian input to the 
*         specified "to" representation
*/
//------------------------------------------------------------------------------
Rvector3 Earth::InertialToBodyFixed(const Rvector3 &inertialVector,
                                    Real jd,
	                                 const std::string &toType)
{
	Rvector3 fixedValue = GetBodyFixedState(inertialVector, jd);
	return Convert(fixedValue, "Cartesian", toType);
}

//------------------------------------------------------------------------------
// Rvector3 GetSunPositionInBodyCoords(Real jd,
//                                     const std::string &toType;
//------------------------------------------------------------------------------
/**
* Computes sun position in body coordinates returning position in requested
* representation. 
* 
* @param jd     Julian date
* @param toType Output representation.  Valid values for are
*               "Cartesian", "Spherical", and "Ellipsoid"
*
* @return Sun position in body coordinates in requested representation
*/
//------------------------------------------------------------------------------
Rvector3 Earth::GetSunPositionInBodyCoords(Real jd,
                                           const std::string &toType)
{
	Rvector3 rSun(0.0,0.0,0.0);
	Real rtAsc = 0.0;
	Real decl = 0.0;
	GetEarthSunDistRaDec(jd, rSun, rtAsc, decl);
	rSun *= GmatPhysicalConstants::ASTRONOMICAL_UNIT;
	Rvector3 fixedCart = GetBodyFixedState(rSun, jd);
    Rvector3 fixedOut;
    if (toType != "Cartesian")
    {
        fixedOut = Convert(fixedCart, "Cartesian", toType);
    }
    else
    {
        fixedOut = fixedCart;
    }
	
	return fixedOut;
}


//------------------------------------------------------------------------------
// Rvector3 FixedToTopocentric(const Rvector3 &bodyFixedVector,
//                             const Real lat, const Real lon)
//------------------------------------------------------------------------------
/**
 * Converts the input vector from body-fixed to topocentric
 *
 * @param bodyFixedVector     input vector in body-fixed
 * @param lat                 latitude
 * @param lon                 longitude
 *
 * @return vector in topocentric coordinates
 */
//------------------------------------------------------------------------------
Rvector3 Earth::FixedToTopocentric(const Rvector3 &bodyFixedVector,
                                   const Real lat, const Real lon)
{
    Rmatrix33 Rot = FixedToTopo(lat, lon);
    return Rot*bodyFixedVector;
}


//------------------------------------------------------------------------------
//  Rmatrix33 FixedToTopo(Real gdLat, Real gdLon)
//------------------------------------------------------------------------------
/**
 * Returns the rotation matrix to convert from body-fixed to topocentric.
 *
 * @param  gdLat  the geodetic latitude
 * @param  gdLon  the geodetic longitude
 * 
 * @return  the rotation matrix from body-fixed to topocentric
 */
//------------------------------------------------------------------------------
Rmatrix33 Earth::FixedToTopo(Real gdLat, Real gdLon)
{
   // Computes rotation matrix from body fixed to topocentric axes
   // Inputs: int gdLat, int gdLon (geodedic coordinates of topo
   // system).

   // If we're at a pole, topo is undefined so return identity
    if ((GmatMathConstants::PI / 2 - GmatMathUtil::Abs(gdLat)) < 1e-8)
    {
        Rmatrix33 result(1, 0, 0,
            0, 1, 0,
            0, 0, 1);
        return result;
    }

   Rvector3 zHat(GmatMathUtil::Cos(gdLat)*GmatMathUtil::Cos(gdLon),
                 GmatMathUtil::Cos(gdLat)*GmatMathUtil::Sin(gdLon),
                 GmatMathUtil::Sin(gdLat));
   Rvector3 kHat(0.0, 0.0, 1.0);
   Rvector3 yHat = Cross(kHat, zHat);
   yHat.Normalize();
   Rvector3 xHat = Cross(yHat, zHat);
   xHat.Normalize();
   Rmatrix33 result(xHat(0), xHat(1), xHat(2),
                    yHat(0), yHat(1), yHat(2),
                    zHat(0), zHat(1), zHat(2));
   return result;
}

//------------------------------------------------------------------------------
//  Real GeocentricToGeodeticLat(Real gcLat)
//------------------------------------------------------------------------------
/**
 * Converts from a geocentric latitude to a geodetic latitude.
 *
 * @param gcLat  the geocentric latitude
 * 
 * @return  the geodetic latitude
 */
//------------------------------------------------------------------------------
Real Earth::GeocentricToGeodeticLat(Real gcLat)
{
   // Converts from geocentric latitude to geodetic latitude
   Real eSquared  = 2 * flattening - flattening * flattening;
   Real gdLat     = gcLat;
   Real xyPos     = radius * GmatMathUtil::Cos(gcLat);
   Real zPos      = radius * GmatMathUtil::Sin(gcLat);
   Real diff      = 1.0;
   while (diff > 1.0e-10)
   {
      Real phi    = gdLat;
      Real sinPhi = GmatMathUtil::Sin(gdLat);
      Real C      = radius / GmatMathUtil::Sqrt(1 - eSquared * sinPhi*sinPhi);
      gdLat       = GmatMathUtil::ATan(zPos + C * eSquared * sinPhi,xyPos);
      diff        = GmatMathUtil::Abs(gdLat - phi);
   }
   return gdLat;
}

//------------------------------------------------------------------------------
//  void GetEarthSunDistRaDec(Real jd,     Rvector3 &rSun,
//                            Real &rtAsc, Real     &decl)
//------------------------------------------------------------------------------
/**
 * Returns the Earth-Sun distance.
 *
 * @param jd    [in]  the Julian data at which to compute the distance
 * @param rSun  [out] the Earth-to-Sun vector
 * @param rtAsc [out] right ascension
 * @param decl  [out] declination
 * 
 */
//------------------------------------------------------------------------------
void Earth::GetEarthSunDistRaDec(Real jd,     Rvector3 &rSun,
                                 Real &rtAsc, Real     &decl)
{
   // author        : david vallado                  719-573-2600   27 may 2002
   //
   // revisions
   //   vallado     - fix mean lon of sun                            7 mat 2004
   // -------------------  initialize values   --------------------
   Real tut1 = (jd - GmatTimeConstants::JD_OF_J2000) /
               GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   
   Real meanlong = 280.460  + 36000.77 * tut1;
   meanlong      = GmatMathUtil::Rem(meanlong, 360.0);  // deg
   
   Real ttdb= tut1;
   Real meananomaly = 357.5277233  + 35999.05034 * ttdb;
   meananomaly      = GmatMathUtil::Rem(meananomaly *
                                        GmatMathConstants::RAD_PER_DEG,
                                        GmatMathConstants::TWO_PI); // rad
   if ( meananomaly < 0.0  )
      meananomaly += GmatMathConstants::TWO_PI;

   Real eclplong  = meanlong + 1.914666471 * GmatMathUtil::Sin(meananomaly) +
                    0.019994643 * GmatMathUtil::Sin(2.0 * meananomaly); // deg
   eclplong       = GmatMathUtil::Rem(eclplong,360.0) *
                    GmatMathConstants::RAD_PER_DEG;  // deg -> rad
   Real obliquity = (23.439291  - 0.0130042 * ttdb) *
                    GmatMathConstants::RAD_PER_DEG;  // deg -> rad

   // Find magnitude of sun vector components
   Real magr  = 1.000140612  - 0.016708617 * GmatMathUtil::Cos(meananomaly) -
                0.000139589 * GmatMathUtil::Cos(2.0 * meananomaly); // in AUs
   rSun(0)    = magr * GmatMathUtil::Cos(eclplong);
   rSun(1)    = magr * GmatMathUtil::Cos(obliquity) *
                GmatMathUtil::Sin(eclplong);
   rSun(2)    = magr * GmatMathUtil::Sin(obliquity) *
                GmatMathUtil::Sin(eclplong);
   rtAsc      = GmatMathUtil::ATan(GmatMathUtil::Cos(obliquity) *
                GmatMathUtil::Tan(eclplong));

   // Check that rtasc is in the same quadrant as eclplong ----
   // make sure it's in 0 to 2pi range
   if (eclplong < 0.0)
      eclplong += GmatMathConstants::TWO_PI;

   if (GmatMathUtil::Abs(eclplong-rtAsc) > GmatMathConstants::PI*0.5)
      rtAsc = rtAsc + 0.5 * GmatMathConstants::PI *
              GmatMathUtil::Round((eclplong-rtAsc)/
              (0.5 * GmatMathConstants::PI));

   decl = GmatMathUtil::ASin(GmatMathUtil::Sin(obliquity) *
          GmatMathUtil::Sin(eclplong));
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
