//------------------------------------------------------------------------------
//                         OrbitRaisingPathObject
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
// Created: 2016.06.03
//
/**
 * Developed based on OrbitRaisingPathObject.m
 */
//------------------------------------------------------------------------------

#include "OrbitRaisingPathObject.hpp"

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

OrbitRaisingPathObject::OrbitRaisingPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OrbitRaisingPathObject::OrbitRaisingPathObject(const OrbitRaisingPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OrbitRaisingPathObject& OrbitRaisingPathObject::operator=(const OrbitRaisingPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OrbitRaisingPathObject::~OrbitRaisingPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void OrbitRaisingPathObject::EvaluateFunctions()
{
   Rvector stateVec   = GetStateVector();
   Rvector controlVec = GetControlVector();
   // @TODO add validation of vector size here!
   
   Real mu   = 1.0;
   Real mdot = 0.0749;
   Real T    = 0.1405;
   
   // Extract state and control components
   Real r         = stateVec(0);
   //Real theta     = stateVec(1);
   Real dr_dt     = stateVec(2);
   Real dtheta_dt = stateVec(3);
   Real m         = stateVec(4);
   Real u_r       = controlVec(0);
   Real u_theta   = controlVec(1);
   
   // Compute the dynamics
   Rvector dynFunctions(5);
   dynFunctions(0) =  dr_dt ;
   dynFunctions(1) =  dtheta_dt/r;
   dynFunctions(2) =  dtheta_dt * (dtheta_dt/r) - mu/(r*r) + T/m*u_r;
   dynFunctions(3) = -dr_dt * dtheta_dt/r +T/m * u_theta;
   dynFunctions(4) = -mdot;
   SetFunctions(DYNAMICS, dynFunctions);
   
   Rvector algFunctions(1);
   Rvector algBounds(1);
   algBounds(0) = 1.0;
   
   algFunctions(0) = (u_r * u_r) + (u_theta * u_theta);
   SetFunctions(ALGEBRAIC, algFunctions);
   SetFunctionBounds(ALGEBRAIC, UPPER, algBounds);
   SetFunctionBounds(ALGEBRAIC, LOWER, algBounds);
   
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void OrbitRaisingPathObject::EvaluateJacobians()
{
   // currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
