//------------------------------------------------------------------------------
//                           SchwartzDriver
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 22, 2017
/**
 * Driver code for the Schwartz test problem.
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "SchwartzDriver.hpp"
#include "SchwartzPointObject.hpp"
#include "SchwartzPathObject.hpp"


//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

SchwartzDriver::SchwartzDriver():
      CsaltTestDriver("Schwartz")
{
}

SchwartzDriver::~SchwartzDriver()
{
}

void SchwartzDriver::SetPointPathAndProperties()
{
   pathObject = new SchwartzPathObject();
   pointObject = new SchwartzPointObject();

   maxMeshRefinementCount = 2;
}

void SchwartzDriver::SetupPhases()
{
   RadauPhase *phase1           = new RadauPhase();
   std::string initialGuessMode = "LinearNoControl";
   Rvector meshIntervalFractions(2,-1.0,1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(10);

   // Set time properties
   Real initialGuessTime = 0.0;
   Real finalGuessTime   = 1.0;
   Real timeLowerBound   = 0.0;
   Real timeUpperBound   = 1.0;

   // Set state properties
   Integer numStateVars  = 2;
   Rvector stateLowerBound  (2, -2.0, -2.0);
   Rvector stateUpperBound  (2,  2.0,  2.0);
   Rvector initialGuessState(2,  1.0,  1.0);
   Rvector finalGuessState  (2,  1.0,  1.0);

   // Set control properties
   Integer numControlVars  = 1;
   Rvector controlUpperBound(1,  0.5);
   Rvector controlLowerBound(1, -0.5);

   phase1->SetInitialGuessMode(initialGuessMode);
   phase1->SetNumStateVars(numStateVars);
   phase1->SetNumControlVars(numControlVars);
   phase1->SetMeshIntervalFractions(meshIntervalFractions);
   phase1->SetMeshIntervalNumPoints(meshIntervalNumPoints);
   phase1->SetStateLowerBound(stateLowerBound);
   phase1->SetStateUpperBound(stateUpperBound);
   phase1->SetStateInitialGuess(initialGuessState);
   phase1->SetStateFinalGuess(finalGuessState);
   phase1->SetTimeLowerBound(timeLowerBound);
   phase1->SetTimeUpperBound(timeUpperBound);
   phase1->SetTimeInitialGuess(initialGuessTime);
   phase1->SetTimeFinalGuess(finalGuessTime);
   phase1->SetControlLowerBound(controlLowerBound);
   phase1->SetControlUpperBound(controlUpperBound);

   //PHASE 2
   RadauPhase *phase2           = new RadauPhase();

   // Time properties
   initialGuessTime = 1.0;
   finalGuessTime   = 2.9;
   timeLowerBound   = 1.0;
   timeUpperBound   = 3.0;

   // State properties match phase 1
   // Update control properties
   numControlVars  = 1;
   controlUpperBound(0) =  5.0;
   controlLowerBound(0) = -5.0;

   phase2->SetInitialGuessMode(initialGuessMode);
   phase2->SetNumStateVars(numStateVars);
   phase2->SetNumControlVars(numControlVars);
   phase2->SetMeshIntervalFractions(meshIntervalFractions);
   phase2->SetMeshIntervalNumPoints(meshIntervalNumPoints);
   phase2->SetStateLowerBound(stateLowerBound);
   phase2->SetStateUpperBound(stateUpperBound);
   phase2->SetStateInitialGuess(initialGuessState);
   phase2->SetStateFinalGuess(finalGuessState);
   phase2->SetTimeLowerBound(timeLowerBound);
   phase2->SetTimeUpperBound(timeUpperBound);
   phase2->SetTimeInitialGuess(initialGuessTime);
   phase2->SetTimeFinalGuess(finalGuessTime);
   phase2->SetControlLowerBound(controlLowerBound);
   phase2->SetControlUpperBound(controlUpperBound);

   phaseList.push_back(phase1);
   phaseList.push_back(phase2);
}
