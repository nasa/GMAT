//------------------------------------------------------------------------------
//                              OrbitTrajectory
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
* Defines the OrbitTrajectory class.
*/
//------------------------------------------------------------------------------
#include "OrbitTrajectory.hpp"

//------------------------------------------------------------------------------
// OrbitTrajectory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
OrbitTrajectory::OrbitTrajectory() :
   Trajectory()
{

}

//------------------------------------------------------------------------------
// OrbitTrajectory(const OrbitTrajectory &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
OrbitTrajectory::OrbitTrajectory(const OrbitTrajectory &copy) :
   Trajectory(copy)
{

}

//------------------------------------------------------------------------------
// OrbitTrajectory& operator=(const OrbitTrajectory &copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
OrbitTrajectory& OrbitTrajectory::operator=(const OrbitTrajectory &copy)
{
   if (&copy == this)
      return *this;

   Trajectory::operator=(copy);
   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitTrajectory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OrbitTrajectory::~OrbitTrajectory()
{

}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes path and point function objects on the trajectory with necessary
 * OrbitPhase data and then calls initialize on the base class Trajectory
 */
//------------------------------------------------------------------------------
void OrbitTrajectory::Initialize()
{
   StringArray stateReps, controlReps, thrustModes;
   RealArray ispVals, thrustVals;
   for (Integer ii = 0; ii < phaseList.size(); ++ii)
   {
      stateReps.push_back(((OrbitPhase*)phaseList.at(ii))->GetStateRep());
      controlReps.push_back(((OrbitPhase*)phaseList.at(ii))->GetControlRep());
      thrustModes.push_back(((OrbitPhase*)phaseList.at(ii))->GetThrustMode());
      ispVals.push_back(((OrbitPhase*)phaseList.at(ii))->GetIsp());
      thrustVals.push_back(
         ((OrbitPhase*)phaseList.at(ii))->GetAvailableThrust());
   }
   ((OrbitPathFunction*)pathFunction)->SetPhaseStateReps(stateReps);
   ((OrbitPathFunction*)pathFunction)->SetPhaseControlReps(controlReps);
   ((OrbitPathFunction*)pathFunction)->SetPhaseThrustModes(thrustModes);
   ((OrbitPathFunction*)pathFunction)->SetPhaseIspValues(ispVals);
   ((OrbitPathFunction*)pathFunction)->SetPhaseThrustValues(thrustVals);

   ((OrbitPointFunction*)pointFunction)->SetPhaseStateReps(stateReps);
   ((OrbitPointFunction*)pointFunction)->SetPhaseControlReps(controlReps);
   ((OrbitPointFunction*)pointFunction)->SetPhaseThrustModes(thrustModes);
   ((OrbitPointFunction*)pointFunction)->SetPhaseIspValues(ispVals);
   ((OrbitPointFunction*)pointFunction)->SetPhaseThrustValues(thrustVals);

   Trajectory::Initialize();
}
