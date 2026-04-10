//------------------------------------------------------------------------------
//                         BrysonMaxRangePathObject
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Mar 13, 2017
/**
 * INSTRUCTIONS:
 *  - Find and Replace "BrysonMaxRange" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "BrysonMaxRangePathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BrysonMaxRange_PATH

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------

BrysonMaxRangePathObject::BrysonMaxRangePathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BrysonMaxRangePathObject::BrysonMaxRangePathObject(const BrysonMaxRangePathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BrysonMaxRangePathObject& BrysonMaxRangePathObject::operator=(const BrysonMaxRangePathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BrysonMaxRangePathObject::~BrysonMaxRangePathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void BrysonMaxRangePathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   //Real x = stateVec(0);
   //Real y = stateVec(1);
   Real v = stateVec(2);
   Real u1 = controlVec(0);
   Real u2 = controlVec(1);

   Real g = 1.0;
   Real a = 0.5*g;

   Rvector dynFunctions(3, v * u1, v * u2, a * g - u2);
   SetFunctions(DYNAMICS, dynFunctions);

   Rvector algFunctions(1, u1*u1 + u2*u2);
   Rvector algFuncUpper(1, 1.0);
   Rvector algFuncLower(1, 1.0);

   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void BrysonMaxRangePathObject::EvaluateJacobians()
{
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
