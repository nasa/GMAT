//------------------------------------------------------------------------------
//                         OrbitRaisingMultiPhasePointObject
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
// Created: 2016.07.21
//
/**
 * Developed based on OrbitRaisingMultiPhasePointObject.m
 */
//------------------------------------------------------------------------------

#include "OrbitRaisingMultiPhasePointObject.hpp"
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

OrbitRaisingMultiPhasePointObject::OrbitRaisingMultiPhasePointObject() :
UserPointFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OrbitRaisingMultiPhasePointObject::OrbitRaisingMultiPhasePointObject(const OrbitRaisingMultiPhasePointObject &copy) :
UserPointFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OrbitRaisingMultiPhasePointObject& OrbitRaisingMultiPhasePointObject::operator=(const OrbitRaisingMultiPhasePointObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPointFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OrbitRaisingMultiPhasePointObject::~OrbitRaisingMultiPhasePointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void OrbitRaisingMultiPhasePointObject::EvaluateFunctions()
{
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("Entering ORMPPO\n");
#endif
   // Define constants
   Real mu   = 1.0;
   
   // Algebraic constraints at beginning and end of phase 1
   Real    initTime      = GetInitialTime(0);
   Real    initTime2     = GetInitialTime(1);
   Rvector initStateVec  = GetInitialStateVector(0);
   Rvector initStateVec2 = GetInitialStateVector(1);
   Integer initStateSize = initStateVec.GetSize();
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("initTime  = %12.10f\n", initTime);
   MessageInterface::ShowMessage("initTime2 = %12.10f\n", initTime2);
   MessageInterface::ShowMessage("initStateVec  = %s\n", initStateVec.ToString(12).c_str());
   MessageInterface::ShowMessage("initStateVec2 = %s\n", initStateVec2.ToString(12).c_str());
   MessageInterface::ShowMessage("initStateSize = %d\n", initStateSize);
#endif
   
   Rvector phase1Functions(initStateSize + 1);
   phase1Functions(0) = initTime;
   Integer idx = 1;
   for (Integer ii = 0; ii < initStateSize; ii++)
      phase1Functions(idx++) = initStateVec(ii);
   
   Rvector phase1ConLowerBound(6, 0.0,  1.0,  0.0, 0.0, 1.0, 1.0);
   Rvector phase1ConUpperBound(6, 0.0,  1.0,  0.0, 0.0, 1.0, 1.0);
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("phase1Functions      = %s\n", phase1Functions.ToString(12).c_str());
   MessageInterface::ShowMessage("phase1ConLowerBound  = %s\n", phase1ConLowerBound.ToString(12).c_str());
   MessageInterface::ShowMessage("phase1ConUpperBound  = %s\n", phase1ConUpperBound.ToString(12).c_str());
#endif
   
   // Algebraic constraints at beginning and end of phase 2
   Real    finalTime0      = GetFinalTime(0);
   Real    finalTime       = GetFinalTime(1);
   Rvector finalStateVec0  = GetFinalStateVector(0);
   Rvector finalStateVec   = GetFinalStateVector(1);
   Integer finalStateSize  = finalStateVec.GetSize();
   Integer finalStateSize0 = finalStateVec0.GetSize();
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("finalStateVec0  = %s\n", finalStateVec0.ToString(12).c_str());
   MessageInterface::ShowMessage("finalStateVec   = %s\n", finalStateVec.ToString(12).c_str());
#endif
   Real    r               = finalStateVec(0);
   //Real    theta           = finalStateVec(1);
   //Real    dr_dt           = finalStateVec(2);
   Real    dtheta_dt       = finalStateVec(3);
   Real    orbitRateError  = GmatMathUtil::Sqrt(mu/r) - dtheta_dt;
   Rvector costF(1,-r);
   SetFunctions(COST, costF);
   Rvector phase2Functions(2+finalStateSize);
   phase2Functions(0)     = orbitRateError;
   phase2Functions(1)     = finalTime;
   idx = 2;
   for (Integer ii = 0; ii < finalStateSize; ii++)
      phase2Functions(idx++) = finalStateVec(ii);

   Rvector phase2ConLowerBound(7, 0.0, 3.32,  -1.0, -GmatMathConstants::PI, 0.0, -10.0, 0.0);
   Rvector phase2ConUpperBound(7, 0.0, 3.32,  10.0,  GmatMathConstants::PI, 0.0,  10.0, 1.0);
   
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("finalTime  = %12.10f\n", finalTime);
   MessageInterface::ShowMessage("finalStateVec0  = %s\n", finalStateVec0.ToString(12).c_str());
   MessageInterface::ShowMessage("finalStateVec   = %s\n", finalStateVec.ToString(12).c_str());
   MessageInterface::ShowMessage("finalStateSize0 = %d\n", finalStateSize0);
   MessageInterface::ShowMessage("finalStateSize  = %d\n", finalStateSize);
#endif
   // Linkage constraints
   Rvector link1(1+finalStateSize0);
   link1(0) = initTime2 - finalTime0;
   idx = 1;
   for (Integer ii = 0; ii < finalStateSize0; ii++)
      link1(idx++) = (initStateVec2(ii) - finalStateVec0(ii));
   //   link1 = [obj.GetInitialTime(2) - obj.GetFinalTime(1); ...
   //            obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1)];
   Rvector link1LowerBound(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   Rvector link1UpperBound(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("link1LowerBound  = %s\n", link1LowerBound.ToString(12).c_str());
   MessageInterface::ShowMessage("link1UpperBound  = %s\n", link1UpperBound.ToString(12).c_str());
#endif
   
   // Set the values
   Integer phase1Size = phase1Functions.GetSize();
   Integer phase2Size = phase2Functions.GetSize();
   Integer link1Size  = link1.GetSize();
   Rvector algF(phase1Size+phase2Size+link1Size);
   idx = 0;
   for (Integer ii = 0; ii < phase1Size; ii++)
      algF(idx++) = phase1Functions(ii);
   for (Integer ii = 0; ii < phase2Size; ii++)
      algF(idx++) = phase2Functions(ii);
   for (Integer ii = 0; ii < link1Size; ii++)
      algF(idx++) = link1(ii);
   SetFunctions(ALGEBRAIC, algF);
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("ALG functions are set!!\n");
#endif

   Integer phase1LowerSize = phase1ConLowerBound.GetSize();
   Integer phase2LowerSize = phase2ConLowerBound.GetSize();
   Integer link1LowerSize  = link1LowerBound.GetSize();

   Rvector algLower(phase1LowerSize+phase2LowerSize+link1LowerSize);
   idx = 0;
   for (Integer ii = 0; ii < phase1LowerSize; ii++)
      algLower(idx++) = phase1ConLowerBound(ii);
   for (Integer ii = 0; ii < phase2LowerSize; ii++)
      algLower(idx++) = phase2ConLowerBound(ii);
   for (Integer ii = 0; ii < link1LowerSize; ii++)
      algLower(idx++) = link1LowerBound(ii);
   SetFunctionBounds(ALGEBRAIC, LOWER, algLower);
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("ALG lower bounds are set!!\n");
#endif

   Integer phase1UpperSize = phase1ConUpperBound.GetSize();
   Integer phase2UpperSize = phase2ConUpperBound.GetSize();
   Integer link1UpperSize  = link1UpperBound.GetSize();
   Rvector algUpper(phase1UpperSize+phase2UpperSize+link1UpperSize);
   idx = 0;
   for (Integer ii = 0; ii < phase1UpperSize; ii++)
      algUpper(idx++) = phase1ConUpperBound(ii);
   for (Integer ii = 0; ii < phase2UpperSize; ii++)
      algUpper(idx++) = phase2ConUpperBound(ii);
   for (Integer ii = 0; ii < link1UpperSize; ii++)
      algUpper(idx++) = link1UpperBound(ii);
   SetFunctionBounds(ALGEBRAIC, UPPER, algUpper);
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("ALG upper bounds are set!!\n");
#endif

//   obj.SetFunctions(ALGEBRAIC, [phase1Functions;phase2Functions;link1])
//   obj.SetAlgFuncLowerBounds([phase1ConLowerBound;phase2ConLowerBound;link1LowerBound]);
//   obj.SetAlgFuncUpperBounds([phase1ConUpperBound;phase2ConUpperBound;link1UpperBound]);
   
#ifdef DEBUG_ORBIT_RAISING
   MessageInterface::ShowMessage("LEAVING ORMPPO\n");
#endif
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void OrbitRaisingMultiPhasePointObject::EvaluateJacobians()
{
   // currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
