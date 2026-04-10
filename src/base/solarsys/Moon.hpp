//$Id$
//------------------------------------------------------------------------------
//                                  Moon
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
 * The Moon class will contain all data and methods for any moon that exists in
 * the solar system.
 *
 * @note Currently, only contains default data for Luna, Phobos, and Deimos.
 */
//------------------------------------------------------------------------------

#ifndef Moon_hpp
#define Moon_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

class GMAT_API Moon : public CelestialBody
{
public:
   // default constructor, with optional name
   Moon(std::string name = GmatSolarSystemDefaults::MOON_NAME);
   // additional constructor
   Moon(std::string name, const std::string &cBody);
   // copy constructor
   Moon(const Moon &m);
   // operator=
   Moon& operator=(const Moon &m);
   // destructor
   virtual ~Moon();

   virtual Rvector GetBodyCartographicCoordinates(const A1Mjd &forTime) const;
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   // required method for all subclasses that can be copied in a script
   virtual void      Copy(const GmatBase* orig);

   virtual bool      NeedsOnlyMainSPK();
   
   virtual Integer   SetIntegerParameter(const Integer id,
                                         const Integer value);


protected:
   enum
   {
      MoonParamCount = CelestialBodyParamCount,
      
   };

private:

};
#endif // Moon_hpp
