//------------------------------------------------------------------------------
//                              PathFuncProperties
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
// Author: S. Hughes / NASA
// Created: 2015.07.20
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef PathFuncProperties_hpp
#define PathFuncProperties_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"

class PathFuncProperties 
{

public:
   
   PathFuncProperties();
   PathFuncProperties(const PathFuncProperties &copy);
   PathFuncProperties& operator=(const PathFuncProperties &copy);
   virtual ~PathFuncProperties();
   
   void  SetControlJacPattern(const Rmatrix jacPattern);
   void  SetStateJacPattern(const Rmatrix jacPattern);
   void  SetTimeJacPattern(const Rmatrix jacPattern);
   void  SetNumFunctions(const Integer numFunctions);
   void  SetHasStateVars(const bool hasVars);
   void  SetHasControlVars(const bool hasVars);

protected:

   /// state Jacobian pattern
   Rmatrix stateJacPattern;
   /// time Jacobian pattern
   Rmatrix timeJacPattern;
   ///  control Jacobian pattern
   Rmatrix controlJacPattern;
   ///  number of functions
   Integer numFunctions;
   ///  indicates if problem has state variables
   bool hasStateVars;
   ///  indicates if problem has control variables
   bool hasControlVars;
   
};
#endif // PathFuncProperties_hpp
