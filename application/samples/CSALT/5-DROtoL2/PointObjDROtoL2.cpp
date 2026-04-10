#include "PointObj.hpp"

// EvaluateFunctions() is where the objective, constraints,
// and bounds are set
void PointObj::EvaluateFunctions() {
    // Get initial and final states	
    Rvector initialState = GetInitialStateVector(0);
    Rvector finalState = GetFinalStateVector(0);

    // Get initial and final times								 
    Real initialTime = GetInitialTime(0);
    Real finalTime = GetFinalTime(0);

    // Constants
    Real mu = 0.012150585609624037;
    Real TU = 375699.81732246041;
    Real DU = 384747.96285603708;
    Real thrust_limit = 1.0;
    Real g0 = 9.80665;
    Real Isp = 3000.0;
    Real mscale = 1000.0;
    Real min_dist_moon = 0.01;
    Real min_dist_earth = 0.01;

    // Desired initial and final times
    Real initialTimeVal = 0.0;
    Real finalTimeVal = 4.50492518;

    // Desired initial state
    Rvector initialStateVals(7);
    initialStateVals(0) = 1.07730946;
    initialStateVals(1) = 0.0;
    initialStateVals(2) = 0.0;
    initialStateVals(3) = 0.0;
    initialStateVals(4) = -0.46973763;
    initialStateVals(5) = 0.0;
    initialStateVals(6) = 1.5;

    // Desired final state
    Rvector finalStateVals(6);
    finalStateVals(0) = 1.11989122;
    finalStateVals(1) = -0.11468544;
    finalStateVals(2) = -0.08789384;
    finalStateVals(3) = -0.09155242;
    finalStateVals(4) = -0.10503982;
    finalStateVals(5) = 0.1516538;

    // Compute terminal constraints
    Rvector algFunctions(15);
    for (int i = 0; i < 7; ++i)
        algFunctions(i) = initialState(i) - initialStateVals(i);
    for (int i = 0; i < 6; ++i)
        algFunctions(i + 7) = finalState(i) - finalStateVals(i);
    algFunctions(13) = initialTime - initialTimeVal;
    algFunctions(14) = finalTime - finalTimeVal;

    // Set Bounds for initial/final times or states
    Rvector lowerBound(15);
    Rvector upperBound(15);
    SetFunctions(ALGEBRAIC, algFunctions);
    SetFunctionBounds(ALGEBRAIC, LOWER, lowerBound);
    SetFunctionBounds(ALGEBRAIC, UPPER, upperBound);

    // Set cost function
    Rvector costFunction(1);
    costFunction(0) = -finalState(6);
    SetFunctions(COST, costFunction);
}

// EvaluateJacobians() is where constraints and bounds are set on
// the derivatives of the dynamics (often not used)
void PointObj::EvaluateJacobians() {
}