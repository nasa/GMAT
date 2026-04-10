#include "PointObj.hpp"

// EvaluateFunctions() is where the objective, constraints,
// and bounds are set
void PointObj::EvaluateFunctions() {
	// Get initial and final states								 
	Rvector X10 = GetInitialStateVector(0);
	Rvector X2f = GetFinalStateVector(1);

	// Get initial and final times								 
	Real T10 = GetInitialTime(0);
	Real T2f = GetFinalTime(1);

	// Set Bounds for initial/final times or states
	Rvector algFunctions(9);
	algFunctions(0) = T10;
	for (Integer i = 0; i < 5; ++i)
		algFunctions(i + 1) = X10(i);
	algFunctions(6) = T2f;
	algFunctions(7) = X2f(2);
	algFunctions(8) = sqrt(1.0 / X2f(0)) - X2f(3);

	Rvector lowerBound(9, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 3.32, 0.0, 0.0);
	Rvector upperBound(9, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 3.32, 0.0, 0.0);
	SetFunctions(ALGEBRAIC, algFunctions);
	SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
	SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);

	// Set cost function
	Rvector costFunction(1);
	costFunction(0) = X2f(0);			// maximize final radius
	SetFunctions(COST, costFunction);
}

// EvaluateJacobians() is where constraints and bounds are set on
// the derivatives of the dynamics (often not used)
void PointObj::EvaluateJacobians() {
}