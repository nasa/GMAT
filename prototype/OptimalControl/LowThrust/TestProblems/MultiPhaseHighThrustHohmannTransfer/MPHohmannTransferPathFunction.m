function [obj] = HohmannTransferPathFunction(obj)


%%  Define constants
DU  = 6378.1363;
TU  = 806.810991306733;
MU  = 1000;
mu  = 398600.4415*TU^2/DU^3;
g   = (9.81/1000)*TU*TU/DU;
T   = (500/1000)*TU*TU/DU/MU;  % kg*km/s^2;
Isp = 300/TU;

if obj.phaseNum ~= 1 || obj.phaseNum ~= 3
    T = T/1000;
end

%%  Extract state and control components
rv  = obj.stateVec(1:3,1);
vv  = obj.stateVec(4:6,1);
r   = sqrt(rv'*rv);
m   = obj.stateVec(7,1);
uv  = obj.controlVec(1:3,1);

%%  Compute the dynamics
thrustVec = T/m*uv;
thrustMag = sqrt(thrustVec'*thrustVec);
dynFunctions(1:3,1) = vv ;
dynFunctions(4:6,1) = -mu/r^3*rv + thrustVec;
dynFunctions(7,1)   = -thrustMag/Isp/g;
obj.dynFunctions    = dynFunctions;

%%  Compute the algebraic path functions.  
% This is constant thrust constraint
thrustConstraint = sqrt(uv(1)*uv(1) + uv(2)*uv(2) + uv(3)*uv(3));
radiusConstraint = r;
algFunctions     = [thrustConstraint;];
obj.algFunctions = algFunctions;



