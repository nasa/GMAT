//------------------------------------------------------------------------------
//                         HohmannTransferPointObject
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2022 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
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

#include "PointObj.hpp"
//#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//------------------------------------------------------------------------------
void PointObj::EvaluateFunctions()
{
   // Extract parameter data

   Rvector DVs = GetStaticVector(0);
   Rvector dv1(2, DVs(0), DVs(1));
   Rvector dv2(2, DVs(2), DVs(3));


   Rvector initStateVec = GetInitialStateVector(0);
   Rvector finalStateVec = GetFinalStateVector(0);

   Real initTime = GetInitialTime(0);

   Real r0 = initStateVec(0);
   Real th0 = initStateVec(1);
   Real u0 = initStateVec(2);
   Real v0 = initStateVec(3);

   Real rf = finalStateVec(0);
   Real uf = finalStateVec(2);
   Real vf = finalStateVec(3);


   Real dv1Mag = dv1.Norm(), dv2Mag = dv2.Norm();
   Rvector minDV(1, dv1Mag + dv2Mag);
   SetFunctions(COST, minDV);

   Rvector algFunctions(8, initTime, r0, th0, u0 + dv1(0), v0 + dv1(1),
       rf, uf + dv2(0), vf + dv2(1));

   Real r0value = 1.05;
   Real rfvalue = 1.5;
   Rvector lowerBound(8, 0.0, r0value, 0.0, 0.0, 1.0 / sqrt(r0value), rfvalue, 0.0, 1.0 / sqrt(rfvalue));
   Rvector upperBound(8, 0.0, r0value, 0.0, 0.0, 1.0 / sqrt(r0value), rfvalue, 0.0, 1.0 / sqrt(rfvalue));

   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
   SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);


}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void PointObj::EvaluateJacobians()
{
   // Does nothing for point objects
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
