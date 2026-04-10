function [obj] = OrbitRaisingPathFunction(obj)

%obj.costFunction = 0.5*(obj.stateVec*obj.stateVec+ ...
%    obj.controlVec*obj.controlVec);

%%  Define constants
mu = 1;
% CONSTANTS.m0 = 1;
mdot = 0.0749;
T = 0.2405;

%%  Extract state and control components
r         = obj.stateVec(1,1);
%theta     = obj.stateVec(2,1);
dr_dt     = obj.stateVec(3,1);
dtheta_dt = obj.stateVec(4,1);
m         = obj.stateVec(5,1);
u_r       = obj.controlVec(1,1);
u_theta   = obj.controlVec(2,1);

%%  Compute the dynamics
dynFunctions(1,1)  = dr_dt ;
dynFunctions(2,1)  = dtheta_dt/r;
dynFunctions(3,1)  = dtheta_dt*dtheta_dt/r - mu/r^2 + T/m*u_r;
dynFunctions(4,1)  = -dr_dt*dtheta_dt/r +T/m*u_theta;
dynFunctions(5,1)  = -mdot;
obj.dynFunctions   = dynFunctions;
% rdot = vr;
% thetadot = vtheta./r;
% vrdot = vtheta.^2./r-MU./r.^2+a.*u1;
% vthetadot = -vr.*vtheta./r+a.*u2;
% mdot = -mdot*ones(size(tLGR));

%%  Compute the algebraic path functions
obj.algFunctions = u_r^2 + u_theta^2;

