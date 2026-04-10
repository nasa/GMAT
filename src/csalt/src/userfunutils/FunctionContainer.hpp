//------------------------------------------------------------------------------
//                              FunctionContainer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Wendy Shoan / NASA
// Created: 2015.07.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 *  FUNCTIONCONTAINER is a container class for data structures passed
 *  to UserFunctions and returned containing user data.  This class
 *  basically helps keep the user interfaces clean as the amount of I/O
 *  data increases by bundling all data into a single class.
 */
//------------------------------------------------------------------------------

#ifndef FunctionContainer_hpp
#define FunctionContainer_hpp

#include "csaltdefs.hpp"
#include "FunctionOutputData.hpp"
#include "LowThrustException.hpp"

class FunctionContainer
{

public:
   // class methods
   FunctionContainer();
   FunctionContainer(const FunctionContainer& copy);
   FunctionContainer& operator=(const FunctionContainer &copy);
   virtual ~FunctionContainer();
   
   
   // Intialize the object <abstract>
   virtual void                Initialize();

   virtual FunctionOutputData* GetCostData();
   virtual FunctionOutputData* GetAlgData();

   //YK mod vectorization
   virtual FunctionOutputData* GetData(UserFunction::FunctionType idx)
   {
      if (idx == UserFunction::ALGEBRAIC)
         return algData;
      else if (idx == UserFunction::COST)
         return costData;
      else
         throw LowThrustException("Error getting function output "
            "data in FunctionContainer, invalid functiontype received.  "
            "Valid function types are ALGEBRAIC and COST.\n");
   };
   
protected:
   FunctionOutputData *costData;
   FunctionOutputData *algData;
   
};
#endif // FunctionContainer_hpp

