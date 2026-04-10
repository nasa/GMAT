//------------------------------------------------------------------------------
//                           ConwayOrbitExampleRKDriver
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
// Created: Feb 13, 2017
/**
 * Driver for the Conway orbit problem, using a Runge-Kutta phase
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "ConwayOrbitExampleRKDriver.hpp"
#include "ConwayOrbitExamplePointObject.hpp"
#include "ConwayOrbitExamplePathObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

ConwayOrbitExampleRKDriver::ConwayOrbitExampleRKDriver() :
      CsaltTestDriver("ConwayOrbitExampleRK")
{
}

ConwayOrbitExampleRKDriver::~ConwayOrbitExampleRKDriver()
{
}

void ConwayOrbitExampleRKDriver::SetPointPathAndProperties()
{
   pathObject = new ConwayOrbitExamplePathObject();
   pointObject = new ConwayOrbitExamplePointObject();

   maxMeshRefinementCount = 25; //25;
   Real costScaling = 0.1;
   costLowerBound = 0.0;
   maxMeshRefinementCount = 20;
   /*
   majorOptimalityTolerances.SetSize(3);
   majorOptimalityTolerances(0) = 0.01;
   majorOptimalityTolerances(1) = 0.01;
   majorOptimalityTolerances(2) = 1e-4;

   totalIterationsLimits.resize(2);
   totalIterationsLimits[0] = 3000;
   totalIterationsLimits[1] = 10000;

   feasibilityTolerances.SetSize(4);
   feasibilityTolerances(0) = 0.001;
   feasibilityTolerances(1) = 0.0001;
   feasibilityTolerances(2) = 0.00001;
   feasibilityTolerances(3) = 1e-8;
   */

   if(traj == NULL)
   {
      MessageInterface::ShowMessage("Trajectory undefined in ConwayOrbitExampleRKDriver::SetPointPathAndProperties\n");
   }
   traj->SetCostScaling(costScaling);
}

void ConwayOrbitExampleRKDriver::SetupPhases()
{
   ImplicitRKPhase *phase1 = new ImplicitRKPhase();
   phase1->SetTranscription("RungeKutta8");
//   phase1->SetTranscription("RungeKutta6");
//   phase1->SetTranscription("RungeKutta4");
//   phase1->SetTranscription("HermiteSimpson");
//   phase1->SetTranscription("Trapezoid");
   std::string initialGuessMode = "LinearUnityControl";
   Rvector meshIntervalFractions(2, 0.0, 1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(20);

   //Set time properties
   Real timeLowerBound = 0.0;
   Real timeUpperBound = 100.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime = 50.0;

   //Set state properties
   Integer numStateVars = 4;
   Rvector stateLowerBound(4,-10.0, -6.0 * 2.0 * GmatMathConstants::PI, -10.0, -10.0);
   Rvector initialGuessState(4,1.1, 0.0, 0.0, 1.0/sqrt(1.1));
   Rvector finalGuessState(4,5.0, 3.0 * 2.0 * GmatMathConstants::PI, 1.0, 1.0);
   Rvector stateUpperBound(4,10.0, 6.0 * 2.0 * GmatMathConstants::PI, 10.0, 10.0);

   //Set control properties
   Integer numControlVars = 1;
   Rvector controlUpperBound(1,10.0);
   Rvector controlLowerBound(1,-10.0);

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
   phase1->SetRelativeErrorTol(1e-6);
   phaseList.push_back(phase1);
}
