//------------------------------------------------------------------------------
//                                 OrbitPhase
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
* Defines the OrbitPhase class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#include "OrbitPhase.hpp"

//------------------------------------------------------------------------------
// OrbitPhase()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
OrbitPhase::OrbitPhase(Real dUnit, Real mUnit, Real gParam) :
   RadauPhase(),
   distanceUnit(dUnit),
   massUnit(mUnit),
   gravParam(gParam),
   thrustMode("Thrust"),
   stateRep("ModEqinoctial"),
   controlRep("CartesianFourVec"),
   availableThrustNewtons(4.0),
   Isp(2831.0),
   thrustLowerBound(0.0)
{

}

//------------------------------------------------------------------------------
// OrbitPhase(const OrbitPhase &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
OrbitPhase::OrbitPhase(const OrbitPhase &copy) :
   RadauPhase(copy),
   distanceUnit(copy.distanceUnit),
   massUnit(copy.massUnit),
   gravParam(copy.gravParam),
   thrustMode(copy.thrustMode),
   stateRep(copy.stateRep),
   controlRep(copy.controlRep),
   availableThrustNewtons(copy.availableThrustNewtons),
   Isp(copy.Isp),
   thrustLowerBound(copy.thrustLowerBound)
{

}

//------------------------------------------------------------------------------
// OrbitPhase& operator=(const OrbitPhase &copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
OrbitPhase& OrbitPhase::operator=(const OrbitPhase &copy)
{
   if (&copy == this)
      return *this;

   RadauPhase::operator=(copy);
   distanceUnit = copy.distanceUnit;
   massUnit = copy.massUnit;
   gravParam = copy.gravParam;
   thrustMode = copy.thrustMode;
   stateRep = copy.stateRep;
   controlRep = copy.controlRep;
   availableThrustNewtons = copy.availableThrustNewtons;
   Isp = copy.Isp;
   thrustLowerBound = copy.thrustLowerBound;
   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitPhase()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OrbitPhase::~OrbitPhase()
{

}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the orbit phase and sets up its scaling utility.  Also calls
 *  Initialize for Phase
 */
//------------------------------------------------------------------------------
void OrbitPhase::Initialize()
{
   // Modify scaling utility using user provided distance, mass, and 
   // gravity parameter units
   scaleUtil->SetUnit("DU", distanceUnit);
   scaleUtil->SetUnit("MU", massUnit);
   Real timeUnit = GmatMathUtil::Sqrt(pow(distanceUnit, 3.0) / gravParam);
   scaleUtil->SetUnit("TU", timeUnit);
   Real accelerationUnit = distanceUnit / (timeUnit * timeUnit);
   scaleUtil->SetUnit("ACCU", accelerationUnit);
   scaleUtil->SetUnit("VU", distanceUnit / timeUnit);
   scaleUtil->SetUnit("MFU", massUnit / timeUnit);
   scaleUtil->AddUnitAndShift("THRU", massUnit * accelerationUnit, 0.0);

   Phase::Initialize();
}

//------------------------------------------------------------------------------
// void SetThrustMode(const std::string &mode)
//------------------------------------------------------------------------------
/**
 * Sets the phase's thrust mode
 *
 * @param mode The new thrust mode, must be either "Thrust" or "Coast"
 */
//------------------------------------------------------------------------------
void OrbitPhase::SetThrustMode(const std::string &mode)
{
   if (mode == "Thrust" || mode == "Coast")
      thrustMode = mode;
   else
      throw LowThrustException("ERROR - OrbitPhase: Thrust mode " + mode +
         " is not valid.  Valid options are [Thrust Coast]\n");
}

//------------------------------------------------------------------------------
// void SetStateRep(const std::string &rep)
//------------------------------------------------------------------------------
/**
 * Sets the phase's state representation
 *
 * @param rep The new state representation, currently only "ModEquinoctial" is
 *            supported
 */
//------------------------------------------------------------------------------
void OrbitPhase::SetStateRep(const std::string &rep)
{
   if (rep == "ModEquinoctial")
      stateRep = rep;
   else
      throw LowThrustException("ERROR - OrbitPhase: Dynamics state rep. " +
         rep + " is not valid.  Valid options are [ModEquinoctial].\n");
}

//------------------------------------------------------------------------------
// void SetControlRep(const std::string &rep)
//------------------------------------------------------------------------------
/**
 * Sets the phase's control representation
 *
 * @param rep The new control representation, currently only "CartesianFourVec"
 *            is supported
 */
//------------------------------------------------------------------------------
void OrbitPhase::SetControlRep(const std::string &rep)
{
   if (rep == "CartesianFourVec")
      controlRep = rep;
   else
      throw LowThrustException("ERROR - OrbitPhase: Control state rep. " + rep +
         " is not valid.  Valid options are [CartesianFourVec].\n");
}

//------------------------------------------------------------------------------
// void SetAvailableThrust(const Real &thrust)
//------------------------------------------------------------------------------
/**
 * Sets the available thrust
 *
 * @param thrust The available thrust
 */
//------------------------------------------------------------------------------
void OrbitPhase::SetAvailableThrust(const Real &thrust)
{
   if (thrust > 0.0)
      availableThrustNewtons = thrust;
   else
      throw LowThrustException("ERROR - OrbitPhase: Thrust value of " +
         GmatStringUtil::ToString(thrust, 0) + " is invalid.  "
         "Thrust must be positive.\n");
}

//------------------------------------------------------------------------------
// void SetIsp(const Real &value)
//------------------------------------------------------------------------------
/**
 * Sets the ISP
 *
 * @param value The ISP
 */
//------------------------------------------------------------------------------
void OrbitPhase::SetIsp(const Real &value)
{
   if (value > 0.0)
      Isp = value;
   else
      throw LowThrustException("ERROR - OrbitPhase: Isp value of " +
         GmatStringUtil::ToString(value, 0) + " is invalid.  "
         "Isp must be positive.\n");
}

//------------------------------------------------------------------------------
// std::string GetThrustMode()
//------------------------------------------------------------------------------
/**
 * Returns the phase's thrust mode
 *
 * @return The thrust mode
 */
//------------------------------------------------------------------------------
std::string OrbitPhase::GetThrustMode()
{
   return thrustMode;
}

//------------------------------------------------------------------------------
// std::string GetStateRep()
//------------------------------------------------------------------------------
/**
 * Returns the phase's state representation
 *
 * @return The state representation
 */
//------------------------------------------------------------------------------
std::string OrbitPhase::GetStateRep()
{
   return stateRep;
}

//------------------------------------------------------------------------------
// std::string GetControlRep()
//------------------------------------------------------------------------------
/**
 * Returns the phase's control representation
 *
 * @return The control representation
 */
//------------------------------------------------------------------------------
std::string OrbitPhase::GetControlRep()
{
   return controlRep;
}

//------------------------------------------------------------------------------
// Real GetAvailableThrust()
//------------------------------------------------------------------------------
/**
 * Returns the available thrust
 *
 * @return The available thrust
 */
//------------------------------------------------------------------------------
Real OrbitPhase::GetAvailableThrust()
{
   return availableThrustNewtons;
}

//------------------------------------------------------------------------------
// Real GetIsp()
//------------------------------------------------------------------------------
/**
 * Returns the ISP
 *
 * @return The ISP
 */
 //------------------------------------------------------------------------------
Real OrbitPhase::GetIsp()
{
   return Isp;
}
