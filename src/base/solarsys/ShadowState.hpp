//$Id$
//------------------------------------------------------------------------------
//                               ShadowState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan
// Created: 2014.05.06
//
/**
 * Base class definition for the ShadowState.
 */
//------------------------------------------------------------------------------


#ifndef ShadowState_hpp
#define ShadowState_hpp

#include "SolarSystem.hpp"
#include "SpacePoint.hpp"

class GMAT_API ShadowState
{
public:
   /// class methods
   ShadowState();
   ShadowState(const ShadowState &copy);
   ShadowState& operator=(const ShadowState &copy);

   virtual ~ShadowState();

   virtual void     SetSolarSystem(SolarSystem *ss);
   virtual Real     FindShadowState(Real *state, Real *cbSun,
                                    Real *sunSat, Real sunRad,
                                    Real bodyRad);
protected:

   SolarSystem                  *solarSystem;
   CelestialBody                *sun;
   virtual Real   GetPercentSunInPenumbra(Real *state,
                              Real pcbrad,  Real psunrad, Real *force);
};

#endif   // ShadowState
