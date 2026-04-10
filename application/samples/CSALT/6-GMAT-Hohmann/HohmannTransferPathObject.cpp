#include "PathObj.hpp"

// EvaluateFunctions() is where the dynamics and path constraints
// are set
void PathObj::EvaluateFunctions() {
	// Get states and controls for the given segment
	Rvector X = GetStateVector();

	// States and controls
	Real r = X(0);
	Real th = X(1);
	Real u = X(2);
	Real v = X(3);

	// State equations
	Real rdot = u;
	Real thdot = v / r;
	Real udot = (v * v) / r - 1.0 / (r * r);
	Real vdot = -(u * v) / r;

	// Set dynamics here
	Rvector dynamics(4, rdot, thdot, udot, vdot);
	SetFunctions(DYNAMICS, dynamics);
}

void PathObj::EvaluateJacobians() {
	Rvector X = GetStateVector();

	Real r = X(0);
	Real th = X(1);
	Real u = X(2);
	Real v = X(3);

	Integer n = 4;		// number of states
	Integer m = 0;		// number of controls

	// Create state jacobian where n is the number of states. Automatically
	// is set to zero initially.
	Rmatrix jac(n, n);
	jac(0, 2) = 1.0;
	jac(1, 0) = -v / (r * r);
	jac(1, 3) = 1.0 / r;
	jac(2, 0) = 2.0 / (r * r * r) - (v * v) / (r * r);
	jac(2, 3) = 2.0 * v / r;
	jac(3, 0) = (u * v) / (r * r);
	jac(3, 2) = -v / r;
	jac(3, 3) = -u / r;

	// Create and fill in control jacobian where m is the number of controls.
	Rmatrix ujac(n, m);

	// Create and fill in time jacobian.
	Rmatrix tjac(n, 1);

	// Set Jacobians
	SetJacobian(DYNAMICS, STATE, jac);
	SetJacobian(DYNAMICS, CONTROL, ujac);
	SetJacobian(DYNAMICS, TIME, tjac);
}
