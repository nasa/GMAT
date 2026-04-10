//------------------------------------------------------------------------------
//                               OrbitPathFunction
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2019.08.22
//
/**
* Defines the OrbitPathFunction class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#include "OrbitPathFunction.hpp"
#include "ModEqDynamics.hpp"
#include "StateConversionUtil.hpp"
#include "LowThrustException.hpp"
#include "StringUtil.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// OrbitPathFunction()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
 //------------------------------------------------------------------------------
OrbitPathFunction::OrbitPathFunction() :
   UserPathFunction()
{

}

//------------------------------------------------------------------------------
// OrbitPathFunction(const OrbitPathFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
 //------------------------------------------------------------------------------
OrbitPathFunction::OrbitPathFunction(const OrbitPathFunction &copy) :
   UserPathFunction(copy)
{

}

//------------------------------------------------------------------------------
// OrbitPathFunction& operator=(const OrbitPathFunction &copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
 //------------------------------------------------------------------------------
OrbitPathFunction& OrbitPathFunction::operator=(const OrbitPathFunction &copy)
{
   if (&copy == this)
      return *this;

   UserPathFunction::operator=(copy);

   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitPathFunction()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
OrbitPathFunction::~OrbitPathFunction()
{

}

//------------------------------------------------------------------------------
// void SetPhaseStateReps(StringArray stateReps)
//------------------------------------------------------------------------------
/**
 * Sets the state representations of each phase
 *
 * @param stateReps Array of state representations
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::SetPhaseStateReps(StringArray stateReps)
{
   phaseStateReps = stateReps;
}

//------------------------------------------------------------------------------
// void SetPhaseControlReps(StringArray controlReps)
//------------------------------------------------------------------------------
/**
 * Sets the control representations of each phase
 *
 * @param stateReps Array of control representations
 */
 //------------------------------------------------------------------------------
void OrbitPathFunction::SetPhaseControlReps(StringArray controlReps)
{
   phaseControlReps = controlReps;
}

//------------------------------------------------------------------------------
// void SetPhaseThrustModes(StringArray thrustModes)
//------------------------------------------------------------------------------
/**
 * Sets the thrust modes of each phase
 *
 * @param stateReps Array of thrust modes
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::SetPhaseThrustModes(StringArray thrustModes)
{
   phaseThrustModes = thrustModes;
}

//------------------------------------------------------------------------------
// void SetPhaseIspValues(RealArray ispVals)
//------------------------------------------------------------------------------
/**
 * Sets the ISP values of each phase
 *
 * @param stateReps Array of ISP values
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::SetPhaseIspValues(RealArray ispVals)
{
   phaseIspVals = ispVals;
}

//------------------------------------------------------------------------------
// void SetPhaseThrustValues(RealArray thrustVals)
//------------------------------------------------------------------------------
/**
 * Sets the thrust values of each phase
 *
 * @param stateReps Array of thrust values
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::SetPhaseThrustValues(RealArray thrustVals)
{
   phaseThrustVals = thrustVals;
}

//------------------------------------------------------------------------------
// void SetDynamics()
//------------------------------------------------------------------------------
/**
 * Sets the orbit dynamics in non-dimensional units
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::SetDynamics()
{
   // Set constants
   Integer phaseIdx = GetPhaseNumber();
   Real gravParam = 1.0;

   // Get the state and control
   Rvector thrustVec;
   Real mdot;
   ComputeThrust(thrustVec, mdot);
   Rvector stateVec = GetStateVector();
   if (phaseStateReps.at(phaseIdx) != "ModEquinoctial")
   {
      throw LowThrustException("ERROR - OrbitPathFunction: Unsupported phase "
         "state rep " + phaseStateReps.at(phaseIdx) +
         " for phase " + GmatStringUtil::ToString(phaseIdx, 0));
   }
   ModEqDynamics dynFunc(gravParam);
   Rvector orbitDerivs = dynFunc.ComputeOrbitDerivatives(stateVec, thrustVec);
   Rvector odeRHS(7, orbitDerivs(0), orbitDerivs(1), orbitDerivs(2),
      orbitDerivs(3), orbitDerivs(4), orbitDerivs(5), -mdot);
   SetDynFunctions(odeRHS);
}

//------------------------------------------------------------------------------
// void SetControlPathConstraint()
//------------------------------------------------------------------------------
/**
 * Set constraint on the control unit vector magnitude
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::SetControlPathConstraint()
{
   Integer phaseIdx = GetPhaseNumber();
   if (phaseControlReps.at(phaseIdx) !=
      "CartesianFourVec")
   {
      throw LowThrustException("ERROR - OrbitPathFunction: Unsupported phase "
         "control rep " + phaseControlReps.at(phaseIdx) +
         " for phase " + GmatStringUtil::ToString(phaseIdx, 0));
   }
   Rvector controlVec = GetControlVector();
   Rvector controlMagCon(1);
   controlMagCon(0) = GmatMathUtil::Sqrt(controlVec(1) * controlVec(1) +
      controlVec(2) * controlVec(2) + controlVec(3) * controlVec(3));
   Rvector funcLB(1), funcUB(1);

   if (phaseThrustModes.at(phaseIdx) == "Coast")
   {
      funcLB(0) = 0.0;
      funcUB(0) = 0.0;
   }
   else
   {
      funcLB(0) = 1.0;
      funcUB(0) = 1.0;
   }

   SetAlgFunctions(controlMagCon);
   SetAlgFunctionsUpperBounds(funcUB);
   SetAlgFunctionsLowerBounds(funcLB);
}

//------------------------------------------------------------------------------
// void ComputeThrust(Rvector &thrustVec, Real &mdot)
//------------------------------------------------------------------------------
/**
 * Computes the thrust vector in non-dimensional units
 *
 * @param thrustVec The thrust vector
 * @param mdot The mass flow rate
 */
//------------------------------------------------------------------------------
void OrbitPathFunction::ComputeThrust(Rvector &thrustVec, Real &mdot)
{
   Integer phaseIdx = GetPhaseNumber();
   thrustVec.SetSize(3);
   for (Integer ii = 0; ii < 3; ++ii)
      thrustVec(ii) = 0.0;

   // If phase is a coast, rates are zero
   if (phaseThrustModes.at(phaseIdx) == "Coast")
   {
      mdot = 0.0;
      return;
   }
   // Define constants
   Real scaleFac = 1.0e3;
   Real Isp = phaseIspVals.at(phaseIdx);
   Real availableThrustNewtons = phaseThrustVals.at(phaseIdx);
   Real availableThrustNonDim =
      phaseScaleUtilList.at(phaseIdx)->ScaleParameter(availableThrustNewtons,
         "THRU") / scaleFac;

   // Compute the thrust based on the control representation
   Rvector controlVec = GetControlVector();
   Rvector stateVec = GetStateVector();
   if (phaseControlReps.at(phaseIdx) != "CartesianFourVec")
   {
      throw LowThrustException("ERROR - OrbitPathFunction: Unsupported phase "
         "control rep " + phaseControlReps.at(phaseIdx) +
         " for phase " + GmatStringUtil::ToString(phaseIdx, 0));
   }
   for (Integer ii = 0; ii < 3; ++ii)
      thrustVec(ii) = controlVec(0) * availableThrustNonDim *
      controlVec(ii + 1) / stateVec(6);
   Real mDotDim = controlVec(0) * (availableThrustNewtons / Isp / 9.81);
   mdot = phaseScaleUtilList.at(phaseIdx)->ScaleParameter(mDotDim, "MFU");
}
