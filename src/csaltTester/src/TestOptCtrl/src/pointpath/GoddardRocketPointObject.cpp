//------------------------------------------------------------------------------
//                         GoddardRocketPointObject
//------------------------------------------------------------------------------
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
// Author: Claire Conway
// Created: 2017.01.30
//
/**
 * Developed based on GoddardRocketPointObject.m
 */
//------------------------------------------------------------------------------

#include "GoddardRocketPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_GoddardRocket_POINT

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

GoddardRocketPointObject::GoddardRocketPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
GoddardRocketPointObject::GoddardRocketPointObject(const GoddardRocketPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
GoddardRocketPointObject& GoddardRocketPointObject::operator=(const GoddardRocketPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
GoddardRocketPointObject::~GoddardRocketPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void GoddardRocketPointObject::EvaluateFunctions()
{
   Rvector stateVeclf = GetFinalStateVector(0);
   Rvector stateInit = GetInitialStateVector(0);
   Real tInit = GetInitialTime(0);
   Real tFinal = GetFinalTime(0);

   Rvector costFunction(1, -stateVeclf(0));
   Rvector algFunction(8, tInit, tFinal, stateInit(0), stateInit(1), stateInit(2), stateVeclf(0), stateVeclf(1), stateVeclf(2));

   //psopt
   Rvector boundVecLower(8, 0.0, 0.1, 1.0, 0.0, 1.0, 1.0, 0.0, 0.6);
   Rvector boundVecUpper(8, 0.0, 1.0, 1.0, 0.0, 1.0, 1e5, 0.0, 0.6);

   //gpops
   //Rvector boundVecLower(8, 0.0, 0.1, 0.0, 0.0, 3.0, 1.0, 0.0, 1.0);
   //Rvector boundVecUpper(8, 0.0, 1e10, 0.0, 0.0, 3.0, 1e10, 0.0, 1.0);

   SetFunctions(COST, costFunction);
   SetFunctions(ALGEBRAIC, algFunction);
   SetFunctionBounds(ALGEBRAIC, LOWER, boundVecLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, boundVecUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void GoddardRocketPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
