//------------------------------------------------------------------------------
//                         OrbitRaisingMultiPhasePathObject
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
// Created: 2016.09.28
//
/**
 * Developed based on OrbitRaisingMultiPhasePathObject.m
 */
//------------------------------------------------------------------------------

#include "OrbitRaisingMultiPhasePathObject.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBIT_RAISING

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

OrbitRaisingMultiPhasePathObject::OrbitRaisingMultiPhasePathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OrbitRaisingMultiPhasePathObject::OrbitRaisingMultiPhasePathObject(const OrbitRaisingMultiPhasePathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OrbitRaisingMultiPhasePathObject& OrbitRaisingMultiPhasePathObject::operator=(const OrbitRaisingMultiPhasePathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OrbitRaisingMultiPhasePathObject::~OrbitRaisingMultiPhasePathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void OrbitRaisingMultiPhasePathObject::EvaluateFunctions()
{
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("Entering ORMPPO\n");
#endif
   
   Rvector stVec = GetStateVector();
   Rvector ctVec = GetControlVector();
   // Define constants
   Real mu   = 1.0;
   Real mDot = 0.0749;
   Real T    = 0.1405;
   
   // Extract state and control components
   Real r         = stVec(0);
   //Real theta     = stVec(1);
   Real dr_dt     = stVec(2);
   Real dtheta_dt = stVec(3);
   Real m         = stVec(4);
   Real u_r       = ctVec(0);
   Real u_theta   = ctVec(1);
   
   // Compute the dynamics
   Rvector dynF(5);
   dynF[0] = dr_dt ;
   dynF[1] = dtheta_dt/r;
   dynF[2] = dtheta_dt*dtheta_dt/r - mu/(r*r) + T/m*u_r;
   dynF[3] = -dr_dt*dtheta_dt/r +T/m*u_theta;
   dynF[4] = -mDot;
//   Rvector dynF(5, dr_dt, dtheta_dt/r, dtheta_dt*dtheta_dt/r - mu/(r*r) + T/m*u_r,
//                -dr_dt*dtheta_dt/r +T/m*u_theta, -mDot);
   
   SetFunctions(DYNAMICS, dynF);
   
   Rvector f(1);
   f(0) = u_r*u_r + u_theta*u_theta;
   SetFunctions(ALGEBRAIC, f);
   f(0) = 1.0;
   SetFunctionBounds(ALGEBRAIC, UPPER, f);
   SetFunctionBounds(ALGEBRAIC, LOWER, f);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void OrbitRaisingMultiPhasePathObject::EvaluateJacobians()
{
   Rvector stVec = GetStateVector();
   Rvector ctVec = GetControlVector();

   // Define constants
   Real mu   = 1.0;
   //Real mDot = 0.0749;
   Real T    = 0.1405;

   // Extract state and control components
   Real r         = stVec(0);
   //Real theta     = stVec(1);
   Real dr_dt     = stVec(2);
   Real dtheta_dt = stVec(3);
   Real m         = stVec(4);
   Real u_r       = ctVec(0);
   Real u_theta   = ctVec(1);
   
   Rmatrix dynStateJac(5,5,
           0.0,                                         0.0,   1.0,            0.0,             0.0,
           -dtheta_dt/(r*r),                            0.0,   0.0,            1.0/r,           0.0,
           (-dtheta_dt*dtheta_dt/(r*r) + 2*mu/(r*r*r)), 0.0,   0.0,            2*dtheta_dt/r,  -T/(m*m)*u_r,
           dr_dt*dtheta_dt/(r*r),                       0.0,  -dtheta_dt/r,   -dr_dt/r,        -T/(m*m)*u_theta,
           0.0,                                         0.0,   0.0,            0.0,             0.0);
           
   SetJacobian(DYNAMICS, STATE, dynStateJac);
   
   Rmatrix dynTimeJac(5, 1, 0.0, 0.0, 0.0, 0.0, 0.0);
   SetJacobian(DYNAMICS, TIME, dynTimeJac);
   
   Rmatrix dynControlJac(5,2,
                         0.0, 0.0,
                         0.0, 0.0,
                         T/m, 0.0,
                         0.0, T/m,
                         0.0, 0.0);

   SetJacobian(DYNAMICS, CONTROL, dynControlJac);
   
   Rmatrix algStateJac(1, 5, 0.0,0.0,0.0,0.0,0.0);
   Rmatrix algTimeJac(1,1, 0.0);
   
   SetJacobian(ALGEBRAIC, STATE, algStateJac);
   SetJacobian(ALGEBRAIC, TIME, algTimeJac);
   
   Rmatrix algControlJac(1,2, 2*u_r, 2*u_theta);
   SetJacobian(ALGEBRAIC, CONTROL, algControlJac);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
