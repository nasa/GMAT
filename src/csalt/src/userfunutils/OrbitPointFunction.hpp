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
* Declares the OrbitPointFunction class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#ifndef OrbitPointFunction_hpp
#define OrbitPointFunction_hpp

#include "csaltdefs.hpp"
#include "UserPointFunction.hpp"

class CSALT_API OrbitPointFunction : public UserPointFunction
{
public:
   OrbitPointFunction();
   OrbitPointFunction(const OrbitPointFunction &copy);
   OrbitPointFunction& operator=(const OrbitPointFunction &copy);
   virtual ~OrbitPointFunction();

   virtual void SetPhaseStateReps(StringArray stateReps);
   virtual void SetPhaseControlReps(StringArray controlReps);
   virtual void SetPhaseThrustModes(StringArray thrustModes);
   virtual void SetPhaseIspValues(RealArray ispVals);
   virtual void SetPhaseThrustValues(RealArray thrustVals);

private:
   /// The state representation for each phase in the trajectory
   StringArray phaseStateReps;
   /// The control representation for each phase in the trajectory
   StringArray phaseControlReps;
   /// The thrust modes each phase uses
   StringArray phaseThrustModes;
   /// The ISP values of each phase
   RealArray phaseIspVals;
   /// The thrust values of each phase
   RealArray phaseThrustVals;
};
#endif // !OrbitPointFunction_hpp
