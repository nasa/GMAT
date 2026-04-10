//------------------------------------------------------------------------------
//                          PathFunctionContainer
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
 *  PATHFUNCTIONCONTAINER is a container class for data structures passed
 *  to UserPathFunctions and returned containing user data.  This class
 *  basically helps keep the user interfaces clean as the amount of I/O
 *  data increases by bundling all data into a single class.
 */
//------------------------------------------------------------------------------

#ifndef PathFunctionContainer_hpp
#define PathFunctionContainer_hpp

#include "csaltdefs.hpp"
#include "FunctionContainer.hpp"
#include "FunctionOutputData.hpp"
#include "LowThrustException.hpp"

class PathFunctionContainer : public FunctionContainer
{
   
public:
   // class methods
   PathFunctionContainer();
   PathFunctionContainer(const PathFunctionContainer& copy);
   PathFunctionContainer& operator=(const PathFunctionContainer &copy);
   virtual ~PathFunctionContainer();
   
   
   // Intialize the object
   virtual void                Initialize();
 
   virtual FunctionOutputData* GetDynData();

   //YK mod vectorization
   virtual FunctionOutputData* GetData(UserFunction::FunctionType idx)
   {
      if (idx == UserFunction::DYNAMICS)
           return dynData;
      else if (idx == UserFunction::ALGEBRAIC)
           return algData;
      else if (idx == UserFunction::COST)
           return costData;
      else
         throw LowThrustException("Error getting path function output "
            "data in PathFunctionContainer, invalid functiontype received.  "
            "Valid function types are DYNAMICS, ALGEBRAIC, and COST.\n");
   };

protected:
   FunctionOutputData *dynData;
   
};
#endif // FunctionContainer_hpp
