//------------------------------------------------------------------------------
//                         SchwartzPathObject
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
 * Path function for the Schwartz test problem
 */
//------------------------------------------------------------------------------

#include "SchwartzPathObject.hpp"
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

SchwartzPathObject::SchwartzPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
SchwartzPathObject::~SchwartzPathObject()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
SchwartzPathObject::SchwartzPathObject(const SchwartzPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// Assignment operator
//------------------------------------------------------------------------------
SchwartzPathObject& SchwartzPathObject::operator=(const SchwartzPathObject &copy)
{
   if (&copy != this)
   {
      UserPathFunction::operator=(copy);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void SchwartzPathObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();
   
   Real x1 = stateVec(0);
   Real u  = controlVec(0);
   Real x2 = stateVec(1);
   
   // Evaluate dynamics and compute Jacobians
   Rvector dynFunctions(2);
   dynFunctions(0) = x2;
   dynFunctions(1) = u - 0.1 * (1.0 + 2.0*x1*x1) * x2;
   
   SetFunctions(DYNAMICS, dynFunctions);

   // Compute algebraic path constraints for first phase
   Integer phaseNum = GetPhaseNumber();
   if (phaseNum == 0) //1)
   {
       // Algebraic path constraints for phase 1
       Rvector c1(1), ub(1), lb(1);

       c1(0) = 1.0 - 9.0 * (x1 - 1.0) * (x1 - 1.0) -
             ((x2 - 0.4)/0.3) * ((x2 - 0.4)/0.3);
       lb(0) = -1.0e16;
       ub(0) = 0.0;

       SetFunctions(ALGEBRAIC, c1);
       SetFunctionBounds(ALGEBRAIC, UPPER, ub);
       SetFunctionBounds(ALGEBRAIC, LOWER, lb);
   }
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void SchwartzPathObject::EvaluateJacobians()
{
   Rvector stateVec = GetStateVector();
   Real x1 = stateVec(0);
   Real x2 = stateVec(1);

   // Set Dynamics Function Jacobians
   Rmatrix dynState(2,2), dynControl(2,1);
   
   dynState(0,0) = 0.0;
   dynState(0,1) = 1.0;
   dynState(1,0) = -0.4 * x1 * x2;
   dynState(1,1) = -0.1 * (1.0 + 2.0 * x1 * x1);
   

   dynControl(0,0) = 0.0;
   dynControl(1,0) = 1.0;
   
   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, CONTROL, dynControl);

   // Set alg path constraint Jacobian
   Integer phaseNum = GetPhaseNumber();
   if (phaseNum == 1)
   {
      // Jacobian for the algebraic path constraints
      Rmatrix dcondx(1,2);
      dcondx(0,0) = - 18.0 *(x1 - 1.0);
      dcondx(0,1) = - 2.0 * ((x2 - 0.4) / 0.3) / 0.3;
      SetJacobian(ALGEBRAIC, STATE, dcondx);
   }
}
