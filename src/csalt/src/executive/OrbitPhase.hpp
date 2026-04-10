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
* Declares the OrbitPhase class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#ifndef OrbitPhase_hpp
#define OrbitPhase_hpp

#include "csaltdefs.hpp"
#include "RadauPhase.hpp"

class CSALT_API OrbitPhase : public RadauPhase
{
public:
   OrbitPhase(Real dUnit, Real mUnit, Real gParam);
   OrbitPhase(const OrbitPhase &copy);
   OrbitPhase& operator=(const OrbitPhase &copy);
   virtual ~OrbitPhase();

   virtual void Initialize();
   virtual void SetThrustMode(const std::string &mode);
   virtual void SetStateRep(const std::string &rep);
   virtual void SetControlRep(const std::string &rep);
   virtual void SetAvailableThrust(const Real &thrust);
   virtual void SetIsp(const Real &value);
   virtual std::string GetThrustMode();
   virtual std::string GetStateRep();
   virtual std::string GetControlRep();
   virtual Real GetAvailableThrust();
   virtual Real GetIsp();

protected:
   /// Input parameter that represents the distance unit for scaling
   Real distanceUnit;
   /// Input parameter that represents the mass unit for scaling
   Real massUnit;
   /// Input parameter that represents the gravitational parameter of the
   /// central body
   Real gravParam;
   /// The mode for thrust model
   std::string thrustMode;
   /// Dyanmics state representation
   std::string stateRep;
   /// Control representation
   std::string controlRep;
   /// Maximum allowed thrust in Newtons
   Real availableThrustNewtons;
   /// Isp of the engine, in seconds
   Real Isp;
   /// Thrust lower bound
   Real thrustLowerBound;
};
#endif
