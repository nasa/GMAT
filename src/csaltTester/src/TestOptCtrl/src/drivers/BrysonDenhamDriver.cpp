//------------------------------------------------------------------------------
//                            BrysonDenhamDriver
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
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Mar 13, 2017
/**
 * Driver for the Bryson-Denham test problem
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "BrysonDenhamDriver.hpp"
#include "BrysonDenhamPointObject.hpp"
#include "BrysonDenhamPathObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

BrysonDenhamDriver::BrysonDenhamDriver() :
      CsaltTestDriver("BrysonDenham")
{
}

BrysonDenhamDriver::~BrysonDenhamDriver()
{
}

void BrysonDenhamDriver::SetPointPathAndProperties()
{
   pathObject = new BrysonDenhamPathObject();
   pointObject = new BrysonDenhamPointObject();

   maxMeshRefinementCount = 15;
}

void BrysonDenhamDriver::SetupPhases()
{
   //PHASE 1
   MessageInterface::ShowMessage("*** Creating the first phase\n");
   MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 mesh properties\n");

   //Set mesh properties

   Phase *phase1 = new RadauPhase();
   std::string initialGuessMode = "LinearUnityControl";
   Rvector meshIntervalFractions(2, -1.0, 1.0);

   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(14);

   //Set time properties
   Real timeLowerBound = 0.0;
   Real timeUpperBound = 2.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime = 2.0;

   //Set state properties
   Integer numStateVars = 3;
   Rvector stateLowerBound(3, 0.0, -10.0, -10.0);
   Rvector initialGuessState(3, 0.0, 0.0, 0.0);
   Rvector finalGuessState(3, 0.0, -1.0, 4.0);
   Rvector stateUpperBound(3, 1.0/9.0, 10.0, 10.0);

   //Set control properties
   Integer numControlVars = 1;
   Rvector controlUpperBound(1,  10.0);
   Rvector controlLowerBound(1, -10.0);

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

   phaseList.push_back(phase1);
}

