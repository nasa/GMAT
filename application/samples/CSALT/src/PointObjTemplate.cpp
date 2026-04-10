#include "PointObj.hpp"

// EvaluateFunctions() is where the objective, constraints,
// and bounds are set
void PointObj::EvaluateFunctions() {
	///////////////////////////////////////////////////////////////
	// If using OptimalControlFunction(s), the following may not //
	// be necessary												 //
	///////////////////////////////////////////////////////////////
	// Get initial and final states								 //
	Rvector initStateVec = GetInitialStateVector(0);			 //
	Rvector finalStateVec = GetFinalStateVector(0);				 //
	//
// Get initial and final times								 //
	Real initTime = GetInitialTime(0);							 //
	Real finalTime = GetFinalTime(0);							 //
	//
// Set Bounds for initial/final times or states				 //
	Rvector algFunctions(										 //
	Rvector lowerBound(											 //
	Rvector upperBound(											 //
	SetFunctions(ALGEBRAIC, algFunctions);						 //
	SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);			 //
	SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);			 //
	///////////////////////////////////////////////////////////////

	// Set cost function
	Rvector costFunction(1);
	costFunction(0) =
	SetFunctions(COST, costFunction);
	// Optional cost bounds. Default is -/+INF
	// costLowerBound = 
	// costUpperBound = 
}

// EvaluateJacobians() is where constraints and bounds are set on
// the derivatives of the dynamics (often not used)
void PointObj::EvaluateJacobians() {
}