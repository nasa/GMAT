#include "PathObj.hpp"

// EvaluateFunctions() is where the dynamics and path constraints
// are set
void PathObj::EvaluateFunctions() {
    // Get states and controls for the given segment
    Rvector X = GetStateVector();
    Rvector U = GetControlVector();

    // Constants
    Real mu = 0.012150585609624037;
    Real TU = 375699.81732246041;
    Real DU = 384747.96285603708;
    Real thrust_limit = 1.0;
    Real sqrtEps = sqrt(2.22044604925031e-16);
    Real g0 = 9.80665;
    Real Isp = 3000.0;
    Real mscale = 1000.0;
    Real min_dist_moon = 0.01;
    Real min_dist_earth = 0.01;

    // States and controls
    Real x = X(0);
    Real y = X(1);
    Real z = X(2);
    Real vx = X(3);
    Real vy = X(4);
    Real vz = X(5);
    Real mbar = X(6) * mscale;
    Real ux = U(0);
    Real uy = U(1);
    Real uz = U(2);

    // Helper quantities
    Real umag = sqrt(ux * ux + uy * uy + uz * uz + sqrtEps);
    Real d1 = sqrt((x + mu) * (x + mu) + y * y + z * z);
    Real d2 = sqrt((x - 1.0 + mu) * (x - 1.0 + mu) + y * y + z * z);
    Real d13 = d1 * d1 * d1;
    Real d23 = d2 * d2 * d2;

    // State equations
    Real xdot = vx;
    Real ydot = vy;
    Real zdot = vz;
    Real vxdot = x + 2.0 * vy - (1.0 - mu) * (x + mu) / d13 - mu * (x + mu - 1.0) / d23 + TU * TU * ux / (DU * mbar * 1000.0);
    Real vydot = y - 2.0 * vx - (1.0 - mu) * y / d13 - mu * y / d23 + TU * TU * uy / (DU * mbar * 1000.0);
    Real vzdot = -(1.0 - mu) * z / d13 - mu * z / d23 + TU * TU * uz / (DU * mbar * 1000.0);
    Real mdot = -TU * umag / (Isp * g0 * mscale);

    Rvector dynamics(7, xdot, ydot, zdot, vxdot, vydot, vzdot, mdot);
    SetFunctions(DYNAMICS, dynamics);

    // Add path constraints
    Rvector path(3), pathlb(3), pathub(3);
    path(0) = umag;
    path(1) = d2;
    path(2) = d1;
    pathlb(0) = 0.0;
    pathlb(1) = min_dist_moon;
    pathlb(2) = min_dist_earth;
    pathub(0) = thrust_limit;
    pathub(1) = 100.0;
    pathub(2) = 100.0;
    SetFunctions(ALGEBRAIC, path);
    SetFunctionBounds(ALGEBRAIC, LOWER, pathlb);
    SetFunctionBounds(ALGEBRAIC, UPPER, pathub);
}

