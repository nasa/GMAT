#include "PathObj.hpp"

// EvaluateFunctions() is where the dynamics and path constraints
// are set
void PathObj::EvaluateFunctions() {
	// Get states and controls for the given segment
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	// Constants
	Real T = 0.1405;
	Real mdot = -0.0749;

	// States and controls
	Real r = X(0);
	Real th = X(1);
	Real u = X(2);
	Real v = X(3);
	Real m = X(4);
	Real ur = U(0);
	Real uth = U(1);

	// State equations
	Real rdot = u;
	Real thdot = v / r;
	Real udot = (v * v) / r - 1.0 / (r * r) + T / m * ur;
	Real vdot = -(u * v) / r + T / m * uth;

	// Set dynamics here
	Rvector dynamics(5, rdot, thdot, udot, vdot, mdot);
	SetFunctions(DYNAMICS, dynamics);

	// Add path constraints
	Rvector path(1), pathlb(1), pathub(1);
	path(0) = ur * ur + uth * uth;
	pathlb(0) = 1.0;
	pathub(0) = 1.0;
	SetFunctions(ALGEBRAIC, path);
	SetFunctionBounds(ALGEBRAIC, LOWER, pathlb);
	SetFunctionBounds(ALGEBRAIC, UPPER, pathub);
}

// EvaluateJacobians() is where analytic time, state, and control
// Jacobians are evaluated
void PathObj::EvaluateJacobians() {
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	// Constants
	Real T = 0.1405;
	Real mdot = -0.0749;

	// States and controls
	Real r = X(0);
	Real th = X(1);
	Real u = X(2);
	Real v = X(3);
	Real m = X(4);
	Real ur = U(0);
	Real uth = U(1);

	// Create state jacobian where n is the number of states. Automatically
	// is set to zero initially.
	Integer n = 5;			// number of states
	Integer nu = 2;			// number of controls
	Rmatrix jac(n, n);

	jac(0, 2) = 1.0;
	jac(1, 0) = -v / (r * r);
	jac(1, 3) = 1.0 / r;
	jac(2, 0) = -v * v / (r * r) + 2.0 * 1.0 / (r * r * r);
	jac(2, 3) = 2.0 * v / r;
	jac(2, 4) = -T / (m * m) * ur;
	jac(3, 0) = u * v / (r * r);
	jac(3, 2) = -v / r;
	jac(3, 3) = -u / r;
	jac(3, 4) = -T / (m * m) * uth;

	// Create and fill in control jacobian where m is the number of controls.
	Rmatrix ujac(n, nu);

	ujac(2, 0) = T / m;
	ujac(3, 1) = T / m;

	// Create and fill in time jacobian.
	Rmatrix tjac(n, 1);

	// Set Jacobians
	SetJacobian(DYNAMICS, STATE, jac);
	SetJacobian(DYNAMICS, CONTROL, ujac);
	SetJacobian(DYNAMICS, TIME, tjac);

	// Add derivatives of path constraint
	Rmatrix dpathx(1, n), dpathu(1, nu), dpatht(1, 1);
	dpathu(0, 0) = 2.0 * ur;
	dpathu(0, 1) = 2.0 * uth;
	SetJacobian(ALGEBRAIC, STATE, dpathx);
	SetJacobian(ALGEBRAIC, CONTROL, dpathu);
	SetJacobian(ALGEBRAIC, TIME, dpatht);
}