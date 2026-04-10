//$Id$
//------------------------------------------------------------------------------
//                         BrachistichronePathObject
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
// Author: Jeremy Knittel
// Created: 2016.11.03
//
/**
 * Developed based on BrachistichronePathObject.m
 */
//------------------------------------------------------------------------------

#include "BrachistichronePathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Brachistichrone_PATH

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

BrachistichronePathObject::BrachistichronePathObject() :
   UserPathFunction(),
   gravity(-32.174)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BrachistichronePathObject::BrachistichronePathObject(const BrachistichronePathObject &copy) :
   UserPathFunction(copy),
   gravity(copy.gravity)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BrachistichronePathObject& BrachistichronePathObject::operator=(const BrachistichronePathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   gravity = copy.gravity;
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BrachistichronePathObject::~BrachistichronePathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void BrachistichronePathObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector yVec = GetStateVector();
   Rvector uVec = GetControlVector();
   
#ifdef DEBUG_Brachistichrone_PATH
   MessageInterface::ShowMessage("yVec size = %d\n", yVec.GetSize());
   MessageInterface::ShowMessage("uVec size = %d\n", uVec.GetSize());
#endif
   Real    u = uVec(0);
   Real    y  = yVec(2) * sin(u);
   Real    y2 = yVec(2) * cos(u);
   Real    y3 = gravity * cos(u);
   
#ifdef DEBUG_Brachistichrone_PATH
   MessageInterface::ShowMessage("y  = %12.10f\n", y);
   MessageInterface::ShowMessage("y2 = %12.10f\n", y2);
   MessageInterface::ShowMessage("y3 = %12.10f\n", y3);
   MessageInterface::ShowMessage("uVec = %d\n", uVec(0));
#endif
   
   // Evaluate dynamics and compute Jacobians
   Rvector dynFunctions(3,y, y2, y3);
   SetFunctions(DYNAMICS, dynFunctions);
#ifdef DEBUG_Brachistichrone_PATH
   MessageInterface::ShowMessage("EXITING BrachistichronePathObject::EvaluateFunctions\n");
#endif
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void BrachistichronePathObject::EvaluateJacobians()
{
   // Computes the user Jacobians
   
   Rvector yVec = GetStateVector();
   Rvector uVec = GetControlVector();
   
#ifdef DEBUG_Brachistichrone_PATH
   MessageInterface::ShowMessage("yVec size = %d\n", yVec.GetSize());
   MessageInterface::ShowMessage("uVec size = %d\n", uVec.GetSize());
#endif
   
   Real    dydy3  =  sin(uVec(0));
   Real    dy2dy3 =  cos(uVec(0));
      
   Real    dydu  =  yVec(2) * cos(uVec(0));
   Real    dy2du = -yVec(2) * sin(uVec(0));
   Real    dy3du = -gravity * sin(uVec(0));
   
#ifdef DEBUG_Brachistichrone_PATH
   MessageInterface::ShowMessage("dydy3  = %12.10f\n", dydy3);
   MessageInterface::ShowMessage("dy2dy3 = %12.10f\n", dy2dy3);
   MessageInterface::ShowMessage("dydu  = %12.10f\n", dydu);
   MessageInterface::ShowMessage("dy2du = %12.10f\n", dy2du);
   MessageInterface::ShowMessage("dy3du = %12.10f\n", dy3du);
#endif
   
   Rmatrix dynState(3,3,
	                0.0,0.0,dydy3,
                    0.0,0.0,dy2dy3,
					0.0,0.0,0.0);
   Rmatrix dynControl(3,1,dydu, dy2du, dy3du);
   Rmatrix dynTime(3,1,0.0, 0.0, 0.0);
   
   // Dynamics Function Partials
   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, CONTROL, dynControl);
   SetJacobian(DYNAMICS, TIME, dynTime);
   
#ifdef DEBUG_Brachistichrone_PATH
   MessageInterface::ShowMessage("EXITING BrachistichronePathObject::EvaluateJacobians\n");
#endif
   
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
