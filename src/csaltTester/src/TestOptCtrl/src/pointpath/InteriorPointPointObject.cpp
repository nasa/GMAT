//------------------------------------------------------------------------------
//                         InteriorPointPointObject
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 16, 2017
/**
 * INSTRUCTIONS:
 *  - Find and Replace "InteriorPoint" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "InteriorPointPointObject.hpp"
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

InteriorPointPointObject::InteriorPointPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
InteriorPointPointObject::InteriorPointPointObject(const InteriorPointPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
InteriorPointPointObject& InteriorPointPointObject::operator=(const InteriorPointPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
InteriorPointPointObject::~InteriorPointPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void InteriorPointPointObject::EvaluateFunctions()
{
   // Extract parameter data
   Real    tInit1         = GetInitialTime(0);
   Real    tFinal1        = GetFinalTime(0);
   Real    tInit2         = GetInitialTime(1);
   Real    tFinal2        = GetFinalTime(1);
   Rvector stateInit1     = GetInitialStateVector(0);
   Rvector stateFinal1    = GetFinalStateVector(0);
   Rvector stateInit2     = GetInitialStateVector(1);
   Rvector stateFinal2    = GetFinalStateVector(1);

   Rvector algFunctions(8);
   algFunctions(0) = tInit1;
   algFunctions(1) = tFinal1;
   algFunctions(2) = tInit2;
   algFunctions(3) = tFinal2;
   algFunctions(4) = stateInit1(0);
   algFunctions(5) = stateFinal1(0);
   algFunctions(6) = stateInit2(0);
   algFunctions(7) = stateFinal2(0);

   Rvector algFuncLower(8, 0.0, 0.75, 0.75, 1.0, 1.0, 0.9, 0.9, 0.75);
   Rvector algFuncUpper(8, 0.0, 0.75, 0.75, 1.0, 1.0, 0.9, 0.9, 0.75);

   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void InteriorPointPointObject::EvaluateJacobians()
{
}
