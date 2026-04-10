#include "PointObj.hpp"

// EvaluateFunctions() is where the objective, constraints,
// and bounds are set
void PointObj::EvaluateFunctions() {
	// Get initial and final states		
	Rvector initStateVec = GetInitialStateVector(0);
	Rvector finalStateVec = GetFinalStateVector(0);

	// Get initial and final times
	Real initTime = GetInitialTime(0);
	Real finalTime = GetFinalTime(0);

	// Set Bounds for initial/final times or states
	Rvector algFunctions(8);
	algFunctions(0) = initTime;
	algFunctions(1) = finalTime;
	algFunctions(2) = initStateVec(0);
	algFunctions(3) = initStateVec(1);
	algFunctions(4) = initStateVec(2);
	algFunctions(5) = finalStateVec(0);
	algFunctions(6) = finalStateVec(1);
	algFunctions(7) = finalStateVec(2);

	Rvector lowerBound(8, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -10.0, -10.0);
	Rvector upperBound(8, 0.0, 100.0, 0.0, 0.0, 0.0, 1.0, 10.0, 0.0);
	SetFunctions(ALGEBRAIC, algFunctions);
	SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
	SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);

	// Set cost function
	Rvector costFunction(1);
	costFunction(0) = finalTime;
	SetFunctions(COST, costFunction);
}

// EvaluateJacobians() is where constraints and bounds are set on
// the derivatives of the dynamics (often not used)
void PointObj::EvaluateJacobians() {
}