//$Id$
//------------------------------------------------------------------------------
//                         EstimationRootFinder
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/09
//
/**
 * The root finder used to locate the epoch for events
 */
//------------------------------------------------------------------------------


#ifndef EstimationRootFinder_hpp
#define EstimationRootFinder_hpp


#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "PropSetup.hpp"
#include "Propagator.hpp"
#include "ODEModel.hpp"
#include "GmatState.hpp"
#include "Event.hpp"

#include "Spacecraft.hpp"
#include "FormationInterface.hpp"


/**
 * Locates roots in Event objects
 */
class ESTIMATION_API EstimationRootFinder
{
public:
   EstimationRootFinder();
   ~EstimationRootFinder();
   
   virtual void CleanUp();

   EstimationRootFinder(const EstimationRootFinder& rl);
   EstimationRootFinder& operator=(const EstimationRootFinder& rl);

   virtual void SetPropSetup(PropSetup* ps);
   virtual void FixState(Event *thisOne);
   virtual Real Locate(ObjectArray &whichOnes);

protected:
   /// The propagator used to locate the root
   PropSetup *propagator;
   /// The maximum number of location attempts before the search aborts
   Integer maxAttempts;
   /// The starting state of the root location search
   GmatState startState;

   /// Buffer of Spacecraft used to restore the initial data
   std::vector<Spacecraft *>    satBuffer;
   /// Buffer of Formations used to restore the initial data
   std::vector<FormationInterface *>     formBuffer;
   /// The current set of events that the EstimationRootFinder is using
   std::vector<Event*> *events;

   virtual Real FindRoot(Integer whichOne);
   virtual void BufferSatelliteStates(bool fillingBuffer);
};

#endif // EstimationRootFinder_hpp
