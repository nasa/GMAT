function [odeRHS] = TwoBodyForce(~,X)

global pertForceDim gravParam

rMag = norm(X(1:3));
odeRHS(1:3,1) = X(4:6,1);
odeRHS(4:6,1) = -gravParam*X(1:3)/rMag^3;






