//------------------------------------------------------------------------------
//                         BangBangPointObject
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
//
//------------------------------------------------------------------------------

#include "BangBangPointObject.hpp"
#include "MessageInterface.hpp"

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

BangBangPointObject::BangBangPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BangBangPointObject::BangBangPointObject(const BangBangPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BangBangPointObject& BangBangPointObject::operator=(const BangBangPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BangBangPointObject::~BangBangPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void BangBangPointObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateInit     = GetInitialStateVector(0);
   Rvector stateFinal    = GetFinalStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);

   // Set cost functions
   Rvector costFunctions(1, tFinal);
   SetCostFunction(costFunctions);

   // Set algebraic functions
   Rvector algFunctions(6, tInit, tFinal, stateInit(0), stateInit(1),
      stateFinal(0), stateFinal(1));
   Rvector algFuncLower(6, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   Rvector algFuncUpper(6, 0.0, 4.0, 0.0, 0.0, 1.0, 0.0);
   SetAlgFunctions(algFunctions);
   SetAlgLowerBounds(algFuncLower);
   SetAlgUpperBounds(algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void BangBangPointObject::EvaluateJacobians()
{
   // Does nothing for point objects
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
