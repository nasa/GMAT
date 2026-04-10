//------------------------------------------------------------------------------
//                         GoddardRocketThreePhasePathObject
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
 * Path function for the GoddardRocketThreePhase test problem
 */
//------------------------------------------------------------------------------

#include "GoddardRocketThreePhasePathObject.hpp"
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

GoddardRocketThreePhasePathObject::GoddardRocketThreePhasePathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
GoddardRocketThreePhasePathObject::~GoddardRocketThreePhasePathObject()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
GoddardRocketThreePhasePathObject::GoddardRocketThreePhasePathObject(const GoddardRocketThreePhasePathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// Assignment operator
//------------------------------------------------------------------------------
GoddardRocketThreePhasePathObject& GoddardRocketThreePhasePathObject::operator=(const GoddardRocketThreePhasePathObject &copy)
{
   if (&copy != this)
   {
      UserPathFunction::operator=(copy);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void GoddardRocketThreePhasePathObject::EvaluateFunctions()
{
   // Values matching the MATLAB gpops settings
   Real D0 = 5.49153484923381010e-5;
   Real B = 1.0/23800.0;
   Real c = sqrt(3.264 * 32.174 * 23800.0);  // 1580.9425279876559;
   Real g = 32.174;
//   Real dg_dh = 0.0;             // Uniform gravity case

   SetStatePerturbation(1e-2);
   SetTimePerturbation(1e-2);
   SetControlPerturbation(1e-2);
   // Extract parameter data
   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   Real h = stateVec(0);         // height
   Real v = stateVec(1);         // speed
   Real m = stateVec(2);         // mass
   Real T = controlVec(0);       // thrust

   Real D = D0 * v * v * exp(-B * h);
   Real hdot = v;
   Real vdot = (T - D) / m - g;
   Real mdot = -T/c;

   Rvector dynFunctions(3, hdot, vdot, mdot);
   SetFunctions(DYNAMICS, dynFunctions);

   Integer phaseNum = GetPhaseNumber();

   // See Betts 2010 for the correct phase 2 constraint
   if (phaseNum == 1)
   {
      Real voverc = v/c;
      Real xmg = m * g;
      Real term1 = (c*c) * (1.0 + voverc) / (g / B) - 1.0 - (2.0 / voverc);
      Real term2 = xmg / (1.0 + 4.0 / voverc + 2.0 / (voverc*voverc));
      Real path_constraint = T - D - xmg - term1*term2;

      Rvector algFunctions(1, path_constraint);
      Rvector algFuncUpper(1, 0.0);
      Rvector algFuncLower(1, 0.0);

      SetFunctions(ALGEBRAIC, algFunctions);
      SetFunctionBounds(ALGEBRAIC, UPPER, algFuncUpper);
      SetFunctionBounds(ALGEBRAIC, LOWER, algFuncLower);
   }
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void GoddardRocketThreePhasePathObject::EvaluateJacobians()
{
   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   // Values matching the MATLAB gpops settings
   Real D0 = 5.49153484923381010e-5;
   Real B = 1.0/23800.0;
   Real c = sqrt(3.264 * 32.174 * 23800.0);  // 1580.9425279876559;
//   Real g = 32.174;
   Real dg_dh = 0.0;

   Real h = stateVec(0);
   Real v = stateVec(1);
   Real m = stateVec(2);
   Real T = controlVec(0);

//   Real D = D0 * v * v * exp(-B * h);

   Real dhdot_dh = 0.0;
   Real dhdot_dv = 1.0;
   Real dhdot_dm = 0.0;
   Real dvdot_dh = 1.0 / m * (D0 * v*v * B * exp(-B * h)) + dg_dh;
   Real dvdot_dv = 1.0 / m * (-2.0 * D0 * v * exp(-B * h));
   Real dvdot_dm = -1.0 / (m*m) * (T - D0 * v*v * exp(-B * h));
   Real dmdot_dh = 0.0;
   Real dmdot_dv = 0.0;
   Real dmdot_dm = 0.0;

   Real dhdot_dT = 0.0;
   Real dvdot_dT = 1.0 / m;
   Real dmdot_dT = -1.0 / c;

   Rmatrix dynState(3,3);
   Rmatrix dynControl(3,1);
   Rmatrix dynTime(3,1);

   // Row 1
   dynState(0,0) = dhdot_dh;
   dynState(0,1) = dhdot_dv;
   dynState(0,2) = dhdot_dm;
   // Row 2
   dynState(1,0) = dvdot_dh;
   dynState(1,1) = dvdot_dv;
   dynState(1,2) = dvdot_dm;
   // Row 3
   dynState(2,0) = dmdot_dh;
   dynState(2,1) = dmdot_dv;
   dynState(2,2) = dmdot_dm;

   dynControl(0,0) = dhdot_dT;
   dynControl(1,0) = dvdot_dT;
   dynControl(2,0) = dmdot_dT;

   dynTime(0,0) = 0.0;
   dynTime(1,0) = 0.0;
   dynTime(2,0) = 0.0;

   SetJacobian(DYNAMICS, STATE, dynState);
   SetJacobian(DYNAMICS, CONTROL, dynControl);
   SetJacobian(DYNAMICS, TIME, dynTime);
}
