//------------------------------------------------------------------------------
//                         GoddardRocketPathObject
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
// Author: Claire Conway
// Created: 2017.01.30
//
/**
 * Developed based on GoddardRocketPathObject.m
 */
//------------------------------------------------------------------------------

#include "GoddardRocketPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_GoddardRocket_PATH

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

GoddardRocketPathObject::GoddardRocketPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
GoddardRocketPathObject::GoddardRocketPathObject(const GoddardRocketPathObject &copy) :
   UserPathFunction(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
GoddardRocketPathObject& GoddardRocketPathObject::operator=(const GoddardRocketPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
GoddardRocketPathObject::~GoddardRocketPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void GoddardRocketPathObject::EvaluateFunctions()
{

   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   Real h = stateVec(0);
   Real v = stateVec(1);
   Real m = stateVec(2);
   Real T = controlVec(0);

   //gpops
   //Real D0 = 5.49153485*10^(-5);
   //Real B = 1.0/23800.0;
   //Real c = sqrt(3.264*32.174*23800);
   //Real g = 32.174;
   //Real dg_dh = 0.0;

   //psopt
   Real D0 = 310.0;
   Real B = 1.0/500.0;
   Real c = .5;
   Real g = 1.0/(h*h);
   //Real dg_dh = 2.0/(h*h*h);

   Real D = D0*v*v*exp(-B*h);

   Real hdot = v;
   Real vdot = 1.0/m * (T - D) - g;
   Real mdot = -T/c;

   Rvector dynFunctions(3, hdot, vdot, mdot);
   SetFunctions(DYNAMICS, dynFunctions);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void GoddardRocketPathObject::EvaluateJacobians()
{
   // Computes the user Jacobians

   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   Real h = stateVec(0);
   Real v = stateVec(1);
   Real m = stateVec(2);
   Real T = controlVec(0);

   //gpops
   //Real D0 = 5.49153485*10^(-5);
   //Real B = 1.0/23800.0;
   //Real c = sqrt(3.264*32.174*23800);
   //Real g = 32.174;
   //Real dg_dh = 0.0;

   //psopt
   Real D0 = 310.0;
   Real B = 1.0/500.0;
   Real c = .5;
   //Real g = 1.0/(h*h);
   Real dg_dh = 2.0/(h*h*h);

   //Real D = D0*v*v*exp(-B*h);

   Real dhdot_dh = 0.0;
   Real dhdot_dv = 1.0;
   Real dhdot_dm = 0.0;
   Real dvdot_dh = 1.0/m * (D0*v*v*B*exp(-B*h)) + dg_dh;
   Real dvdot_dv = 1.0/m * (-2*D0*v*exp(-B*h));
   Real dvdot_dm = -1/(m*m) * (T - D0*v*v*exp(-B*h));
   Real dmdot_dh = 0.0;
   Real dmdot_dv = 0.0;
   Real dmdot_dm = 0.0;

   Real dhdot_dT = 0.0;
   Real dvdot_dT = 1.0/m;
   Real dmdot_dT = -1.0/c;

   Rmatrix dynStateJac(3,3);
   dynStateJac(0,0) = dhdot_dh;
   dynStateJac(0,1) = dhdot_dv;
   dynStateJac(0,2) = dhdot_dm;
   dynStateJac(1,0) = dvdot_dh;
   dynStateJac(1,1) = dvdot_dv;
   dynStateJac(1,2) = dvdot_dm;
   dynStateJac(2,0) = dmdot_dh;
   dynStateJac(2,1) = dmdot_dv;
   dynStateJac(2,2) = dmdot_dm;

   Rmatrix dynControlJac(3,1);
   dynControlJac(0,0) = dhdot_dT;
   dynControlJac(1,0) = dvdot_dT;
   dynControlJac(2,0) = dmdot_dT;

   Rmatrix dynTimeJac(3,1);
   dynTimeJac(0,0) = 0.0;
   dynTimeJac(1,0) = 0.0;
   dynTimeJac(2,0) = 0.0;

   SetJacobian(DYNAMICS, STATE, dynStateJac);
   SetJacobian(DYNAMICS, CONTROL, dynControlJac);
   SetJacobian(DYNAMICS, TIME, dynTimeJac);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
