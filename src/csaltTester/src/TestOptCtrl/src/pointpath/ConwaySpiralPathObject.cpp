//------------------------------------------------------------------------------
//                         ConwaySpiralPathObject
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
// Created: 2017.02.13
//
/**
 * Developed based on ConwaySpiralPathObject.m
 */
//------------------------------------------------------------------------------

#include "ConwaySpiralPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ConwaySpiral_PATH

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

ConwaySpiralPathObject::ConwaySpiralPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ConwaySpiralPathObject::ConwaySpiralPathObject(const ConwaySpiralPathObject &copy) :
   UserPathFunction(copy)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ConwaySpiralPathObject& ConwaySpiralPathObject::operator=(const ConwaySpiralPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ConwaySpiralPathObject::~ConwaySpiralPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void ConwaySpiralPathObject::EvaluateFunctions()
{

   
   //Extract parameter data
   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   Real r = stateVec(0);
   Real vr = stateVec(2);
   Real vt = stateVec(3);
   //TODO: Figure out how to set A=controlMag
   Real A = abs(controlVec(0));
   Real B = controlVec(0);

   Real dynFunction2 = vt/r;
   Real dynFunction3 = (vt*vt)/r - 1/(r*r) + A*sin(B);
   Real dynFunction4 = -(vr*(vt/r)) + A*cos(B);
   Rvector dynVector(4,vr, dynFunction2, dynFunction3, dynFunction4);
   SetFunctions(DYNAMICS, dynVector);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void ConwaySpiralPathObject::EvaluateJacobians()
{
   //Extract parameter data
   Rvector stateVec = GetStateVector();
   Rvector controlVec = GetControlVector();

   Real r = stateVec(0);
   Real vr = stateVec(2);
   Real vt = stateVec(3);
   //TODO: Figure out how to set A=controlMag
   Real A = abs(controlVec(0));
   Real B = controlVec(0);

   //TODO: Figure out how to do this DynStateJac

   Rvector dynControlJac(4,0, 0, A*cos(B), -A*sin(B));
   //SetDynControlJac(dynControlJac);

   Rvector dynTimeJac(4,0, 0, 0, 0);
   //SetDynTimeJac(dynTimeJac);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
