//$Id$
//------------------------------------------------------------------------------
//                         AxisSystemFactory
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
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy Shoan
// Created: 2004/12/23
//
/**
 *  This class is the factory class for AxisSystem objects.
 *
 */
//------------------------------------------------------------------------------
#ifndef AxisSystemFactory_hpp
#define AxisSystemFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AxisSystem.hpp"

class GMAT_API AxisSystemFactory : public Factory
{
public:
   GmatBase*    CreateObject(const std::string &ofType,
                             const std::string &withName = "");
   AxisSystem*  CreateAxisSystem(const std::string &ofType,
                                 const std::string &withName = "");
 
   // default constructor
   AxisSystemFactory();
   // constructor
   AxisSystemFactory(StringArray createList);
   // copy constructor
   AxisSystemFactory(const AxisSystemFactory& fact);
   // assignment operator
   AxisSystemFactory& operator= (const AxisSystemFactory& fact);

   // destructor
   ~AxisSystemFactory();

protected:
      // protected data

private:


};

#endif // AxisSystemFactory_hpp




