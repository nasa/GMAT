//------------------------------------------------------------------------------
//                         OrbitRaisingPointObject
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
// Author: Wendy Shoan
// Created: 2016.07.21
//
/**
 * Developed based on OrbitRaisingPointObject.m
 */
//------------------------------------------------------------------------------

#include "OrbitRaisingPointObject.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"

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

OrbitRaisingPointObject::OrbitRaisingPointObject() :
UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OrbitRaisingPointObject::OrbitRaisingPointObject(const OrbitRaisingPointObject &copy) :
UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OrbitRaisingPointObject& OrbitRaisingPointObject::operator=(const OrbitRaisingPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OrbitRaisingPointObject::~OrbitRaisingPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void OrbitRaisingPointObject::EvaluateFunctions()
{
   // Define constants
   Real mu   = 1.0;
   
   // Extract state and control components
   Rvector initialStateVec  = GetInitialStateVector(0);
   Rvector finalStateVec    = GetFinalStateVector(0);
   Real    initTime         = GetInitialTime(0);
   Real    finalTime        = GetFinalTime(0);
   Real    r                = finalStateVec(0);
   Real    theta            = finalStateVec(1);
   Real    dr_dt            = finalStateVec(2);
   Real    dtheta_dt        = finalStateVec(3);
   Real    orbitRateError   = GmatMathUtil::Sqrt(mu/r) - dtheta_dt;
   Integer initSize         = initialStateVec.GetSize();
   Integer finalSize        = finalStateVec.GetSize();
   Rvector algFunctions(3 + initSize + finalSize);   // is this part right???
   
   algFunctions(0) = orbitRateError;
   algFunctions(1) = initTime;
   algFunctions(2) = finalTime;
   Integer idx = 3;
   for (Integer ii = 0; ii < initSize; ii++)
      algFunctions(idx++) = initialStateVec(ii);
   for (Integer ii = 0; ii < finalSize; ii++)
      algFunctions(idx++) = finalStateVec(ii);
         
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctions(COST, -r);
   Rvector lower(13, 0.0, 0.0, 3.32, 1.0, 0.0, 0.0, 1.0, 1.0, -1.0, -GmatMathConstants::PI, 0.0, -10.0, 0.0);
   SetFunctionBounds(ALGEBRAIC, LOWER, lower);
   Rvector upper(13, 0.0, 0.0, 3.32, 1.0, 0.0, 0.0, 1.0, 1.0, 10.0, GmatMathConstants::PI, 0.0, 10.0, 1.0);
   SetFunctionBounds(ALGEBRAIC, UPPER, upper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void OrbitRaisingPointObject::EvaluateJacobians()
{
   // currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
