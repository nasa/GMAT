//------------------------------------------------------------------------------
//                         InteriorPointPathObject
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

#include "InteriorPointPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_InteriorPoint_PATH


//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
InteriorPointPathObject::InteriorPointPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
InteriorPointPathObject::InteriorPointPathObject(const InteriorPointPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
InteriorPointPathObject& InteriorPointPathObject::operator=(const InteriorPointPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
InteriorPointPathObject::~InteriorPointPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void InteriorPointPathObject::EvaluateFunctions()
{     
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   Real x = stateVec(0);
   Real u = controlVec(0);

   Rvector dynFunctions(1);
   dynFunctions(0) = u;
   SetFunctions(DYNAMICS, dynFunctions);

   Real cost = x*x + u*u;

   Rvector costFunction(1);
   costFunction(0) = cost;
   SetFunctions(COST, costFunction);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void InteriorPointPathObject::EvaluateJacobians()
{
   Rmatrix ddynState(1,1);
   Rmatrix ddynControl(1,1);
   Rmatrix ddynTime(1,1);

   ddynState(0,0) = 0.0;
   ddynControl(0,0) = 1.0;
   ddynTime(0,0) = 0.0;

   SetJacobian(DYNAMICS, STATE, ddynState);
   SetJacobian(DYNAMICS, CONTROL, ddynControl);
   SetJacobian(DYNAMICS, TIME, ddynTime);
}
