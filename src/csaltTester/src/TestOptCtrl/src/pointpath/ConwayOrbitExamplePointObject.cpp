//------------------------------------------------------------------------------
//                         ConwayOrbitExamplePointObject
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
 * Developed based on ConwayOrbitExamplePointObject.m
 */
//------------------------------------------------------------------------------

#include "ConwayOrbitExamplePointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ConwayOrbitExample_POINT

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

ConwayOrbitExamplePointObject::ConwayOrbitExamplePointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ConwayOrbitExamplePointObject::ConwayOrbitExamplePointObject(const ConwayOrbitExamplePointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ConwayOrbitExamplePointObject& ConwayOrbitExamplePointObject::operator=(const ConwayOrbitExamplePointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ConwayOrbitExamplePointObject::~ConwayOrbitExamplePointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void ConwayOrbitExamplePointObject::EvaluateFunctions()
{

   Rvector initStateVec = GetInitialStateVector(0);
   Rvector finalStateVec = GetFinalStateVector(0);

   Real initTime = GetInitialTime(0);
   Real finalTime = GetFinalTime(0);

   Real r = finalStateVec(0);
   Real vr = finalStateVec(2);
   Real vt = finalStateVec(3);

   Rvector costFunction(1);
   costFunction(0) = -(0.5*(vr*vr + vt*vt) - 1.0/r);
   SetFunctions(COST, costFunction);

   Rvector algFunctions(6, initTime, finalTime, initStateVec(0),
         initStateVec(1), initStateVec(2), initStateVec(3));
   Rvector lowerBound(6,0.0, 50.0, 1.1, 0.0, 0.0, 1.0/sqrt(1.1));
   Rvector upperBound(6,0.0, 50.0, 1.1, 0.0, 0.0, 1.0/sqrt(1.1));
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
   SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void ConwayOrbitExamplePointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
