//------------------------------------------------------------------------------
//                           InteriorPointDriver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 16, 2017
/**
 * Interior point driver
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "InteriorPointDriver.hpp"
#include "InteriorPointPointObject.hpp"
#include "InteriorPointPathObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace GmatMathConstants;

InteriorPointDriver::InteriorPointDriver() :
      CsaltTestDriver("InteriorPoint")
{
}

InteriorPointDriver::~InteriorPointDriver()
{
}

void InteriorPointDriver::SetPointPathAndProperties()
{
   pathObject = new InteriorPointPathObject();
   pointObject = new InteriorPointPointObject();

   maxMeshRefinementCount = 10;
}

void InteriorPointDriver::SetupPhases()
{
   std::string rkTranscription = "RungeKutta8"; //"Trapezoid";

   //PHASE 1
   //Set mesh properties
   Phase *phase1;

   ImplicitRKPhase *rkphase1 = new ImplicitRKPhase();
   rkphase1->SetTranscription(rkTranscription);
   phase1 = rkphase1;

   Rvector meshIntervalFractions(2, 0.0, 1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(10);

   std::string initialGuessMode = "LinearNoControl";

   //Set time properties
   Real timeLowerBound = 0.0;
   Real timeUpperBound = 1.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime = 0.75;

   //Set state properties
   Integer numStateVars = 1;
   Rvector stateLowerBound(1, -1.0);
   Rvector initialGuessState(1, 1.0);
   Rvector finalGuessState(1, 0.9);
   Rvector stateUpperBound(1, 1.0);

   //Set control properties
   Integer numControlVars = 1;
   Rvector controlUpperBound(1,  1.0);
   Rvector controlLowerBound(1, -1.0);

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
   Phase *phase2;

   ImplicitRKPhase *rkphase2 = new ImplicitRKPhase();
   rkphase2->SetTranscription(rkTranscription);
   phase2 = rkphase2;

   //Set time properties
   timeLowerBound = 0.0;
   timeUpperBound = 1.0;
   initialGuessTime = 0.75;
   finalGuessTime = 1.0;

   //Update state properties
   stateLowerBound(0) = -1.0;
   initialGuessState(0) = 0.9;
   finalGuessState(0) = 0.75;
   stateUpperBound(0) = 1.0;

   //Set control properties
   controlUpperBound(0) =  131.0;
   controlLowerBound(0) = -1.0;

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
