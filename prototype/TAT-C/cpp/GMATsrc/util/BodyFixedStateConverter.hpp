//$Id: $
//------------------------------------------------------------------------------
//                         BodyFixedStateConverter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy Shoan, GSFC/GSSB
// Created: 2010.08.24
//
/**
 * Definition of the namespace containing methods to convert between
 * celestial-body-fixed (CBF) state representations.
 *
 * @Note  Cartesian states are (x,y,z)
 *        Spherical and Spherical-Ellipsoid states are (latitude, longitude, height)
 */
//------------------------------------------------------------------------------


#ifndef BodyFixedStateConverter_hpp
#define BodyFixedStateConverter_hpp

//#include "CelestialBody.hpp"
#include "utildefs.hpp"
#include "Rvector3.hpp"

class GMATUTIL_API InvalidStateRepresentationException : public BaseException
{
   public:
   InvalidStateRepresentationException(const std::string &message =
      "BodyFixedStateConverter: Conversion to invalid state representation requested: ")
      : BaseException(message) {};
};

namespace BodyFixedStateConverterUtil
{
   static const Integer     NUM_STATE_REPRESENTATIONS = 3;
   static const std::string BODY_FIXED_STATE_REPRESENTATION_TEXT[NUM_STATE_REPRESENTATIONS] =
   {
      "Cartesian",
      "Spherical",
      "Ellipsoid"
      // future representations here
   };

   Rvector3 GMATUTIL_API Convert(const Rvector3    &origValue, const std::string &fromType, const std::string &toType,
                    const Real        flattening, const Real        meanRadius);

   Rvector3 GMATUTIL_API Convert(const Rvector3    &origValue,
                    const std::string &fromType,  const std::string &fromHorizon,
                    const std::string &toType,    const std::string &toHorizon,
                    const Real        flattening, const Real        meanRadius);

   Rvector3 GMATUTIL_API CartesianToSpherical(const Rvector3 &cart, const Real flattening, const Real meanRadius);

   Rvector3 GMATUTIL_API SphericalToCartesian(const Rvector3 &spherical, const Real flattening, const Real meanRadius);

   Rvector3 GMATUTIL_API SphericalEllipsoidToCartesian(const Rvector3 &sphEll, const Real flattening, const Real meanRadius);

   Rvector3 GMATUTIL_API CartesianToSphericalEllipsoid(const Rvector3 &cart, const Real flattening, const Real meanRadius);

   Rvector3 GMATUTIL_API SphericalToSphericalEllipsoid(const Rvector3 &spherical, const Real flattening, const Real meanRadius);

   Rvector3 GMATUTIL_API SphericalEllipsoidToSpherical(const Rvector3 &sphEll, const Real flattening, const Real meanRadius);

   bool GMATUTIL_API IsValidStateRepresentation(const std::string &rep);
   StringArray GMATUTIL_API GetValidRepresentations();

}

#endif /* BodyFixedStateConverter_hpp */
