#include "PathObj.hpp"

// EvaluateFunctions() is where the dynamics and path constraints
// are set
void PathObj::EvaluateFunctions() {
	// Get states and controls for the given segment
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	// Constants
	Real gravity = -32.174;

	// States and controls
	Real x = X(0);
	Real y = X(1);
	Real v = X(2);
	Real u = U(0);

	// State equations
	Real xdot = v * sin(u);
	Real ydot = v * cos(u);
	Real vdot = gravity * cos(u);

	// Set dynamics here
	Rvector dynamics(3, xdot, ydot, vdot);
	SetFunctions(DYNAMICS, dynamics);
}

void PathObj::EvaluateJacobians() {
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	Real gravity = -32.174;

	Real x = X(0);
	Real y = X(1);
	Real v = X(2);
	Real u = U(0);

	Integer n = 3;		// number of states
	Integer m = 1;		// number of controls

	// Create state jacobian where n is the number of states. Automatically
	// is set to zero initially.
	Rmatrix jac(n, n);

	jac(0, 2) = sin(u);
	jac(1, 2) = cos(u);

	// Create and fill in control jacobian where m is the number of controls.
	Rmatrix ujac(n, m);

	ujac(0, 0) = v * cos(u);
	ujac(1, 0) = -v * sin(u);
	ujac(2, 0) = -gravity * sin(u);

	// Create and fill in time jacobian.
	Rmatrix tjac(n, 1);

	// Set Jacobians
	SetJacobian(DYNAMICS, STATE, jac);
	SetJacobian(DYNAMICS, CONTROL, ujac);
	SetJacobian(DYNAMICS, TIME, tjac);
}