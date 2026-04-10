//------------------------------------------------------------------------------
//                         BangBangPathObject
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
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Mar 20, 2018
//------------------------------------------------------------------------------

#include "BangBangPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BangBang_PATH

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

BangBangPathObject::BangBangPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BangBangPathObject::BangBangPathObject(const BangBangPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BangBangPathObject& BangBangPathObject::operator=(const BangBangPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BangBangPathObject::~BangBangPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void BangBangPathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   // Set dynamic functions
   Real x = stateVec(0);
   Real y = stateVec(1);
   Real u = controlVec(0);

   Rvector dynFunctions(2, y, u);
   SetDynFunctions(dynFunctions);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void BangBangPathObject::EvaluateJacobians()
{
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector controlVec        = GetControlVector();
   
   // Set various Jacobians
   Rmatrix dynState(2,2,
      0.0, 1.0,
      0.0, 0.0);
   Rmatrix dynControl(2,1,
      0.0,
      1.0);
   Rmatrix dynTime(2,1,
      0.0,
      0.0);

   SetDynStateJacobian(dynState);
   SetDynControlJacobian(dynControl);
   SetDynTimeJacobian(dynTime);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
