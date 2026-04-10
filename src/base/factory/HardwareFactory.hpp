//$Id$
//------------------------------------------------------------------------------
//                         HardwareFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124
//
// Author: Darrel Conway (Based on SpacecraftFactory)
// Created: 2004/11/10
//
/**
 *  This class is the factory class for hardware.  
 */
//------------------------------------------------------------------------------
#ifndef HardwareFactory_hpp
#define HardwareFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Hardware.hpp"

class GMAT_API HardwareFactory : public Factory
{
public:
   // method to create Hardware object
   virtual GmatBase*    CreateObject(const std::string &ofType,
                                     const std::string &withName = "");        
   virtual Hardware*    CreateHardware(const std::string &ofType,
                                       const std::string &withName = "");
   
   // default constructor
   HardwareFactory();
   // constructor
   HardwareFactory(StringArray createList);
   // copy constructor
   HardwareFactory(const HardwareFactory& fact);
   // assignment operator
   HardwareFactory& operator= (const HardwareFactory& fact);

   // destructor
   ~HardwareFactory();

protected:
   // protected data

private:
   // private data
};

#endif // HardwareFactory_hpp
