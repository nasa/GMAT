//------------------------------------------------------------------------------
//                         ConwaySpiralPointObject
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
// Created: 2017.02.13
//
/**
 * Developed based on ConwaySpiralPointObject.m
 */
//------------------------------------------------------------------------------

#include "ConwaySpiralPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ConwaySpiral_POINT

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

ConwaySpiralPointObject::ConwaySpiralPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ConwaySpiralPointObject::ConwaySpiralPointObject(const ConwaySpiralPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ConwaySpiralPointObject& ConwaySpiralPointObject::operator=(const ConwaySpiralPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ConwaySpiralPointObject::~ConwaySpiralPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void ConwaySpiralPointObject::EvaluateFunctions()
{
//   Rvector stateInit     = GetInitialStateVector(0);
//   Real    tInit         = GetInitialTime(0);
//   Real    tFinal        = GetFinalTime(0);
//
//   Rvector boundVector(4,tInit, tFinal, stateInit(0), stateInit(1));
//   Rvector boundVecLower(4,0, 4.5, -5, -5);
//   Rvector boundVecUpper(4,0, 4.5, -5, -5);
//   SetAlgFunctions(boundVector);
//   SetAlgLowerBounds(boundVecLower);
//   SetAlgUpperBounds(boundVecUpper);

   Rvector finalStateVec = GetFinalStateVector(8);
   Rvector initStateVec = GetInitialStateVector(0);
   Real initTime = GetInitialTime(8);
   Real r = finalStateVec(0);
   Real vr = finalStateVec(2);
   Real vt = finalStateVec(3);

   Real costFunction = -(0.5*(vr*vr + vt*vt) - 1/r);
   SetFunctions(COST, costFunction);

   Rvector algFunctions(3,initTime, finalStateVec(0), initStateVec(0));
   Rvector lowerBound(6,0, 200, 1.1, 0, 0, 1/sqrt(1.1));
   Rvector upperBound(6,0, 300, 1.1, 0, 0, 1/sqrt(1.1));
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
   SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void ConwaySpiralPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
