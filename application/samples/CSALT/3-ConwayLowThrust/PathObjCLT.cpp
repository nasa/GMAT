#include "PathObj.hpp"

// EvaluateFunctions() is where the dynamics and path constraints
// are set
void PathObj::EvaluateFunctions() {
	// Get states and controls for the given segment
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	// Constants
	Real Acc = 0.01;

	// States and controls
	Real r = X(0);
	Real th = X(1);
	Real u = X(2);
	Real v = X(3);
	Real angle = U(0);

	// State equations
	Real rdot = u;
	Real thdot = v / r;
	Real udot = (v * v) / r - 1.0 / (r * r) + Acc * sin(angle);
	Real vdot = -(u * v) / r + Acc * cos(angle);

	// Set dynamics here
	Rvector dynamics(4, rdot, thdot, udot, vdot);
	SetFunctions(DYNAMICS, dynamics);
}

void PathObj::EvaluateJacobians() {
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	Real r = X(0);
	Real th = X(1);
	Real u = X(2);
	Real v = X(3);
	Real Acc = 0.01;
	Real angle = U(0);

	Integer n = 4;		// number of states
	Integer m = 1;		// number of controls

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
	ujac(0, 0) = 0.0;
	ujac(1, 0) = 0.0;
	ujac(2, 0) = Acc * cos(angle);
	ujac(3, 0) = -Acc * sin(angle);

	// Create and fill in time jacobian.
	Rmatrix tjac(n, 1);

	// Set Jacobians
	SetJacobian(DYNAMICS, STATE, jac);
	SetJacobian(DYNAMICS, CONTROL, ujac);
	SetJacobian(DYNAMICS, TIME, tjac);
}