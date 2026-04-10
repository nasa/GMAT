//------------------------------------------------------------------------------
//                         HohmannTransferPointObject
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

#include "HohmannTransferPointObject.hpp"
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

HohmannTransferPointObject::HohmannTransferPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
HohmannTransferPointObject::HohmannTransferPointObject(const HohmannTransferPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
HohmannTransferPointObject& HohmannTransferPointObject::operator=(const HohmannTransferPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
HohmannTransferPointObject::~HohmannTransferPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//------------------------------------------------------------------------------
void HohmannTransferPointObject::EvaluateFunctions()
{
   // Extract parameter data

   Rvector p1 = GetStaticVector(0); // p1 = delta-v1  
   Rvector dv1(2, p1(0), p1(1));
   Rvector dv2(2, p1(2), p1(3));


   Rvector initStateVec = GetInitialStateVector(0);
   Rvector finalStateVec = GetFinalStateVector(0);

   Real initTime = GetInitialTime(0);
   Real finalTime = GetFinalTime(0);

   Real r = finalStateVec(0);
   Real vr = finalStateVec(2);
   Real vt = finalStateVec(3);


   Real dv1Mag = dv1.Norm(), dv2Mag = dv2.Norm();
   Rvector cost(1, dv1Mag + dv2Mag);
   SetFunctions(COST, cost);

   Rvector algFunctions(8, initTime, initStateVec(0), initStateVec(1), initStateVec(2) - dv1(0), initStateVec(3) - dv1(1),
       finalStateVec(0), finalStateVec(2) + dv2(0), finalStateVec(3) + dv2(1));

   Real r0 = 1.0;
   Real rf = 1.5;
   Rvector initState(4, r0, 0.0, 0.0, 1.0 / sqrt(r0));
   Rvector lowerBound(8, 0.0, initState(0), initState(1), initState(2), initState(3), rf, 0.0, 1.0 / sqrt(rf));
   Rvector upperBound(8, 0.0, initState(0), initState(1), initState(2), initState(3), rf, 0.0, 1.0 / sqrt(rf));

   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
   SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);


}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void HohmannTransferPointObject::EvaluateJacobians()
{
   // Does nothing for point objects
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
