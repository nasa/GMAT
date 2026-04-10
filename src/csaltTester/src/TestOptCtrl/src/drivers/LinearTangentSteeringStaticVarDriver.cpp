//------------------------------------------------------------------------------
//                           LinearTangentSteeringDriver
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
// Created: Mar 14, 2017
/**
 * Linear tangent steering test problem
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "LinearTangentSteeringStaticVarDriver.hpp"
#include "LinearTangentSteeringPointStaticVarObject.hpp"
#include "LinearTangentSteeringPathStaticVarObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

LinearTangentSteeringStaticVarDriver::LinearTangentSteeringStaticVarDriver() :
      CsaltTestDriver("LinearTangentSteeringStaticVar")
{
}

LinearTangentSteeringStaticVarDriver::~LinearTangentSteeringStaticVarDriver()
{
}

void LinearTangentSteeringStaticVarDriver::SetPointPathAndProperties()
{
   pathObject = new LinearTangentSteeringPathStaticVarObject;
   pointObject = new LinearTangentSteeringPointStaticVarObject;

   maxMeshRefinementCount = 8;
}

void LinearTangentSteeringStaticVarDriver::SetupPhases()
{
   //Set mesh properties
   Phase *phase1;
   phase1 = new RadauPhase();

   Rvector meshIntervalFractions(9, -1.0, -0.75, -0.5, -0.25, 0.0, 0.25, 0.5, 0.75, 1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);
   meshIntervalNumPoints.push_back(4);

   //Adjust this value based on your test problem.
   std::string initialGuessMode = "LinearNoControl";

   //Set time properties
   Real timeLowerBound = 0.0;
   Real timeUpperBound = 3.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime = 1.0;

   //Set state properties
   Integer numStateVars = 4;
   Rvector stateLowerBound(4, -10.0, -10.0, -10.0, -10.0);
   Rvector initialGuessState(4, 0.0, 0.0, 0.0, 0.0);
   Rvector finalGuessState(4, 12.0, 45.0, 5.0, 0.0);
   Rvector stateUpperBound(4, 100.0, 100.0, 100.0, 100.0);

   //Set control properties
   Integer numStaticVars = 2;
   Rvector staticGuess(2, 0.0, 0.0);
   Rvector staticUpperBound(2, 10.0, 10.0);
   Rvector staticLowerBound(2, 0.0, 0.0);

   phase1->SetInitialGuessMode(initialGuessMode);
   phase1->SetNumStateVars(numStateVars);
   phase1->SetNumControlVars(0); // no control 
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
   phase1->SetNumStaticVars(numStaticVars);
   phase1->SetStaticLowerBound(staticLowerBound);
   phase1->SetStaticUpperBound(staticUpperBound);
   phase1->SetStaticGuess(staticGuess);

   phaseList.push_back(phase1);
}
