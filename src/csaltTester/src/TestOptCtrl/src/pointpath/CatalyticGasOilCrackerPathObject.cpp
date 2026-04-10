//------------------------------------------------------------------------------
//                         CatalyticGasOilCrackerPathObject
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

#include "CatalyticGasOilCrackerPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CatalyticGasOilCracker_PATH

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

CatalyticGasOilCrackerPathObject::CatalyticGasOilCrackerPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CatalyticGasOilCrackerPathObject::CatalyticGasOilCrackerPathObject(const CatalyticGasOilCrackerPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CatalyticGasOilCrackerPathObject& CatalyticGasOilCrackerPathObject::operator=(const CatalyticGasOilCrackerPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CatalyticGasOilCrackerPathObject::~CatalyticGasOilCrackerPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void CatalyticGasOilCrackerPathObject::EvaluateFunctions()
{     
   // Extract parameter data
   Rvector stateVec          = GetStateVector();
   Rvector staticVec        = GetStaticVector();
   
   // Set dynamic functions
   Real y1 = stateVec(0);
   Real y2 = stateVec(1);
   Real theta1 = staticVec(0);
   Real theta2 = staticVec(1);
   Real theta3 = staticVec(2);

   Rvector dynFunctions(2);

   dynFunctions(0) = -(theta1 + theta3)*y1*y1;
   dynFunctions(1) = theta1*y1*y1 - theta2*y2;

   SetDynFunctions(dynFunctions);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void CatalyticGasOilCrackerPathObject::EvaluateJacobians()
{
   // Extract parameter data
   Rvector stateVec = GetStateVector();
   Rvector staticVec = GetStaticVector();

   // Set dynamic functions
   Real y1 = stateVec(0);
   Real y2 = stateVec(1);
   Real theta1 = staticVec(0);
   Real theta2 = staticVec(1);
   Real theta3 = staticVec(2);

   // Set various Jacobians
   Rmatrix dynState(2, 2, 
                    -2 * (theta1 + theta3)*y1, 0.0, 
                    2 * theta1*y1, -theta2);
   Rmatrix dynStatic(2,3, 
                     -y1*y1, 0.0, - y1*y1,
                     y1*y1, -y2, 0.0);
   Rmatrix dynTime(2,1, 0.0, 0.0);

   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, STATIC, dynStatic);
   SetJacobian(DYNAMICS, TIME, dynTime);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
