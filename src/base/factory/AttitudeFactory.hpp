//$Id$
//------------------------------------------------------------------------------
//                         AttitudeFactory
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
// Order 124.
//
// Author: Wendy Shoan
// Created: 2006.03.16
//
/**
 *  This class is the factory class for Attitude objects.  
 */
//------------------------------------------------------------------------------
#ifndef AttitudeFactory_hpp
#define AttitudeFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Attitude.hpp"

class GMAT_API AttitudeFactory : public Factory
{
public:
   // method to create Attitude object
   virtual GmatBase*    CreateObject(const std::string &ofType,
                                     const std::string &withName = "");        
   virtual Attitude*    CreateAttitude(const std::string &ofType,
                                       const std::string &withName = "");
   
   // default constructor
   AttitudeFactory();
   // constructor
   AttitudeFactory(StringArray createList);
   // copy constructor
   AttitudeFactory(const AttitudeFactory& fact);
   // assignment operator
   AttitudeFactory& operator= (const AttitudeFactory& fact);

   // destructor
   ~AttitudeFactory();

protected:
   // protected data

private:
   // private data


};

#endif // AttitudeFactory_hpp




