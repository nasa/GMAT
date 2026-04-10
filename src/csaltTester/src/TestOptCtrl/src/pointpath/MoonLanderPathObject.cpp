//------------------------------------------------------------------------------
//                         MoonLanderPathObject
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
 * Developed based on MoonLanderPathObject.m
 */
//------------------------------------------------------------------------------

#include "MoonLanderPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MoonLander_PATH

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

MoonLanderPathObject::MoonLanderPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
MoonLanderPathObject::MoonLanderPathObject(const MoonLanderPathObject &copy) :
   UserPathFunction(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
MoonLanderPathObject& MoonLanderPathObject::operator=(const MoonLanderPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
MoonLanderPathObject::~MoonLanderPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void MoonLanderPathObject::EvaluateFunctions()
{
   // Define constants
   double g = 1.0;
   double E = 2.349;

   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   Real T = controlVec(0);
   Real h = stateVec(0);
   Real v = stateVec(1);
   Real m = stateVec(2);

   Rvector dynFunctions(3, v, -g + T/m, -T/E);
   Rvector algFunctions(4, T, h, v, m);
   Rvector algFuncUpper(4, 1.227, 20.0, 20.0, 1.0);
   Rvector algFuncLower(4, 0.0, -20.0, -20.0, 0.01);

   SetFunctions(DYNAMICS, dynFunctions);
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);

   Rvector costFunction(1, T);
   SetFunctions(COST, costFunction);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void MoonLanderPathObject::EvaluateJacobians()
{
   // Computes the user Jacobians
   //Does not exist for MoonLanderPathObject.m
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
