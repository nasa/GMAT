//------------------------------------------------------------------------------
//                         RauAutomaticaPointObject
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
 * Developed based on RauAutomaticaPointObject.m
 */
//------------------------------------------------------------------------------

#include "RauAutomaticaPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RauAutomatica_POINT

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

RauAutomaticaPointObject::RauAutomaticaPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
RauAutomaticaPointObject::RauAutomaticaPointObject(const RauAutomaticaPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
RauAutomaticaPointObject& RauAutomaticaPointObject::operator=(const RauAutomaticaPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RauAutomaticaPointObject::~RauAutomaticaPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void RauAutomaticaPointObject::EvaluateFunctions()
{

      // Extract parameter data
      Rvector stateInit = GetInitialStateVector(0);
      Rvector stateFinal = GetFinalStateVector(0);
      Real    tInit = GetInitialTime(0);
      Real    tFinal = GetFinalTime(0);

      Rvector costVec(stateFinal.GetSize());
      for (int idx = 0; idx < stateFinal.GetSize(); ++idx)
      {
         costVec(idx) = -stateFinal(idx);
      }

      SetFunctions(COST, costVec);

      Rvector algF(3);
      algF(0) = tInit;
      algF(1) = tFinal;
      algF(2) = stateInit(0);

      SetFunctions(ALGEBRAIC, algF);

      Rvector lower(3, 0.0, 2.0, 1.0);
      Rvector upper(3, 0.0, 2.0, 1.0);

      SetFunctionBounds(ALGEBRAIC, LOWER, lower);
      SetFunctionBounds(ALGEBRAIC, UPPER, upper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void RauAutomaticaPointObject::EvaluateJacobians()
{
   // Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
