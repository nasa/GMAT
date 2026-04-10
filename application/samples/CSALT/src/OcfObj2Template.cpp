#include "OcfObj2.hpp"

// Basic constructor where we set OptimalControlFunction parameters and constraint bounds
void OcfObj2::setupOCF() {
	// Declare the number of points where constraints are enforced on the phases
	numPoints =

	// Declare the number of functions for the points in question
	numFunctions =

	// The phaseDependencies variable indicates which phase the constraints apply for 
	// each point. Available values are up to the number of phases in the problem
	// (index zero-based).
	IntegerArray phaseDependencies =

	// The pointDependencies variable indicates where on the phase given by phaseDependencies
	// the points are located. Available values are:
	// 0 - beginning of phase
	// 1 - end of phase
	// 2 - all points in phase (path constraint)
	IntegerArray pointDependencies =

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
	Rvector lBounds(numFunctions,
	Rvector uBounds(numFunctions,
	lowerBounds = lBounds;
	upperBounds = uBounds;
}

// Required function to evaluate constraints at the desired points in the phases
Rvector OcfObj1::EvaluateFunctions() {

	// Set the constraints
	Rvector algF(numFunctions);
	//e.g. initial and final time constraints on phase1 (0 index)
	//algF(0) = timeData[0][0];
	//algF(1) = timeData[1][0];

	return algF;
}

// Required function to scale the constraint function bounds if desired
void OcfObj2::ScaleFunctionBounds() {
	// Check that bounds are same size and have valid values
	ValidateFunctionBounds();
}