void PathObj::EvaluateJacobians() {
    Rvector X = GetStateVector();
    Rvector U = GetControlVector();

    Real mu = 0.012150585609624037;
    Real TU = 375699.81732246041;
    Real DU = 384747.96285603708;
    Real thrust_limit = 1.0;
    Real sqrtEps = sqrt(2.22044604925031e-16);
    Real g0 = 9.80665;
    Real Isp = 3000.0;
    Real mscale = 1000.0;
    Real min_dist_moon = 0.01;
    Real min_dist_earth = 0.01;

    Real x = X(0);
    Real y = X(1);
    Real z = X(2);
    Real vx = X(3);
    Real vy = X(4);
    Real vz = X(5);
    Real mbar = X(6) * mscale;
    Real ux = U(0);
    Real uy = U(1);
    Real uz = U(2);

    // Helper quantities
    Real umag = sqrt(ux * ux + uy * uy + uz * uz + sqrtEps);
    Real d2 = sqrt((mu + x - 1.0) * (mu + x - 1.0) + y * y + z * z);
    Real d1 = sqrt((mu + x) * (mu + x) + y * y + z * z);
    Real d13 = d1 * d1 * d1;
    Real d15 = d13 * d1 * d1;
    Real d23 = d2 * d2 * d2;
    Real d25 = d23 * d2 * d2;

    Integer n = 7;      // number of states
    Integer nu = 3;     // number of controls

    // Create state jacobian where n is the number of states. Automatically
    // is set to zero initially.
    Rmatrix jac(n, n);
    jac(0, 3) = 1.0;
    jac(1, 4) = 1.0;
    jac(2, 5) = 1.0;
    jac(3, 0) = 1.0 - (1.0 - mu) / d13 + 3.0 * (1 - mu) * (x + mu) * (x + mu) / d15 - mu / d23 + 3.0 * mu * (x + mu - 1.0) * (x + mu - 1.0) / d25;
    jac(3, 1) = 3.0 * (1.0 - mu) * (x + mu) * y / d15 + 3.0 * mu * (x + mu - 1.0) * y / d25;
    jac(3, 2) = 3.0 * (1.0 - mu) * (x + mu) * z / d15 + 3.0 * mu * (x + mu - 1.0) * z / d25;
    jac(3, 4) = 2.0;
    jac(3, 6) = -TU * TU * ux / (DU * mbar * mbar * 1000.0);
    jac(4, 0) = jac(3, 1);
    jac(4, 1) = 1.0 - (1.0 - mu) / d13 + 3.0 * (1.0 - mu) * y * y / d15 - mu / d23 + 3.0 * mu * y * y / d25;
    jac(4, 2) = 3.0 * (1.0 - mu) * y * z / d15 + 3.0 * mu * y * z / d25;
    jac(4, 3) = -2.0;
    jac(4, 6) = -TU * TU * uy / (DU * mbar * mbar * 1000.0);
    jac(5, 0) = jac(3, 2);
    jac(5, 1) = jac(4, 2);
    jac(5, 2) = -(1.0 - mu) / d13 + 3.0 * (1.0 - mu) * z * z / d15 - mu / d23 + 3.0 * mu * z * z / d25;
    jac(5, 6) = -TU * TU * uz / (DU * mbar * mbar * 1000.0);

    // Create and fill in control jacobian where m is the number of controls.
    Rmatrix ujac(n, nu);
    ujac(3, 0) = TU * TU / (DU * mbar * 1000.0);
    ujac(4, 1) = TU * TU / (DU * mbar * 1000.0);
    ujac(5, 2) = TU * TU / (DU * mbar * 1000.0);
    ujac(6, 0) = -TU * ux / (umag * Isp * g0 * mscale);
    ujac(6, 1) = -TU * uy / (umag * Isp * g0 * mscale);
    ujac(6, 2) = -TU * uz / (umag * Isp * g0 * mscale);

    // Create and fill in time jacobian.
    Rmatrix tjac(n, 1);

    // Add derivatives of path constraints
    Rmatrix dpathx(3, n), dpathu(3, nu), dpatht(3, 1);
    dpathx(1, 0) = (mu + x - 1) / d2;
    dpathx(1, 1) = y / d2;
    dpathx(1, 2) = z / d2;
    dpathx(2, 0) = (x + mu) / d1;
    dpathx(2, 1) = y / d1;
    dpathx(2, 2) = z / d1;

    dpathu(0, 0) = ux / umag;
    dpathu(0, 1) = uy / umag;
    dpathu(0, 2) = uz / umag;

    // Set Jacobians
    SetJacobian(DYNAMICS, STATE, jac);
    SetJacobian(DYNAMICS, CONTROL, ujac);
    SetJacobian(DYNAMICS, TIME, tjac);

    // Set path constraints:
    SetJacobian(ALGEBRAIC, STATE, dpathx);
    SetJacobian(ALGEBRAIC, CONTROL, dpathu);
    SetJacobian(ALGEBRAIC, TIME, dpatht);
}