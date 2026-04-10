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

	Real r = finalStateVec(0);
	Real u = finalStateVec(2);
	Real v = finalStateVec(3);

	// Set Bounds for initial/final times or states	
	Rvector algFunctions(6, initTime, finalTime, initStateVec(0),
		initStateVec(1), initStateVec(2), initStateVec(3));
	Rvector lowerBound(6, 0.0, 50.0, 1.1, 0.0, 0.0, 1.0 / sqrt(1.1));
	Rvector upperBound(6, 0.0, 50.0, 1.1, 0.0, 0.0, 1.0 / sqrt(1.1));
	SetFunctions(ALGEBRAIC, algFunctions);
	SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
	SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);

	// Set cost function
	Rvector costFunction(1);
	costFunction(0) = (0.5 * (u * u + v * v) - 1.0 / r);
	SetFunctions(COST, costFunction);
}

// EvaluateJacobians() is where constraints and bounds are set on
// the derivatives of the dynamics (often not used)
void PointObj::EvaluateJacobians() {
}