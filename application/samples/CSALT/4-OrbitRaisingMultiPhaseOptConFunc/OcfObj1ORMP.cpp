#include "OCfObj1.hpp"

// Basic constructor where we set OptimalControlFunction parameters and constraint bounds
void OcfObj1::setupOCF() {
	// Declare the number of points where constraints are enforced on the phases
	numPoints = 2;

	// Declare the number of functions for the points in question
	numFunctions = 6;

	// The phaseDependencies variable indicates which phase the constraints apply for 
	// each point. Available values are up to the number of phases in the problem
	// (index zero-based).
	IntegerArray phaseDependencies = { 0, 1 };

	// The pointDependencies variable indicates where on the phase given by phaseDependencies
	// the points are located. Available values are:
	// 0 - beginning of phase
	// 1 - end of phase
	// 2 - all points in phase (path constraint)
	IntegerArray pointDependencies = { 1, 0 };

	// Resize variables for state, control, time, and parameter dependency mapping
	stateDepMap.resize(numPoints);
	controlDepMap.resize(numPoints);
	timeDepMap.resize(numPoints);
	paramDepMap.resize(numPoints);

	// Loop through boundary points and set dependencies
	for (Integer i = 0; i < numPoints; ++i) {
		timeDepMap.at(i) = true;
		stateDepMap.at(i) = true;
		controlDepMap.at(i) = false;
		paramDepMap.at(i) = false;
	}

	// Initialize the OptimalControlFunction and set desired parameters
	Initialize();
	SetNumPhases(numPoints);
	SetNumFunctions(numFunctions);
	SetPhaseDependencies(phaseDependencies);
	SetPointDependencies(pointDependencies);
	SetStateDepMap(stateDepMap);
	SetControlDepMap(controlDepMap);
	SetTimeDepMap(timeDepMap);
	SetParamDepMap(paramDepMap);

	// Set lower and upper bounds
	Rvector lBounds(numFunctions);
	Rvector uBounds(numFunctions);
	lowerBounds = lBounds;
	upperBounds = uBounds;
}

// Required function to evaluate constraints at the desired points in the phases
Rvector OcfObj1::EvaluateFunctions() {
	// Evaluate linkage constraints between phase1 and phase2
	// First index indicates the desired point
	// Second index indicates location in the array (timeData or stateData)
	Rvector algF(numFunctions);
	algF(0) = timeData[0][0] - timeData[1][0];				// Link time between phase1 and phase2
	algF(1) = stateData[0][0] - stateData[1][0];			// Link r between phase1 and phase2
	algF(2) = stateData[0][1] - stateData[1][1];			// Link th between phase1 and phase2
	algF(3) = stateData[0][2] - stateData[1][2];			// Link u between phase1 and phase2
	algF(4) = stateData[0][3] - stateData[1][3];			// Link v between phase1 and phase2
	algF(5) = stateData[0][4] - stateData[1][4];			// Link m between phase1 and phase2

	return algF;
}

// Required function to scale the constraint function bounds if desired
void OcfObj1::ScaleFunctionBounds() {
	// Check that bounds are same size and have valid values
	ValidateFunctionBounds();
}