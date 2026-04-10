//------------------------------------------------------------------------------
//                             OrbitPointFunction
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
* Defines the OrbitPointFunction class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#include "OrbitPointFunction.hpp"

//------------------------------------------------------------------------------
// OrbitPointFunction()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
OrbitPointFunction::OrbitPointFunction() :
   UserPointFunction()
{

}

//------------------------------------------------------------------------------
// OrbitPointFunction(const OrbitPointFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
OrbitPointFunction::OrbitPointFunction(const OrbitPointFunction &copy) :
   UserPointFunction(copy)
{

}

//------------------------------------------------------------------------------
// OrbitPointFunction& operator=(const OrbitPointFunction &copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
OrbitPointFunction& OrbitPointFunction::operator=(const OrbitPointFunction &copy)
{
   if (&copy == this)
      return *this;

   UserPointFunction::operator=(copy);

   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitPointFunction()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OrbitPointFunction::~OrbitPointFunction()
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
void OrbitPointFunction::SetPhaseStateReps(StringArray stateReps)
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
void OrbitPointFunction::SetPhaseControlReps(StringArray controlReps)
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
void OrbitPointFunction::SetPhaseThrustModes(StringArray thrustModes)
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
void OrbitPointFunction::SetPhaseIspValues(RealArray ispVals)
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
void OrbitPointFunction::SetPhaseThrustValues(RealArray thrustVals)
{
   phaseThrustVals = thrustVals;
}
