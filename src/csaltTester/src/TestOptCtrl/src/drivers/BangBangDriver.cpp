//------------------------------------------------------------------------------
//                           BangBangDriver
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
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Mar 20, 2018
//------------------------------------------------------------------------------

#include "BangBangDriver.hpp"
#include "BangBangPathObject.hpp"
#include "BangBangPointObject.hpp"

static const Real INF = std::numeric_limits<Real>::infinity();

BangBangDriver::BangBangDriver() :
   CsaltTestDriver("BangBang")
{
}

BangBangDriver::~BangBangDriver()
{
}

void BangBangDriver::SetPointPathAndProperties()
{
   pathObject = new BangBangPathObject();
   pointObject = new BangBangPointObject();

   // Also set things like the bounds, mesh refinement count, and so forth here
   maxMeshRefinementCount = 0;
}

void BangBangDriver::SetupPhases()
{
   try
   {
      // Cost bounds
      Real costLowerBound = -INF;
      Real costUpperBound = INF;

      // PHASE 1
      // Set mesh properties
      RadauPhase *phase1 = new RadauPhase();
      std::string initialGuessMode = "LinearUnityControl";
      Rvector meshIntervalFractions(3, -1.0, 0.0, 1.0);
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(5);
      meshIntervalNumPoints.push_back(5);

      // Set time properties
      Real timeLowerBound = 0.0;
      Real timeUpperBound = 4.0;
      Real initialGuessTime = 0.0;
      Real finalGuessTime = 1.5;

      // Set state properties
      Integer numStateVars = 2;
      Rvector stateLowerBound(2, -INF, -INF);
      Rvector initialGuessState(2, 0.0, 0.0);
      Rvector finalGuessState(2, 1.0, 0.0);
      Rvector stateUpperBound(2, INF, INF);

      // Set control properties
      Integer numControlVars = 1;
      Rvector controlLowerBound(1, -1.0);
      Rvector controlUpperBound(1, 1.0);

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
   catch (LowThrustException &ex)
   {
      MessageInterface::ShowMessage("%s\n", ex.GetDetails().c_str());
   }
}

