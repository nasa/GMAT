//------------------------------------------------------------------------------
//                         RayleighControlStateConstraintPointObject
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
 * Developed based on RayleighControlStateConstraintPointObject.m
 */
//------------------------------------------------------------------------------

#include "RayleighControlStateConstraintPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RayleighControlStateConstraint_POINT

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

RayleighControlStateConstraintPointObject::RayleighControlStateConstraintPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RayleighControlStateConstraintPointObject::RayleighControlStateConstraintPointObject(const RayleighControlStateConstraintPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RayleighControlStateConstraintPointObject& RayleighControlStateConstraintPointObject::operator=(const RayleighControlStateConstraintPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RayleighControlStateConstraintPointObject::~RayleighControlStateConstraintPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RayleighControlStateConstraintPointObject::EvaluateFunctions()
{
   Rvector stateInit     = GetInitialStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);

   Rvector boundVector(4,tInit, tFinal, stateInit(0), stateInit(1));
   Rvector boundVecLower(4,0.0, 4.5, -5.0, -5.0);
   Rvector boundVecUpper(4,0.0, 4.5, -5.0, -5.0);
   SetFunctions(ALGEBRAIC, boundVector);
   SetFunctionBounds(ALGEBRAIC, LOWER, boundVecLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, boundVecUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RayleighControlStateConstraintPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
