//$Id$
//------------------------------------------------------------------------------
//                         FormationFactory
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
// Author: Linda Jun
// Created: 2016.09.21
//
/**
 *  This class is the factory class for Formation.  
 */
//------------------------------------------------------------------------------
#ifndef FormationFactory_hpp
#define FormationFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
//#include "Spacecraft.hpp"

class GMAT_API FormationFactory : public Factory
{
public:
   GmatBase*    CreateObject(const std::string &ofType,
                             const std::string &withName = "");
   SpaceObject* CreateFormation(const std::string &ofType,
                                const std::string &withName = "");
   
   // default constructor
   FormationFactory();
   // constructor
   FormationFactory(StringArray createList);
   // copy constructor
   FormationFactory(const FormationFactory &fact);
   // assignment operator
   FormationFactory& operator= (const FormationFactory &fact);

   // destructor
   ~FormationFactory();

protected:
   // protected data

private:
   // private data


};

#endif // FormationFactory_hpp
