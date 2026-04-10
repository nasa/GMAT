function [odeRHS] = MOEDynWrapper(t,X)

global config pertForceDim

gravParam = 1.0;
dynFunc = ModEqDynamics(gravParam);
pertForces = [0 0 0]';
R = InertialToMOEForceSystem(X,gravParam);
pertForces = R*pertForces;
odeRHS = dynFunc.ComputeOrbitDerivatives(X,pertForces);




