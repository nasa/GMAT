function [obj] = HohmannTransferPathFunction(obj)

%obj.costFunction = 0.5*(obj.stateVec*obj.stateVec+ ...
%    obj.controlVec*obj.controlVec);

%%  Define constants
DU  = 6378.1363;
TU  = 806.810991306733;
MU  = 1000;
mu  = 398600.4415*TU^2/DU^3;
g   = (9.81/1000)*TU*TU/DU;
T   = (500/1000)*TU*TU/DU/MU;  % kg*km/s^2;
Isp = 300/TU;

%%  Extract state and control components
rv  = obj.stateVec(1:2,1);
vv  = obj.stateVec(3:4,1);
r   = sqrt(rv'*rv);
m   = obj.stateVec(5,1);
uv  = obj.controlVec(1:2,1);

%%  Compute the dynamics
thrustVec = T/m*uv;
thrustMag = sqrt(thrustVec'*thrustVec);
dynFunctions(1:2,1) = vv ;
dynFunctions(3:4,1) = -mu/r^3*rv + thrustVec;
dynFunctions(5,1)   = -thrustMag/Isp/g;
obj.dynFunctions    = dynFunctions;

%%  Compute the algebraic path functions.  
% This is constant thrust constraint
thrustConstraint = uv(1)*uv(1) + uv(2)*uv(2);
radiusConstraint = r;
algFunctions = [thrustConstraint ];% radiusConstraint];
obj.algFunctions = algFunctions;
% obj.costFunction = -m;



