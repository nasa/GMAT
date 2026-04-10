//------------------------------------------------------------------------------
//                         LinearTangentSteeringPointObject
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
// Created: Mar 14, 2017
//
/**
 * INSTRUCTIONS:
 *  - Find and Replace "LinearTangentSteering" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "LinearTangentSteeringPointObject.hpp"
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

LinearTangentSteeringPointObject::LinearTangentSteeringPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
LinearTangentSteeringPointObject::LinearTangentSteeringPointObject(const LinearTangentSteeringPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
LinearTangentSteeringPointObject& LinearTangentSteeringPointObject::operator=(const LinearTangentSteeringPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
LinearTangentSteeringPointObject::~LinearTangentSteeringPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void LinearTangentSteeringPointObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateInit     = GetInitialStateVector(0);
   Rvector stateFinal    = GetFinalStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);

   // TODO: Add function math by correctly filling in costFunction and algFunction
   //       vectors, as applicable. Certain problems may not need one or the other.
   Rvector costFunctions(1, tFinal);
   SetFunctions(COST, costFunctions);

   Real x2f = stateFinal(1);
   Real x3f = stateFinal(2);
   Real x4f = stateFinal(3);

   Rvector algFunctions(8, tInit, stateInit(0), stateInit(1), stateInit(2), stateInit(3), x2f, x3f, x4f);
   Rvector algFuncLower(8, 0.0, 0.0, 0.0, 0.0, 0.0, 45.0, 5.0, 0.0);
   Rvector algFuncUpper(8, 0.0, 0.0, 0.0, 0.0, 0.0, 45.0, 5.0, 0.0);
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void LinearTangentSteeringPointObject::EvaluateJacobians()
{
   // Does nothing for point objects
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
