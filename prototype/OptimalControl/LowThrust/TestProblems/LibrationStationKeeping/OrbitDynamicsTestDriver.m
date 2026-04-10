function Run()

%  The purpose of this function is to make sure the canonical units in the
%  orbit dynamics is implemented correcty.  To test that, try propagation
%  with and without non-dimensionalization, and compare solutions.

global mode 

%% Configure initial state
A     = 3.9045;
omXY  = 2.0570;
omZ   = 1.9851;
k     = -0.3138;
cnt   = 0;
AU    = 149597871;
%  Specific liss properties
aXY   = 260285.777055646;
aZ    = 163495.200129871;
phiXY = pi/2;
phiZ  = pi/2;
for time = 0:.01:5*pi
    cnt     =  cnt + 1;
    t(cnt)  = time;
    x(cnt)  =  aXY*k*cos(omXY*time + phiXY);
    y(cnt)  =  aXY*sin(omXY*time + phiXY);
    z(cnt)  =  aZ*cos(phiZ*time + phiZ);
    vx(cnt) = -aXY*k*omXY*sin(omXY*time + phiXY);
    vy(cnt) =  aXY*omXY*cos(omXY*time + phiXY);
    vz(cnt) = -aZ*phiZ*sin(omZ*time + phiZ);
end
figure(1)
plot3(x,y,z);
view([1 0 0]); axis equal; drawnow;
y0 = [x(1) y(1) z(1) vx(1) vy(1) vz(1)]';
view([1 0 0]);
xlabel('X-Axis'); ylabel('Y-Axis'); zlabel('Z-Axis');
figure(2);
plot(t,x,t,y,t,z);

muE  = 398600.4415;
muS  = 132712440017.99;
mE   = 3.003571e-06;
mS   = 1 - mE;
mu   = mE;
mode = 1;
if mode == 1
    DU  = AU;
    TU  = 806.810991306733;
    MU  = muE + muS;
else
    DU  = 1;
    TU  = 1;
    MU  = 1;
end

%  Dimensional Quantities
% circRad     = 6900;
% meanMotion  = sqrt(mu/circRad^3);
% rv0         = [circRad 0 0]';
% vv0         = [0 sqrt(mu/circRad) 0]'*1000;
 m0          = 1250/MU;

odeOpt      = odeset('RelTol',1e-9,'AbsTol',1e-9);
[tVec,xVec] = ode45(@PathFunction,[0 pi],[y0; m0],odeOpt );

figure(10)
plot3(xVec(:,1),xVec(:,2),xVec(:,3));
view([0 0 1]);

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
    DU  = 149597871;;
    TU  = 806.810991306733;
    MU  = 1000;
else
    DU  = 1;
    TU  = 1;
    MU  = 1;
end

mu  = 398600.4415*TU^2/DU^3;
g   = (9.81/1000)*TU*TU/DU;
T   = 0*(100/1000)*TU*TU/DU/MU;  % kg*km/s^2;
Isp = 300/TU;

mu  = 3.003571e-06;

%%  Extract state and control components
rv  = X(1:3,1);
vv  = X(4:6,1);
m   = X(7,1);
uv  = U(1:3,1);
x   = rv(1,1);
y   = rv(2,1);
z   = rv(3,1);
vx  = vv(1,1);
vy  = vv(2,1);
vz  = vv(3,1);

%%  Compute the dynamics
%  The CRTBP dynamcis
d1 = sqrt((x + mu)^2 + y^2 + z^2);
d2 = sqrt((x - 1 + mu)^2 + y^2 + z^2);
A  = (1 - mu)/d1^3 + mu/d2^3;
ax =  2*vy + (1 + 2*A)*vx;
ay = -2*vx + (1 - A)*vy;
az = -A*vz;

%%  Compute the dynamics
dynFunctions(1:3,1) = vv ;
dynFunctions(4:6,1) = [ax;ay;az] + T/m*uv;
dynFunctions(7,1)   = -T/Isp/g;
obj.dynFunctions    = dynFunctions;
Xdot                = dynFunctions;

%%  Compute the algebraic path functions.
% This is constant thrust constraint
obj.algFunctions = uv(1)*uv(1) + uv(2)*uv(2) + uv(3)*uv(3) - T;
