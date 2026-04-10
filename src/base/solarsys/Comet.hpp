//$Id$
//------------------------------------------------------------------------------
//                                  Comet
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
// number NNG06CA54C
//
// Author: Wendy C. Shoan
// Created: 2009.01.12
//
/**
 * The Comet class will contain all data and methods for any comet that
 * exists in the solar system.
 *
 */
//------------------------------------------------------------------------------

#ifndef Comet_hpp
#define Comet_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

class GMAT_API Comet : public CelestialBody
{
public:
   // default constructor, with optional name
   Comet(std::string name = "");
   // additional constructor
   Comet(std::string name, const std::string &cBody);
   // copy constructor
   Comet(const Comet &copy);
   // operator=
   Comet& operator=(const Comet &copy);
   // destructor
   virtual ~Comet();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   // required method for all subclasses that can be copied in a script
   virtual void      Copy(const GmatBase* orig);


protected:
   enum
   {
      CometParamCount = CelestialBodyParamCount,
      
   };

private:

};
#endif // Comet_hpp
