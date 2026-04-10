#include "PathObj.hpp"

// EvaluateFunctions() is where the dynamics and path constraints
// are set
void PathObj::EvaluateFunctions() {
	// Get states and controls for the given segment
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	// Extract specific variables if needed, e.g.:
	// Real r = X(0);
	// Real th = X(1);
	// Real u = X(2);
	// Real v = X(3);
	// Real Acc = 0.01;
	// Real angle = U(0);

	// Put State equations here, e.g.:
	// Real rdot = u;
	// Real thdot = v/r;
	// Real udot = (v*v)/r-1.0/(r*r)+Acc*sin(angle);
	// Real vdot = -(u*v)/r+Acc*cos(angle);

	// Set dynamics here, e.g.:
	// Rvector dynamics(4,rdot,thdot,udot,vdot);
	Rvector dynamics(
	SetFunctions(DYNAMICS, dynamics);

	// Add path constraints if necessary, e.g.:
	// Rvector path(1), pathlb(1), pathub(1);
	// path(0) = X(0)*U(0);
	// pathlb(0) = 0.0;
	// pathub(0) = 100.0;
	// SetFunctions(ALGEBRAIC, path);
	// SetFunctionBounds(ALGEBRAIC, LOWER, pathlb);
	// SetFunctionBounds(ALGEBRAIC, UPPER, pathub);
}

// EvaluateJacobians() is where analytic time, state, and control
// Jacobians are evaluated
void PathObj::EvaluateJacobians() {
	Rvector X = GetStateVector();
	Rvector U = GetControlVector();

	// Extract specific variables if needed, e.g.:
	// Real r = X(0);
	// Real th = X(1);
	// Real u = X(2);
	// Real v = X(3);
	// Real Acc = 0.01;
	// Real angle = U(0);

	// Create state jacobian where n is the number of states. Automatically
	// is set to zero initially.
	Integer n = 			// number of states
	Integer m = 			// number of controls
	Rmatrix jac(n, n);

	// Fill in nonzero Jacobian elements, e.g.:
	// jac(0,2) = 1.0;
	// jac(1,0) = -v/(r*r);
	// jac(1,3) = 1.0/r;

	// Create and fill in control jacobian where m is the number of controls.
	Rmatrix ujac(n, m);

	// Create and fill in time jacobian.
	Rmatrix tjac(n, 1);

	// Add derivatives of path constraints, if they exist, e.g. from
	// path constraint in the UserPointFunction class:
	// Rmatrix dpathx(1,n), dpathu(1,m), dpatht(1,1);
	// dpathx(0) = U(0);
	// dpathu(0) = X(0);

	// Set Jacobians
	SetJacobian(DYNAMICS, STATE, jac);
	SetJacobian(DYNAMICS, CONTROL, ujac);
	SetJacobian(DYNAMICS, TIME, tjac);

	// If there are path constraints:
	// SetJacobian(ALGEBRAIC, STATE, dpathx);
	// SetJacobian(ALGEBRAIC, CONTROL, dpathu);
	// SetJacobian(ALGEBRAIC, TIME, dpatht);
}