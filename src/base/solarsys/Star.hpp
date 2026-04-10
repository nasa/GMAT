//$Id$
//------------------------------------------------------------------------------
//                                  Star
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
// Author: Wendy C. Shoan
// Created: 2004/01/29
//
/**
 * This is the class for stars.
 *
 */
//------------------------------------------------------------------------------


#ifndef Star_hpp
#define Star_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

/**
 * Star class, for all stars in the solar system : ).
 *
 * The Star class will contain all data and methods for any star that exists in
 * the solar system.
 */
class GMAT_API Star : public CelestialBody
{
public:
   // default constructor, with optional name
   Star(std::string name = GmatSolarSystemDefaults::SUN_NAME);
   // copy constructor
   Star(const Star &st);
   // operator=
   Star& operator=(const Star &st);
   // destructor
   virtual ~Star();

   Real GetRadiantPower() const;       // W / m^2
   Real GetReferenceDistance() const;  // km

   bool SetRadiantPower(Real radPower, Real refDistance);
   bool SetPhotosphereRadius(Real rad);

   // overridden access methods from CelestialBody
   virtual std::string    GetParameterText(const Integer id) const;
   virtual Integer        GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                          GetParameterType(const Integer id) const;
   virtual std::string    GetParameterTypeString(const Integer id) const;

   virtual Real           GetRealParameter(const Integer id) const;
   virtual Real           SetRealParameter(const Integer id,
                                           const Real value);

   
   virtual bool           IsParameterReadOnly(const Integer id) const;

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   // required method for all subclasses that can be copied in a script
   virtual void      Copy(const GmatBase* orig);
   virtual bool      NeedsOnlyMainSPK();


protected:

   enum
   {
      RADIANT_POWER = CelestialBodyParamCount,
      REFERENCE_DISTANCE,
      PHOTOSPHERE_RADIUS,
      StarParamCount
   };


   static const std::string PARAMETER_TEXT[
      StarParamCount - CelestialBodyParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      StarParamCount - CelestialBodyParamCount];

   // radiant power, reference distance, and photospher radius
   Real      radiantPower;
   Real      referenceDistance;
   Real      photosphereRadius;  // meters

   virtual Rvector6 ComputeTwoBody(const A1Mjd &forTime);

private:

};
#endif // Star_hpp

