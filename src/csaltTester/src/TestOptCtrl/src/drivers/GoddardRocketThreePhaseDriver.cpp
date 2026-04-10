//------------------------------------------------------------------------------
//                           GoddardRocketThreePhaseDriver
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
// Created: Feb 27, 2017
/**
 * Driver for the 3-phase Goddard Rocket Problem
 * 
 * This is the Goddard Rocket Problem, as formulated in A Collection of
 * Optimal Control Test Problems, Chapter 28 (John T. Betts).
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "GoddardRocketThreePhaseDriver.hpp"
#include "GoddardRocketThreePhasePointObject.hpp"
#include "GoddardRocketThreePhasePathObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

GoddardRocketThreePhaseDriver::GoddardRocketThreePhaseDriver():
   CsaltTestDriver("GoddardRocketThreePhase")
{
}

GoddardRocketThreePhaseDriver::~GoddardRocketThreePhaseDriver()
{
}

void GoddardRocketThreePhaseDriver::SetPointPathAndProperties()
{
   pathObject = new GoddardRocketThreePhasePathObject();
   pointObject = new GoddardRocketThreePhasePointObject();
   maxMeshRefinementCount = 8;
}

void GoddardRocketThreePhaseDriver::SetupPhases()
{
   bool useRadau = false;

   //PHASE 1
   MessageInterface::ShowMessage("*** Creating the first phase\n");
   MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 mesh properties\n");

   //Set mesh properties
   Rvector meshIntervalFractions;
   IntegerArray meshIntervalNumPoints;
   Phase *phase1, *phase2, *phase3;
   
   std::string rkMode = "RungeKutta8";
   
   if (useRadau)
   {
      phase1 = new RadauPhase();

      meshIntervalFractions.SetSize(9);
      meshIntervalFractions[0] = -1.0;
      meshIntervalFractions[1] = -0.75;
      meshIntervalFractions[2] = -0.5;
      meshIntervalFractions[3] = -0.25;
      meshIntervalFractions[4] =  0.0;
      meshIntervalFractions[5] =  0.25;
      meshIntervalFractions[6] =  0.5;
      meshIntervalFractions[7] =  0.75;
      meshIntervalFractions[8] =  1.0;

      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
      meshIntervalNumPoints.push_back(4);
   }
   else
   {
      ImplicitRKPhase *rkphase = new ImplicitRKPhase();
      rkphase->SetTranscription(rkMode);
      phase1 = rkphase;

      meshIntervalFractions.SetSize(5);
      meshIntervalFractions[0] = 0;
      meshIntervalFractions[1] = 0.25;
      meshIntervalFractions[2] = 0.5;
      meshIntervalFractions[3] = 0.75;
      meshIntervalFractions[4] = 1.0;

      meshIntervalNumPoints.push_back(10);
      meshIntervalNumPoints.push_back(10);
      meshIntervalNumPoints.push_back(10);
      meshIntervalNumPoints.push_back(10);
   }

   std::string initialGuessMode = "LinearUnityControl";

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 time properties\n");
   //Set time properties
   //gpops
   Real timeLowerBound = 0.0;
   Real timeUpperBound = 100.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime = 10.0;

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 state properties\n");
   //Set state properties
   //gpops
   Integer numStateVars = 3;
   Rvector stateLowerBound(3, 0.0, 0.0, 1.0);
   Rvector initialGuessState(3, 0.0, 0.0, 3.0);
   Rvector finalGuessState(3, 1000.0, 800.0, 2.5);
   Rvector stateUpperBound(3, 1.0e10, 1.0e10, 3.0);

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 1 control properties\n");
   //Set control properties
   //gpops
   Integer numControlVars = 1;
   Rvector controlUpperBound(1,193.044);
   Rvector controlLowerBound(1,193.044);

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
   MessageInterface::ShowMessage("*** Creating the second phase\n");
   MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 mesh properties\n");
   //Set mesh properties
   if (useRadau)
   {
      phase2 = new RadauPhase();
   }
   else
   {
      ImplicitRKPhase *rkphase = new ImplicitRKPhase();
      rkphase->SetTranscription(rkMode);
      phase2 = rkphase;
   }

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 time properties\n");
   //Set time properties
   //gpops
   timeLowerBound = 0.0;
   timeUpperBound = 100.0;
   initialGuessTime = 10.0;
   finalGuessTime = 40.0;

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 state properties\n");
   //Set state properties
   //gpops
   numStateVars = 3;
   Rvector stateLowerBound2(3, 0.0, 0.0, 1.0);
   Rvector initialGuessState2(3, 1000.0, 800.0, 2.5);
   Rvector finalGuessState2(3, 15000.0, 800.0, 1.0);
   Rvector stateUpperBound2(3, 1.0e10, 1.0e10, 3.0);

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 2 control properties\n");
   //Set control properties
   //gpops
   Rvector controlUpperBound2(1,193.0);      // Shouldn't this be 193.044?
   Rvector controlLowerBound2(1,0.0);

   phase2->SetInitialGuessMode(initialGuessMode);

   phase2->SetNumStateVars(numStateVars);
   phase2->SetNumControlVars(numControlVars);
   phase2->SetMeshIntervalFractions(meshIntervalFractions);
   phase2->SetMeshIntervalNumPoints(meshIntervalNumPoints);
   phase2->SetStateLowerBound(stateLowerBound2);
   phase2->SetStateUpperBound(stateUpperBound2);
   phase2->SetStateInitialGuess(initialGuessState2);
   phase2->SetStateFinalGuess(finalGuessState2);
   phase2->SetTimeLowerBound(timeLowerBound);
   phase2->SetTimeUpperBound(timeUpperBound);
   phase2->SetTimeInitialGuess(initialGuessTime);
   phase2->SetTimeFinalGuess(finalGuessTime);
   phase2->SetControlLowerBound(controlLowerBound2);
   phase2->SetControlUpperBound(controlUpperBound2);

   //PHASE 3
   MessageInterface::ShowMessage("*** Creating the third phase\n");
   MessageInterface::ShowMessage("*** TEST *** Setting Phase 3 mesh properties\n");
   //Set mesh properties
   if (useRadau)
   {
      phase3 = new RadauPhase();
   }
   else
   {
      ImplicitRKPhase *rkphase = new ImplicitRKPhase();
      rkphase->SetTranscription(rkMode);
      phase3 = rkphase;
   }

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 3 time properties\n");
   //Set time properties
   //gpops
   timeLowerBound = 0.0;
   timeUpperBound = 100.0;
   initialGuessTime = 40.0;
   finalGuessTime = 45.0;

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 3 state properties\n");
   //Set state properties
   //gpops
   numStateVars = 3;
   Rvector stateLowerBound3(3, 0.0, 0.0, 1.0);
   Rvector initialGuessState3(3, 15000.0, 800.0, 1.0);
   Rvector finalGuessState3(3, 19000.0, 0.0, 1.0);
   Rvector stateUpperBound3(3, 1e10, 1e10, 3.0);

   MessageInterface::ShowMessage("*** TEST *** Setting Phase 3 control properties\n");
   //Set control properties
   //gpops
   Rvector controlUpperBound3(1,0.0);
   Rvector controlLowerBound3(1,0.0);

   phase3->SetInitialGuessMode(initialGuessMode);

   phase3->SetNumStateVars(numStateVars);
   phase3->SetNumControlVars(numControlVars);
   phase3->SetMeshIntervalFractions(meshIntervalFractions);
   phase3->SetMeshIntervalNumPoints(meshIntervalNumPoints);
   phase3->SetStateLowerBound(stateLowerBound3);
   phase3->SetStateUpperBound(stateUpperBound3);
   phase3->SetStateInitialGuess(initialGuessState3);
   phase3->SetStateFinalGuess(finalGuessState3);
   phase3->SetTimeLowerBound(timeLowerBound);
   phase3->SetTimeUpperBound(timeUpperBound);
   phase3->SetTimeInitialGuess(initialGuessTime);
   phase3->SetTimeFinalGuess(finalGuessTime);
   phase3->SetControlLowerBound(controlLowerBound3);
   phase3->SetControlUpperBound(controlUpperBound3);

   phaseList.push_back(phase1);
   phaseList.push_back(phase2);
   phaseList.push_back(phase3);
}
