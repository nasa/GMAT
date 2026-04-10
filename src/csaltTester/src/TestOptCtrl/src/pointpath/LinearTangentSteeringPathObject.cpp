//------------------------------------------------------------------------------
//                         LinearTangentSteeringPathObject
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

#include "LinearTangentSteeringPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LinearTangentSteering_PATH

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

LinearTangentSteeringPathObject::LinearTangentSteeringPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
LinearTangentSteeringPathObject::LinearTangentSteeringPathObject(const LinearTangentSteeringPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
LinearTangentSteeringPathObject& LinearTangentSteeringPathObject::operator=(const LinearTangentSteeringPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
LinearTangentSteeringPathObject::~LinearTangentSteeringPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void LinearTangentSteeringPathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   // TODO: Add function math by correctly filling in dynFunction and algFunction
   //       vectors, as applicable. Certain problems may not need one or the other.
   Real x2 = stateVec(1);
   Real x4 = stateVec(3);
   Real u = controlVec(0);
   Real a = 100.0;

   Rvector dynFunctions(4, x2, a*cos(u), x4, a*sin(u));
   SetFunctions(DYNAMICS, dynFunctions);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void LinearTangentSteeringPathObject::EvaluateJacobians()
{
   
   // TODO: Add function math by uncommenting the following block and filling in
   //       the jacobian matrices. Some test problems allow EvaluateJacobians to
   //       be blank.

   
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();

   Real u = controlVec(0);
   Real a = 100.0;

   Rmatrix dynState(4,4);
   Rmatrix dynControl(4,1);
   Rmatrix dynTime(4,1);

   dynState(0,0) = 0.0;
   dynState(0,1) = 1.0;
   dynState(0,2) = 0.0;
   dynState(0,3) = 0.0;
   dynState(1,0) = 0.0;
   dynState(1,1) = 0.0;
   dynState(1,2) = 0.0;
   dynState(1,3) = 0.0;
   dynState(2,0) = 0.0;
   dynState(2,1) = 0.0;
   dynState(2,2) = 0.0;
   dynState(2,3) = 1.0;
   dynState(3,0) = 0.0;
   dynState(3,1) = 0.0;
   dynState(3,2) = 0.0;
   dynState(3,3) = 0.0;

   dynControl(0,0) = 0.0;
   dynControl(1,0) = -a*sin(u);
   dynControl(2,0) = 0.0;
   dynControl(3,0) = a*cos(u);

   dynTime(0,0) = 0.0;
   dynTime(1,0) = 0.0;
   dynTime(2,0) = 0.0;
   dynTime(3,0) = 0.0;

   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, CONTROL, dynControl);
   SetJacobian(DYNAMICS, TIME, dynTime);

   
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
