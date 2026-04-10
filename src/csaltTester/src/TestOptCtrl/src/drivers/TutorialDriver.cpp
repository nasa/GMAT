//------------------------------------------------------------------------------
//                           TutorialDriver
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
//
// Author: Noble Hatten
// Created: 2019/07/03
/**
 * Test driver for the Tutorial test case
 */
 //------------------------------------------------------------------------------


#include "TutorialDriver.hpp"
#include "TutorialPointObject.hpp"
#include "TutorialPathObject.hpp"

//#define DEBUG_SHOWRESULTS

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

TutorialDriver::TutorialDriver() :
	CsaltTestDriver("Tutorial")
{
}

TutorialDriver::~TutorialDriver()
{
}


void TutorialDriver::SetPointPathAndProperties()
{
	pathObject = new TutorialPathObject();
	pointObject = new TutorialPointObject();

	// Also set things like the bounds, mesh refinement count, and so forth here
	maxMeshRefinementCount = 0;
}


void TutorialDriver::SetupPhases()
{
	// Create a phase and set transcription configuration
	RadauPhase *phase1 = new RadauPhase();
	std::string initialGuessMode = "LinearNoControl";
	Rvector meshIntervalFractions(3, -1.0, 0.0, 1.0);
	IntegerArray meshIntervalNumPoints;
	meshIntervalNumPoints.push_back(5);
	meshIntervalNumPoints.push_back(5);

	// Set time properties
	Real timeLowerBound = 0.0;
	Real timeUpperBound = 100.0;
	Real initialGuessTime = 0.0;
	Real finalGuessTime = .3;

	// Set state properties
	Integer numStateVars = 3;
	Rvector stateLowerBound(3, -10.0, -10.0, -10.0);
	Rvector stateUpperBound(3, 10.0, 0.0, 0.0);
	Rvector initialGuessState(3, 0.0, 0.0, 0.0);
	Rvector finalGuessState(3, 2.0, -1.0, -1.0);

	// Set control properties
	Integer numControlVars = 1;
	Rvector controlUpperBound(1, 10.0);
	Rvector controlLowerBound(1, -10.0);

	// Set the phase configuration
	phase1->SetInitialGuessMode(initialGuessMode);
	//phase1->SetInitialGuessArrays(timeArray, stateArray, controlArray);
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

// method suitable for a standalone test like what is written in the User Guide Tutorial
void TutorialDriver::StandAloneTest(void)
{
	// Create a phase and set transcription configuration
	RadauPhase *phase1 = new RadauPhase();
	std::string initialGuessMode = "LinearNoControl";
	Rvector meshIntervalFractions(3, -1.0, 0.0, 1.0);
	IntegerArray meshIntervalNumPoints;
	meshIntervalNumPoints.push_back(5);
	meshIntervalNumPoints.push_back(5);

	// Set time properties
	Real timeLowerBound = 0.0;
	Real timeUpperBound = 100.0;
	Real initialGuessTime = 0.0;
	Real finalGuessTime = .3;

	// Set state properties
	Integer numStateVars = 3;
	Rvector stateLowerBound(3, -10.0, -10.0, -10.0);
	Rvector stateUpperBound(3, 10.0, 0.0, 0.0);
	Rvector initialGuessState(3, 0.0, 0.0, 0.0);
	Rvector finalGuessState(3, 2.0, -1.0, -1.0);

	// Set control properties
	Integer numControlVars = 1;
	Rvector controlUpperBound(1, 10.0);
	Rvector controlLowerBound(1, -10.0);

	// Set the phase configuration
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

	// Create a trajectory object
	// To illustrate the high level approach, default tolerances are used.
	// in CsaltTestDriver.hpp (from which TutorialDriver derives), we have protected Trajectory *traj;
	traj = new Trajectory();

	// Create the path and point function objects and add to Trajectory
	pathObject = new TutorialPathObject();
	pointObject = new TutorialPointObject();
	traj->SetUserPathFunction(pathObject);
	traj->SetUserPointFunction(pointObject);

	// Add the Phase to the Trajectory
	std::vector<Phase*> phaseList;
	phaseList.push_back(phase1);
	traj->SetPhaseList(phaseList);

	// Optimize the problem
	traj->Optimize();

	// Write the solution to file
	std::string solutionFile = "DocTestFile.och";
	traj->WriteToFile(solutionFile);
	
}
