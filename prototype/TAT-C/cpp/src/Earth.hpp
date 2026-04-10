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
 * Definition of the Earth class.  This class is a simple model of the Earth.
 */
//------------------------------------------------------------------------------
#ifndef Earth_hpp
#define Earth_hpp

#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "AbsoluteDate.hpp"
#include "BodyFixedStateConverter.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"

class Earth
{
public:
   
   /// class construction/destruction
   Earth();
   Earth( const Earth &copy);
   Earth& operator=(const Earth &copy);
   
   virtual ~Earth();
   
   /// Get the inertial-to-fixed rotation matrix
   virtual Rmatrix33        GetInertialToFixedRotation(Real jd);
   /// Compute the Greenwich Mean Time
   virtual Real             ComputeGMT(Real jd);
   /// Get the body-fixed state
   virtual Rvector3         GetBodyFixedState(Rvector3 inertialState,
                                              Real      jd);
   /// Convert between body-fixed representations
   virtual Rvector3         Convert (const Rvector3 &origValue,
                                     const std::string &fromType,
                                     const std::string &toType);

   /// Convert the input vector from inertial to body-fixed
   virtual Rvector3         InertialToBodyFixed(const Rvector3 &inertialVector,
                                                Real jd,
	                                             const std::string &toType);
   /// COnvert the input vecgor from body-fixed to topocentric
   virtual Rvector3         FixedToTopocentric(const Rvector3 &inertialVector,
                            const Real lat, const Real lon);

   /// Get the Sun position in body coordinates
   virtual Rvector3         GetSunPositionInBodyCoords(
                                           Real jd,
                                           const std::string &toType);
   
   /// Compute the body-fixed-to-topocentric rotaiton matrix
   virtual Rmatrix33        FixedToTopo(Real gdLat, Real gdLon);
   /// Convert geocentric latitude to geodetic latitude
   virtual Real             GeocentricToGeodeticLat(Real gcLat);
   /// Get the Earth-Sun distance
   virtual void             GetEarthSunDistRaDec(Real jd,     Rvector3 &rSun,
                                                 Real &rtAsc, Real     &decl);
   /// Get the mean equatorial radius
   Real                     GetRadius();
   
protected:
   
   /// J2 term for Earth
   Real      J2;      // units??
   /// Gravitational parameter of the Earth
   Real      mu;       // units??
   /// Equatorial radius of the Earth
   Real      radius;  // km
   /// Flattening of the Earth
   Real      flattening;
   
   /// Local class data for performance
   Rmatrix33 rotationResult;
   /// Save the last computd rotation time, for performance
   Real      lastRotationTime;
};
#endif // Earth_hpp
