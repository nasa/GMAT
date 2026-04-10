//$Id$
//------------------------------------------------------------------------------
//                         SolarSystemFactory
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
*  This class is the factory class for SolarSystems.
 */
//------------------------------------------------------------------------------
#ifndef SolarSystemFactory_hpp
#define SolarSystemFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "SolarSystem.hpp"

class GMAT_API SolarSystemFactory : public Factory
{
public:
   GmatBase*     CreateObject(const std::string &ofType,
                              const std::string &withName = "");
   SolarSystem*  CreateSolarSystem(const std::string &ofType,
                                   const std::string &withName = "");

   // default constructor
   SolarSystemFactory();
   // constructor
   SolarSystemFactory(StringArray createList);
   // copy constructor
   SolarSystemFactory(const SolarSystemFactory& fact);
   // assignment operator
   SolarSystemFactory& operator= (const SolarSystemFactory& fact);

   // destructor
   ~SolarSystemFactory();

protected:
      // protected data

private:
      // private data


};

#endif // SolarSystemFactory_hpp




