//------------------------------------------------------------------------------
//                           UserFunctionManager
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
// Created: 2015.11.20
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserFunctionManager_hpp
#define UserFunctionManager_hpp

#include "csaltdefs.hpp"
#include <random>
#include "FunctionOutputData.hpp"
#include "UserPathFunction.hpp"
#include "Rvector.hpp"

class UserFunctionManager
{
public:
   
   UserFunctionManager();
   UserFunctionManager(const UserFunctionManager &copy);
   UserFunctionManager& operator=(const UserFunctionManager &copy);
   virtual ~UserFunctionManager();
   
   Rvector      GetRandomVector(const Rvector &lowerBound,
                                const Rvector &upperBound);
   
   virtual void SetIsInitializing(bool isInit);
   virtual bool IsInitializing();
   
   virtual bool HasCostFunction();
   
protected:
   
   /// Pointer to the FunctionOutputData
   UserPathFunction   *userData;
   /// The phase ID number
   Integer            phaseNum;
   /// Flag indicating whether the user provided a function
   bool               hasFunction;
   /// Number of state parameters
   Integer            numStateVars;
   /// Number of controls
   Integer            numControlVars;
   /// Number of time variables
   Integer            numTimeVars;
   /// Number of static parameters
   Integer            numStaticVars;
   /// Flag indicating testing of user-provided derivatives
   bool               forceFiniteDiff;
   /// Indicates if user has cost component
   bool               hasCostFunction;
   /// Flag to control some actions during initialization
   bool               isInitializing;
   
   /// Random number generator
   std::default_random_engine randGen;
   /// Distribution setting for the random number generator, will be 
   /// set to use real numbers between 0 and 1
   std::uniform_real_distribution<Real> realDistribution;
};
#endif // UserFunctionManager_hpp

