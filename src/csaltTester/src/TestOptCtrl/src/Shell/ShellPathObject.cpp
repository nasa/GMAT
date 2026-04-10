//------------------------------------------------------------------------------
//                         ShellPathObject
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
 *  - Find and Replace "Shell" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "ShellPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Shell_PATH

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

ShellPathObject::ShellPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ShellPathObject::ShellPathObject(const ShellPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ShellPathObject& ShellPathObject::operator=(const ShellPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ShellPathObject::~ShellPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void ShellPathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   // TODO: Add function math by correctly filling in dynFunction and algFunction
   //       vectors, as applicable. Certain problems may not need one or the other.
   Rvector dynFunctions(1, 0.0);
   SetDynFunctions(dynFunctions);

   Rvector algFunctions(1, 0.0);
   Rvector algFuncUpper(1, 0.0);
   Rvector algFuncLower(1, 0.0);
   SetAlgFunctions(algFunctions);
   SetAlgFunctionsUpperBounds(algFuncUpper);
   SetAlgFunctionsLowerBounds(algFuncLower);

   // If this problem has phase-specific math, uncomment the following block to
   // add it.

   /*
   Integer phaseNum = GetPhaseNumber();
   if (phaseNum == 0)
   {
      //Do a thing
   }
   */
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void ShellPathObject::EvaluateJacobians()
{
   
   // TODO: Add function math by uncommenting the following block and filling in
   //       the jacobian matrices. Some test problems allow EvaluateJacobians to
   //       be blank.

   /*
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();

   Rmatrix dynState(1,1);
   Rmatrix dynControl(1,1);
   Rmatrix dynTime(1,1);

   dynState(0,0) = 0.0;
   dynControl(0,0) = 0.0;
   dynTime(0,0) = 0.0;

   SetDynStateJacobian(dynState);
   SetDynControlJacobian(dynControl);
   SetDynTimeJacobian(dynTime);

   */
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
