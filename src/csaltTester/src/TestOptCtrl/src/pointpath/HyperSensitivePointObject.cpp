//------------------------------------------------------------------------------
//                         HyperSensitivePointObject
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
// Created: 2016.10.18
//
/**
 * Developed based on HyperSensitivePointObject.m
 */
//------------------------------------------------------------------------------

#include "HyperSensitivePointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_HYPERSENSITIVE_POINT

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

HyperSensitivePointObject::HyperSensitivePointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
HyperSensitivePointObject::HyperSensitivePointObject(const HyperSensitivePointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
HyperSensitivePointObject& HyperSensitivePointObject::operator=(const HyperSensitivePointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
HyperSensitivePointObject::~HyperSensitivePointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void HyperSensitivePointObject::EvaluateFunctions()
{
   // Extract parameter data
   Rvector stateInit     = GetInitialStateVector(0);
   Rvector stateFinal    = GetFinalStateVector(0);
   Real    tInit         = GetInitialTime(0);
   Real    tFinal        = GetFinalTime(0);
   
   #ifdef DEBUG_HYPERSENSITIVE_POINT
      MessageInterface::ShowMessage(" --- stateInit  size = %d\n", stateInit.GetSize());
      MessageInterface::ShowMessage(" --- stateFinal size = %d\n", stateFinal.GetSize());
      MessageInterface::ShowMessage(" --- tInit           = %le\n", tInit);
      MessageInterface::ShowMessage(" --- tFinal          = %le\n", tFinal);
   #endif

   Rvector algF(2 + stateInit.GetSize() + stateFinal.GetSize());
   algF(0) = tInit;
   algF(1) = tFinal;
   Integer idx = 2;
   for (Integer ii = 0; ii < stateInit.GetSize(); ii++)
      algF(idx++) = stateInit(ii);

   for (Integer jj = 0; jj < stateFinal.GetSize(); jj++)
      algF(idx++) = stateFinal(jj);
   
#ifdef DEBUG_HYPERSENSITIVE_POINT
   MessageInterface::ShowMessage(" Setting alg functions: %s\n", algF.ToString(12).c_str());
#endif
   SetFunctions(ALGEBRAIC, algF);
   
   Rvector lower(4, 0.0, 10000.0, 1.0, 1.5);
   Rvector upper(4, 0.0, 10000.0, 1.0, 1.5);
   
   SetFunctionBounds(ALGEBRAIC, LOWER, lower);
   SetFunctionBounds(ALGEBRAIC, UPPER, upper);
#ifdef DEBUG_HYPERSENSITIVE_POINT
   MessageInterface::ShowMessage(" EXITING HyperSensitivePointObject::EvaluateFunctions\n");
#endif
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void HyperSensitivePointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
