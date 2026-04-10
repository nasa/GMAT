function Run()

%  The purpose of this function is to make sure the canonical units in the
%  orbit dynamics is implemented correcty.  To test that, try propagation
%  with and without non-dimensionalization, and compare solutions.

global mode 

%% Configure initial state
mu  = 398600.4415;
mode = 1;
if mode == 1
    DU  = 6378.1363;
    TU  = 806.810991306733;
    MU  = 1000;
else
    DU  = 1;
    TU  = 1;
    MU  = 1;
end

%  Dimensional Quantities
circRad     = 6900;
meanMotion  = sqrt(mu/circRad^3);
rv0         = [circRad 0 0]';
vv0         = [0 sqrt(mu/circRad) 0]';
m0          = 1250;
orbitPeriod = 2*pi/meanMotion;

%  Non-Dimensional Quantities
rv0         = rv0/DU;
vv0         = vv0/DU*TU;
m0          = m0/MU;
X0          = [rv0;vv0;m0];
orbitPeriod = orbitPeriod/TU;
odeOpt      = odeset('RelTol',1e-9,'AbsTol',1e-9);
[tVec,xVec] = ode45(@PathFunction,[0 orbitPeriod/2], X0,odeOpt );

xVec(:,1:3) = xVec(:,1:3)*DU;
xVec(:,4:6) = xVec(:,4:6)*DU/TU;
xVec(:,7)   = xVec(:,7)*MU;

xVec(end,:)

function [Xdot] = PathFunction(t,X)

global mode 

%  Hard code the control for now
sqrt3 = 1/sqrt(3);
U     = [sqrt3 sqrt3 sqrt3]';

%%  Define constants
if mode == 1
    DU  = 6378.1363;
    TU  = 806.810991306733;
    MU  = 1000;
else
    DU  = 1;
    TU  = 1;
    MU  = 1;
end

mu  = 398600.4415*TU^2/DU^3;
g   = (9.81/1000)*TU*TU/DU;
T   = (100/1000)*TU*TU/DU/MU;  % kg*km/s^2;
Isp = 300/TU;

%%  Extract state and control components
rv  = X(1:3,1);
vv  = X(4:6,1);
m   = X(7,1);
uv  = U(1:3,1);

%%  Compute the dynamics
dynFunctions(1:3,1) = vv ;
dynFunctions(4:6,1) = -mu/norm(rv)^3*rv + T/m*uv;
dynFunctions(7,1)   = -T/Isp/g;
obj.dynFunctions    = dynFunctions;
Xdot                = dynFunctions;

%%  Compute the algebraic path functions.
% This is constant thrust constraint
obj.algFunctions = uv(1)*uv(1) + uv(2)*uv(2) + uv(3)*uv(3) - T;
