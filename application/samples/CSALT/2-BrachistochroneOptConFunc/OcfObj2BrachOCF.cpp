#include "OcfObj2.hpp"

// Basic constructor where we set OptimalControlFunction parameters and constraint bounds
void OcfObj2::setupOCF() {
	// Declare the number of points where constraints are enforced on the phases
	numPoints = 2;

	// Declare the number of functions for the points in question
	numFunctions = 6;

	// The phaseDependencies variable indicates which phase the constraints apply for 
	// each point. Available values are up to the number of phases in the problem
	// (index zero-based).
	IntegerArray phaseDependencies = { 0, 0 };

	// The pointDependencies variable indicates where on the phase given by phaseDependencies
	// the points are located. Available values are:
	// 0 - beginning of phase
	// 1 - end of phase
	// 2 - all points in phase (path constraint)
	IntegerArray pointDependencies = { 0, 1 };

	// Resize variables for state, control, time, and parameter dependency mapping
	stateDepMap.resize(numPoints);
	controlDepMap.resize(numPoints);
	timeDepMap.resize(numPoints);
	paramDepMap.resize(numPoints);

	// Loop through boundary points and set dependencies
	for (Integer i = 0; i < numPoints; ++i)
	{
		timeDepMap.at(i) = false;
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
	Rvector lowerBound(numFunctions, 0.0, 0.0, 0.0, 1.0, -10.0, -10.0);
	Rvector upperBound(numFunctions, 0.0, 0.0, 0.0, 1.0, 10.0, 0.0);
	lowerBounds = lowerBound;
	upperBounds = upperBound;
}

// Required function to evaluate constraints at the desired points in the phases
Rvector OcfObj2::EvaluateFunctions() {

	// Set the constraints. First index indicates the desired point
	// Second index indicates which state in the array
	Rvector algF(numFunctions);
	algF(0) = stateData[0][0];
	algF(1) = stateData[0][1];
	algF(2) = stateData[0][2];
	algF(3) = stateData[1][0];
	algF(4) = stateData[1][1];
	algF(5) = stateData[1][2];

	return algF;
}

// Required function to scale the constraint function bounds if desired
void OcfObj2::ScaleFunctionBounds() {
	// Check that bounds are same size and have valid values
	ValidateFunctionBounds();
}