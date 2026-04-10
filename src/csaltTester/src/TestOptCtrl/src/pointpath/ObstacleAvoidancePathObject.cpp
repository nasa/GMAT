//------------------------------------------------------------------------------
//                         ObstacleAvoidancePathObject
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
// Created: Mar 16, 2017
//
/**
 * INSTRUCTIONS:
 *  - Find and Replace "ObstacleAvoidance" with the actual name of your test problem.
 *  - Fill in EvaluateFunctions
 *  - If applicable, fill in EvaluateJacobians
 */
//------------------------------------------------------------------------------

#include "ObstacleAvoidancePathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ObstacleAvoidance_PATH

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

ObstacleAvoidancePathObject::ObstacleAvoidancePathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ObstacleAvoidancePathObject::ObstacleAvoidancePathObject(const ObstacleAvoidancePathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ObstacleAvoidancePathObject& ObstacleAvoidancePathObject::operator=(const ObstacleAvoidancePathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ObstacleAvoidancePathObject::~ObstacleAvoidancePathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void ObstacleAvoidancePathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   // TODO: Add function math by correctly filling in dynFunction and algFunction
   //       vectors, as applicable. Certain problems may not need one or the other.
   Real V = 2.138;
   Real x = stateVec(0);
   Real y = stateVec(1);
   Real theta = controlVec(0);

   Real xdot = V*cos(theta);
   Real ydot = V*sin(theta);

   Rvector dynFunctions(2, xdot, ydot);
   SetFunctions(DYNAMICS, dynFunctions);

   Real con1 = (x - 0.4)*(x - 0.4) + (y - 0.5)*(y - 0.5);
   Real con2 = (x - 0.8)*(x - 0.8) + (y - 1.5)*(y - 1.5);

   Rvector algFunctions(2, con1, con2);
   Rvector algFuncUpper(2, 100.0, 100.0);
   Rvector algFuncLower(2, 0.1, 0.1);
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);

   // Compute the integrand of the cost function
   Rvector cost(1, xdot*xdot + ydot*ydot);
   SetFunctions(COST, cost);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void ObstacleAvoidancePathObject::EvaluateJacobians()
{
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
