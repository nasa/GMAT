//------------------------------------------------------------------------------
//                         SchwartzPointObject
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
// Created: Feb 22, 2017
/**
 * Point functions for the Schwartz test case
 */
//------------------------------------------------------------------------------

#include "MessageInterface.hpp"
#include "SchwartzPointObject.hpp"

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
SchwartzPointObject::SchwartzPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
SchwartzPointObject::~SchwartzPointObject()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
SchwartzPointObject::SchwartzPointObject(const SchwartzPointObject &copy) :
   UserPointFunction(copy)
{
}

//------------------------------------------------------------------------------
// Assignment operator
//------------------------------------------------------------------------------
SchwartzPointObject& SchwartzPointObject::operator=(const SchwartzPointObject &copy)
{
   if (&copy != this)
   {
      UserPointFunction::operator=(copy);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void SchwartzPointObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector statePhase1Initial  = GetInitialStateVector(0);
   Rvector statePhase2Initial  = GetInitialStateVector(1);
   Rvector statePhase1Final    = GetFinalStateVector(0);
   Rvector statePhase2Final    = GetFinalStateVector(1);
   Real fsp2_1 = statePhase2Final(0);
   Real fsp2_2 = statePhase2Final(1);

   Rvector costFunc(1);
   costFunc(0) = 5.0 * (fsp2_1*fsp2_1 + fsp2_2*fsp2_2);
   
   Rvector boundCon(9);
   boundCon(0) = GetInitialTime(0);
   boundCon(1) = GetFinalTime(0);
   boundCon(2) = statePhase1Initial(0);
   boundCon(3) = statePhase1Initial(1);
   boundCon(4) = GetInitialTime(1);
   boundCon(5) = GetFinalTime(1);
   
   // Link phase 1 and phase 2 together by matching times and state values
   boundCon(6) = GetFinalTime(0) - GetInitialTime(1);
   boundCon(7) = statePhase1Final(0) - statePhase2Initial(0);
   boundCon(8) = statePhase1Final(1) - statePhase2Initial(1);

   
   Rvector lower(9, 0.0, 1.0, 1.0, 1.0, 1.0, 2.9, 0.0, 0.0, 0.0);
   Rvector upper(9, 0.0, 1.0, 1.0, 1.0, 1.0, 2.9, 0.0, 0.0, 0.0);
   
   SetFunctions(COST, costFunc);
   SetFunctions(ALGEBRAIC, boundCon);
   SetFunctionBounds(ALGEBRAIC, LOWER, lower);
   SetFunctionBounds(ALGEBRAIC, UPPER, upper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void SchwartzPointObject::EvaluateJacobians()
{
   // Does nothing
}
