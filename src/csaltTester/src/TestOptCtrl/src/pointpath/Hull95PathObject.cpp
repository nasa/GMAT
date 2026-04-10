//------------------------------------------------------------------------------
//                         Hull95PathObject
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
 *  - Find and Replace "Hull95" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "Hull95PathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Hull95_PATH

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

Hull95PathObject::Hull95PathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
Hull95PathObject::Hull95PathObject(const Hull95PathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
Hull95PathObject& Hull95PathObject::operator=(const Hull95PathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Hull95PathObject::~Hull95PathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void Hull95PathObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   Real x = stateVec(0);
   Real u = controlVec(0);

   Rvector dynFunctions(1, u);
   SetFunctions(DYNAMICS, dynFunctions);
   Rvector cost(1, u*u - x);
   SetFunctions(COST, cost);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void Hull95PathObject::EvaluateJacobians()
{
   Rvector controlVec = GetControlVector();
   Real u = controlVec(0);

   Rmatrix zeroState(1,1);
   Rmatrix oneMatrix(1,1);
   Rmatrix ccMatrix(1,1);

   zeroState(0,0) = 0.0;
   oneMatrix(0,0) = 1.0;
   ccMatrix(0,0) = 2.0 * u;

   SetJacobian(DYNAMICS, STATE, zeroState);
   SetJacobian(DYNAMICS, CONTROL, oneMatrix);
   oneMatrix(0,0) = -1.0;
   SetJacobian(COST, STATE, oneMatrix);
   SetJacobian(COST, CONTROL, ccMatrix);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
