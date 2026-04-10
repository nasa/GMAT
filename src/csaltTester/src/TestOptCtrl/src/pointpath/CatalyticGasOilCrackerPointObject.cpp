//------------------------------------------------------------------------------
//                         CatalyticGasOilCrackerPointObject
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

#include "CatalyticGasOilCrackerPointObject.hpp"
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

CatalyticGasOilCrackerPointObject::CatalyticGasOilCrackerPointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CatalyticGasOilCrackerPointObject::CatalyticGasOilCrackerPointObject(const CatalyticGasOilCrackerPointObject &copy) :
   UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CatalyticGasOilCrackerPointObject& CatalyticGasOilCrackerPointObject::operator=(const CatalyticGasOilCrackerPointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CatalyticGasOilCrackerPointObject::~CatalyticGasOilCrackerPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//------------------------------------------------------------------------------
void CatalyticGasOilCrackerPointObject::EvaluateFunctions()
{

   // Measured values of y1 
   Rvector y1meas(21, 1.0, 0.8105, 0.6208, 0.5258, 0.4345, 0.3903, 0.3342, 
                  0.3034, 0.2735, 0.2405, 0.2283, 0.2071, 0.1669, 0.153, 0.1339, 
                  0.1265, 0.12, 0.099, 0.087, 0.077, 0.069);
   // Measured values of y2 
   Rvector y2meas(21, 0.0, 0.2, 0.2886, 0.301, 0.3215, 0.3123, 0.2716, 0.2551, 
                  0.2258, 0.1959, 0.1789, 0.1457, 0.1198, 0.0909, 0.0719, 0.0561, 
                  0.046, 0.028, 0.019, 0.014, 0.01);

   Rvector tmeas(21, 0.0, 0.025, 0.05, 0.075, 0.1, 0.125, 0.15, 0.175, 0.2, 0.225, 0.25,
      0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.65, 0.75, 0.85, 0.95);

   Integer numPhases = 20;
   Real costFunc = 0.0;
   for (Integer idx = 0; idx < numPhases; idx++)
   {
      Rvector stateVeci = GetInitialStateVector(idx);
      costFunc += 1.0 / 2.0*pow(stateVeci(0) - y1meas(idx), 2);
      costFunc += 1.0 / 2.0*pow(stateVeci(1) - y2meas(idx), 2);
   }
   // handle the last point
   Rvector stateVecf = GetFinalStateVector(numPhases-1);
   costFunc += 1.0 / 2.0*pow(stateVecf(0) - y1meas(20), 2);
   costFunc += 1.0 / 2.0*pow(stateVecf(1) - y2meas(20), 2);

   Integer numTimeConstr = 40;
   Integer numStateConstr = 40;
   Integer numStaticConstr = 60;
   Integer numAlgConstr = numTimeConstr + numStateConstr + numStaticConstr;
   Rvector algFunctions(numAlgConstr);

   // take case of the end points
   algFunctions(0) = GetInitialTime(0) - tmeas(0);
   algFunctions(1) = GetFinalTime(numPhases - 1) - tmeas(20);
   
   for (Integer idx = 0; idx < numPhases - 1; idx++)
   {
      // the continuity and measurement constraints of time variable
      Real time = tmeas(idx + 1);
      Real timef = GetFinalTime(idx);
      Real timei = GetInitialTime(idx + 1);
      algFunctions(2 + 2 * idx) = timef - time;
      algFunctions(3 + 2 * idx) = timei - time;

      // the continuity of state variable
      Rvector stateVecf = GetFinalStateVector(idx);
      Rvector stateVeci = GetInitialStateVector(idx + 1);      
      algFunctions(numTimeConstr + 2 * idx) = stateVecf(0) - stateVeci(0);
      algFunctions(numTimeConstr + 1 + 2 * idx) = stateVecf(1) - stateVeci(1);

      // the continuity of static variable
      Rvector staticVecf = GetStaticVector(idx);
      Rvector staticVeci = GetStaticVector(idx + 1);      
      algFunctions(numTimeConstr + numStateConstr + 3 * idx) = staticVecf(0) - staticVeci(0);
      algFunctions(numTimeConstr + numStateConstr + 1 + 3 * idx) = staticVecf(1) - staticVeci(1);
      algFunctions(numTimeConstr + numStateConstr + 2 + 3 * idx) = staticVecf(2) - staticVeci(2);
   }

   
   Rvector costFunction(1, costFunc);
   SetFunctions(COST, costFunction);


   // Set algebraic functions
   Rvector algFuncLower(numAlgConstr), algFuncUpper(numAlgConstr);
   algFuncLower.MakeZeroVector();
   algFuncUpper.MakeZeroVector();

   SetAlgFunctions(algFunctions);
   SetAlgLowerBounds(algFuncLower);
   SetAlgUpperBounds(algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void CatalyticGasOilCrackerPointObject::EvaluateJacobians()
{
   // Does nothing for point objects
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
