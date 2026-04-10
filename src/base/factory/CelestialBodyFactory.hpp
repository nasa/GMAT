//$Id$
//------------------------------------------------------------------------------
//                         CelestialBodyFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/01/29
//
/**
*  This class is the factory class for CelestialBodys.
 */
//------------------------------------------------------------------------------
#ifndef CelestialBodyFactory_hpp
#define CelestialBodyFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CelestialBody.hpp"

class GMAT_API CelestialBodyFactory : public Factory
{
public:
   CelestialBody*  CreateObject(const std::string &ofType,
                                const std::string &withName = "");
   CelestialBody*  CreateCelestialBody(const std::string &ofType,
                                       const std::string &withName = "");

   // default constructor
   CelestialBodyFactory();
   // constructor
   CelestialBodyFactory(const StringArray &createList);
   // copy constructor
   CelestialBodyFactory(const CelestialBodyFactory &fact);
   // assignment operator
   CelestialBodyFactory& operator= (const CelestialBodyFactory &fact);

   // destructor
   ~CelestialBodyFactory();

protected:
      // protected data

private:
      // private data


};

#endif // CelestialBodyFactory_hpp




