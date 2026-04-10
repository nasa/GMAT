//------------------------------------------------------------------------------
//                         GoddardRocketThreePhasePointObject
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
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Feb 27, 2017
/**
 * Point functions for the GoddardRocketThreePhase test case
 */
//------------------------------------------------------------------------------

#include "MessageInterface.hpp"
#include "GoddardRocketThreePhasePointObject.hpp"

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
GoddardRocketThreePhasePointObject::GoddardRocketThreePhasePointObject() :
   UserPointFunction()
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
GoddardRocketThreePhasePointObject::~GoddardRocketThreePhasePointObject()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
GoddardRocketThreePhasePointObject::GoddardRocketThreePhasePointObject(const GoddardRocketThreePhasePointObject &copy) :
   UserPointFunction(copy)
{
}

//------------------------------------------------------------------------------
// Assignment operator
//------------------------------------------------------------------------------
GoddardRocketThreePhasePointObject& GoddardRocketThreePhasePointObject::operator=(const GoddardRocketThreePhasePointObject &copy)
{
   if (&copy != this)
   {
      UserPointFunction::operator=(copy);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void GoddardRocketThreePhasePointObject::EvaluateFunctions()
{
   // Values matching the MATLAB gpops settings
   Real D0 = 5.49153484923381010e-5;
   Real B = 1.0/23800.0;
   Real c = sqrt(3.264 * 32.174 * 23800.0); //1580.9425279876559;
//   Real g = 32.174;
//   Real dg_dh = 0.0;

   SetStatePerturbation(1e-2);
   SetTimePerturbation(1e-2);
   SetControlPerturbation(1e-2);

   Rvector stateVec1i = GetInitialStateVector(0);
   Rvector stateVec1f = GetFinalStateVector(0);
   Rvector stateVec2i = GetInitialStateVector(1);
   Rvector stateVec2f = GetFinalStateVector(1);
   Rvector stateVec3i = GetInitialStateVector(2);
   Rvector stateVec3f = GetFinalStateVector(2);

   Rvector costFunc(1, -stateVec3f(0));      // Maximize height -> minimize -h
   SetFunctions(COST, costFunc);

   Real timePhase1i = GetInitialTime(0);
   Real timePhase1f = GetFinalTime(0);
   Real timePhase2i = GetInitialTime(1);
   Real timePhase2f = GetFinalTime(1);
   Real timePhase3i = GetInitialTime(2);
   Real timePhase3f = GetFinalTime(2);

   Real t_interface_1_2_error = timePhase1f - timePhase2i;
   Real h_interface_1_2_error = stateVec1f(0) - stateVec2i(0);
   Real v_interface_1_2_error = stateVec1f(1) - stateVec2i(1);
   Real m_interface_1_2_error = stateVec1f(2) - stateVec2i(2);

   Real t_interface_2_3_error = timePhase2f - timePhase3i;
   Real h_interface_2_3_error = stateVec2f(0) - stateVec3i(0);
   Real v_interface_2_3_error = stateVec2f(1) - stateVec3i(1);
   Real m_interface_2_3_error = stateVec2f(2) - stateVec3i(2);

   Real h2f = stateVec2f(0);
   Real v2f = stateVec2f(1);
   Real m2f = stateVec2f(2);

   Real g2f = 32.174;
//   Real dg2f_dh2f = 0.0;

   Real D2f = D0 * v2f * v2f * exp(-B * h2f);
   Real point_constraint = m2f * g2f - (1.0 + v2f / c) * D2f;

   Rvector algFunctions(17,
         timePhase1i,
         t_interface_1_2_error,
         t_interface_2_3_error,
         timePhase3f,
         stateVec1i(0),
         stateVec1i(1),
         stateVec1i(2),
         h_interface_1_2_error,
         v_interface_1_2_error,
         m_interface_1_2_error,
         h_interface_2_3_error,
         v_interface_2_3_error,
         m_interface_2_3_error,
         stateVec3f(0),
         stateVec3f(1),
         stateVec3f(2),
         point_constraint);

   // Values matching the MATLAB gpops settings
   Rvector algFuncLower(17, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 
      0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
   Rvector algFuncUpper(17, 0.0, 0.0, 0.0, 1.0e10, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 1.0e10, 0.0, 1.0, 0.0);

   //psopt
   //Rvector algFuncLower(17, 0.0, 0.0, 0.0, 0.1, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 
   //   0.0, 0.0, 0.0, 1.0, 0.0, 0.6, 0.0);
   //Rvector algFuncUpper(17, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0,
   //   0.0, 0.0, 0.0, 1e5, 0.0, 0.6, 0.0);

   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
   SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void GoddardRocketThreePhasePointObject::EvaluateJacobians()
{
   // Does nothing
}
