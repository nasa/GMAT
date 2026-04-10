//------------------------------------------------------------------------------
//                         RayleighPathObject
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
 * Developed based on RayleighPathObject.m
 */
//------------------------------------------------------------------------------

#include "RayleighPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Rayleigh_PATH

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

RayleighPathObject::RayleighPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RayleighPathObject::RayleighPathObject(const RayleighPathObject &copy) :
   UserPathFunction(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RayleighPathObject& RayleighPathObject::operator=(const RayleighPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RayleighPathObject::~RayleighPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RayleighPathObject::EvaluateFunctions()
{
	// Define constants
	double p = 0.14;

	//Extract parameter data
	Rvector stateVec = GetStateVector();
	Rvector controlVec = GetControlVector();

   Real y2 = -stateVec(0) + stateVec(1)*(1.4 - p * stateVec(1) * stateVec(1)) +
              4.0 * controlVec(0);

   Rvector dynFunctions(2, stateVec(1), y2);
   SetFunctions(DYNAMICS, dynFunctions);

   Real y4 = stateVec(0)*stateVec(0) + controlVec(0)*controlVec(0);

   Rvector costFunction(1,y4);
   SetFunctions(COST, costFunction);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RayleighPathObject::EvaluateJacobians()
{
   // Computes the user Jacobians
   //Does not exist for RayleighPathObject.m
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
