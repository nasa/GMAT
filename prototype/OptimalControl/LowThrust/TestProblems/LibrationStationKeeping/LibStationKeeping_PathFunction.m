function [obj] = LibStationKeeping_PathFunction(obj)

%obj.costFunction = 0.5*(obj.stateVec*obj.stateVec+ ...
%    obj.controlVec*obj.controlVec);

%%  Define constants
% DU  = 6378.1363;
% TU  = 806.810991306733;
% MU  = 1000;
% mu  = 398600.4415*TU^2/DU^3;
% g   = (9.81/1000)*TU*TU/DU;
% T   = (100/1000)*TU*TU/DU/MU;  % kg*km/s^2;
% Isp = 300/TU;

%%  Extract state and control components
rv  = obj.stateVec(1:3,1);
vv  = obj.stateVec(4:6,1);
x   = rv(1,1);
y   = rv(2,1);
z   = rv(3,1);
vx  = vv(1,1);
vy  = vv(2,1);
vz  = vv(3,1);
m   = obj.stateVec(7,1);
uv  = obj.controlVec(1:3,1);

%%  Compute the dynamics
%  The CRTBP dynamcis
d1 = sqrt((x + mu)^2 + y^2 + z^2);
d2 = sqrt((x - 1 + mu)^2 + y^2 + z^2);
A  = (1 - mu)/d1^3 + mu/d2^3;
ax =  2*vy +(1 + 2*A)*xdot;
ay = -2*vx +(1 - A)*xdot;
az = -A*vz;
%  The total sytem dynamics
thrustVec           = T/m*uv;
thrustMag           = sqrt(thrustVec'*thrustVec);
dynFunctions(1:3,1) = vv ;
dynFunctions(4:6,1) = [ax;ay;az] + thrustVec;
dynFunctions(7,1)   = -thrustMag/Isp/g;
obj.dynFunctions    = dynFunctions;

%%  Compute the algebraic path functions.  
% This is constant thrust constraint
thrustConstraint = uv(1)*uv(1) + uv(2)*uv(2) + uv(3)*uv(3);
algFunctions = thrustConstraint;
obj.algFunctions = algFunctions;




