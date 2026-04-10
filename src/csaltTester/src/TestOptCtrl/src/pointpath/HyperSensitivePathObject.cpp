//------------------------------------------------------------------------------
//                         HyperSensitivePathObject
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
// Created: 2016.10.17
//
/**
 * Developed based on HyperSensitivePathObject.m
 */
//------------------------------------------------------------------------------

#include "HyperSensitivePathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_HYPERSENSITIVE_PATH

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

HyperSensitivePathObject::HyperSensitivePathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
HyperSensitivePathObject::HyperSensitivePathObject(const HyperSensitivePathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
HyperSensitivePathObject& HyperSensitivePathObject::operator=(const HyperSensitivePathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
HyperSensitivePathObject::~HyperSensitivePathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void HyperSensitivePathObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector yVec          = GetStateVector();
   Rvector uVec          = GetControlVector();
   //Real    t             = GetTime();

   Real    y  = yVec(0);
   Real    y2 = y  * y;
   Real    y3 = y2 * y;

   Real    u  = uVec(0);
   Real    u2 = u * u;
   
   // Evaluate cost function
   Rvector costF(1, y2 + u2);
   SetFunctions(COST, costF);
   
   // Evaluate dynamics and compute Jacobians
   Rvector dynFunctions(1, -y3 + u);
   SetFunctions(DYNAMICS, dynFunctions);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void HyperSensitivePathObject::EvaluateJacobians()
{
   // Computes the user Jacobians
   
   Rvector y          = GetStateVector();
   Rvector u          = GetControlVector();
   Real    t          = GetTime();
   Real timeFactor = 0.0;
   
   Rmatrix yJac(1,y.GetSize());

   for (Integer ii = 0; ii < y.GetSize(); ii++)
      yJac(0,ii) = 2.0 * y(ii);

   Rmatrix uJac(1,u.GetSize());
   for (Integer ii = 0; ii < u.GetSize(); ii++)
      uJac(0,ii) = 2.0 * u(ii);

   Rmatrix tJac(1,1);
   tJac(0,0) = timeFactor/100.0;
   
   // Cost Function Partials
   SetJacobian(COST, STATE, yJac*2.0);
   SetJacobian(COST, CONTROL, uJac*2.0);
   SetJacobian(COST, TIME, tJac);
   
   Rmatrix dynState(1,1, -3.0 * y(0) * y(0));
   Rmatrix dynControl(1,1, 1.0);
   Rmatrix dynTime(1,1, timeFactor*3.0*t*t/100.0);
   
   // Dynamics Function Partials
   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, CONTROL, dynControl);
   SetJacobian(DYNAMICS, TIME, dynTime);
   
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
