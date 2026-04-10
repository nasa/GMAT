function [C,G] = SNOPTFunctionWrapper(z);
%  Compute the cost and constraints
% Construct the vector of constraints plus the objective function

global iGfun jGvar traj
reDim = true();
nonDim = true();
traj.SetDecisionVector(z,reDim);
C = traj.GetCostConstraintFunctions(nonDim);
J = traj.GetJacobian(nonDim);
if ~isempty(J)
    G = snfindG(iGfun,jGvar,J);
else
    G = [];
end
