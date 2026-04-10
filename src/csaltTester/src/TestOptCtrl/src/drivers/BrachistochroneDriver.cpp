//------------------------------------------------------------------------------
//                           BrachistochroneDriver
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
// Created: Jan 4, 2017
/**
 * Test driver for the Brachistochrone test case
 */
//------------------------------------------------------------------------------


#include "BrachistochroneDriver.hpp"
#include "BrachistichronePointObject.hpp"
#include "BrachistichronePathObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

BrachistochroneDriver::BrachistochroneDriver() :
   CsaltTestDriver      ("Brachistochrone")
{
}

BrachistochroneDriver::BrachistochroneDriver(const std::string &testName, const Integer snoptConsoleOutputLevel) :
	CsaltTestDriver(testName, snoptConsoleOutputLevel)
{
}

BrachistochroneDriver::~BrachistochroneDriver()
{
}


void BrachistochroneDriver::SetPointPathAndProperties()
{
   pathObject = new BrachistichronePathObject();
   pointObject = new BrachistichronePointObject();

   // Also set things like the bounds, mesh refinement count, and so forth here
   maxMeshRefinementCount  = 0;
}


void BrachistochroneDriver::SetupPhases()
{
   RadauPhase *phase1           = new RadauPhase();
   //std::string initialGuessMode = "LinearNoControl";
   std::string initialGuessMode = "GuessArrays";
   Rvector meshIntervalFractions(3,-1.0, 0.0,1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(5);
   meshIntervalNumPoints.push_back(5);

   // Set time properties
   Real timeLowerBound   = 0.0;
   Real timeUpperBound   = 100.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime   = .3;
   // Set state properties
   Integer numStateVars  = 3;
   Rvector stateLowerBound(3, -10.0, -10.0, -10.0);
   Rvector stateUpperBound(3,  10.0,   0.0,   0.0);
   Rvector initialGuessState(3, 0.0,   0.0,   0.0);
   Rvector finalGuessState(3,   2.0,  -1.0,  -1.0);
   // Set control properties
   Integer numControlVars  = 1;
   Rvector controlUpperBound(1,  10.0);
   Rvector controlLowerBound(1, -10.0);

   //  This is effectively the solution, used as a guess to make sure we
   // have at lease one CSALT test that flexes guess arrays as the 
   // initial guess source
   Rvector timeArray(11,
      0,
      0.021836090339203817,
      0.065059858061501996,
      0.11298609481175435,
      0.14731810202287049,
      0.15624006535589879,
      0.1780761556951026,
      0.22129992341740082,
      0.26922616016765311,
      0.30355816737876928,
      0.31248013071179759);

   Rmatrix stateArray;
   stateArray.SetSize(11, 3);
   stateArray(0, 0) = 0;
   stateArray(0, 1) = 0;
   stateArray(0, 2) = 0;
   stateArray(1, 0) = 0.000558;
   stateArray(1, 1) = -0.0076368;
   stateArray(1, 2) = -0.70114;
   stateArray(2, 0) = 0.014536;
   stateArray(2, 1) = -0.065705;
   stateArray(2, 2) = -2.0561;
   stateArray(3, 0) = 0.072888;
   stateArray(3, 1) = -0.1842;
   stateArray(3, 2) = -3.4429;
   stateArray(4, 0) = 0.15442;
   stateArray(4, 1) = -0.2898;
   stateArray(4, 2) = -4.3183;
   stateArray(5, 0) = 0.18169;
   stateArray(5, 1) = -0.31831;
   stateArray(5, 2) = -4.5258;
   stateArray(6, 0) = 0.25921;
   stateArray(6, 1) = -0.38763;
   stateArray(6, 2) = -4.9943;
   stateArray(7, 0) = 0.4556;
   stateArray(7, 1) = -0.51198;
   stateArray(7, 2) = -5.7398;
   stateArray(8, 0) = 0.72747;
   stateArray(8, 1) = -0.607;
   stateArray(8, 2) = -6.2497;
   stateArray(9, 0) = 0.94294;
   stateArray(9, 1) = -0.63533;
   stateArray(9, 2) = -6.394;
   stateArray(10, 0) = 1;
   stateArray(10, 1) = -0.63662;
   stateArray(10, 2) = -6.4004;

   Rmatrix controlArray;
   controlArray.SetSize(11, 1);
   controlArray(0, 0) = 0;
   controlArray(1, 0) = -0.10977;
   controlArray(2, 0) = -0.32704;
   controlArray(3, 0) = -0.56797;
   controlArray(4, 0) = -0.74055;
   controlArray(5, 0) = -0.78541;
   controlArray(6, 0) = -0.89516;
   controlArray(7, 0) = -1.1125;
   controlArray(8, 0) = -1.3534;
   controlArray(9, 0) = -1.5259;
   controlArray(10, 0) = -1.5705;

   phase1->SetInitialGuessMode(initialGuessMode);
   phase1->SetInitialGuessArrays(timeArray, stateArray, controlArray);
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
