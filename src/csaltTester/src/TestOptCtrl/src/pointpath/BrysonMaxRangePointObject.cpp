//------------------------------------------------------------------------------
//                         BrysonMaxRangePointObject
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
//
/**
 * INSTRUCTIONS:
 *  - Find and Replace "BrysonMaxRange" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "BrysonMaxRangePointObject.hpp"
#include "MessageInterface.hpp"

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

BrysonMaxRangePointObject::BrysonMaxRangePointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BrysonMaxRangePointObject::BrysonMaxRangePointObject(const BrysonMaxRangePointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BrysonMaxRangePointObject& BrysonMaxRangePointObject::operator=(const BrysonMaxRangePointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BrysonMaxRangePointObject::~BrysonMaxRangePointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void BrysonMaxRangePointObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateInit     = GetInitialStateVector(0);
   Rvector stateFinal    = GetFinalStateVector(0);
   Real    tFinal        = GetFinalTime(0);

   Rvector costFunctions(1, -stateFinal(0));
   SetFunctions(COST, costFunctions);

   Rvector algFunctions(5, stateInit(0), stateInit(1), stateInit(2), stateFinal(1), tFinal);
   Rvector algFuncLower(5, 0.0, 0.0, 0.0, 0.1, 2.0);
   Rvector algFuncUpper(5, 0.0, 0.0, 0.0, 0.1, 2.0);
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void BrysonMaxRangePointObject::EvaluateJacobians()
{
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
