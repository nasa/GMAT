function PropSRPwithSTM()

%%  Set up prop epoch
propDuration    = 86400;
outputStepSize  = 60;

%%  Define spacecraft properties
Sat.initialEpoch = 23158.00037076831;
Sat.Cd = 2.2;
Sat.Cr = 1.2;
Sat.DragArea = 20;
Sat.SRPArea = 20;
Sat.DryMass = 1000;
initialState = [-4453.7835859999996
-5038.2037559999999
-426.384456
3.8318880000000002
-2.8872209999999998
-6.0182320000000002];
            

%%  Define the force model
Force.UseSRP        = 0;
Force.Debug         = 0;
Force.Drag          = 'None';
Force.UseRelativity = 0;
Force.STM          = 1;
Force.deltaAT       = 32;
Force.xp           = -0.083901;
Force.yp           = 0.467420;
Force.deltaUT1     = -0.4709524;
Force.lod          = 0.0004155;
Force.flattening   = 0.0033527;
Force.bodyRadius   = 6378.1363;
Force.bodyMu       = 398600.4415;
Force.speedOfLight = 299792.458;
Force.muSun        = 132712440017.99;
Force.radiusSun    = 695990;
Force.ExponentialDensity.refHeight   = 0;
Force.ExponentialDensity.refDensity  = 1217000000;
Force.ExponentialDensity.scaleHeight = 8.5;
Force.MSISE.F107A  = 150;
Force.MSISE.F107   = 150;
Force.MSISE.Ap     = 15*ones(1,7);
Force.flux         = 1367;
Force.AU           = 149597870.691;



%  Configure integrator and propagate
odeOptions           = odeset('RelTol',2e-11,'AbsTol',1e-11);
[timeHist,stateHist] = ode113(@OrbitODE,[0 propDuration],...
                                     [initialState ;reshape(eye(6),36,1)], odeOptions, Force, Sat);
%  Report the data
%plot3(stateHist(:,1),stateHist(:,2),stateHist(:,3))
[m,n] = size(stateHist);
stateHist(m,1:6)'
stmLast = reshape(stateHist(m,7:42),6,6)
   
%%  Computes SRP force and variational terms
function [Xdot] = OrbitODE(t,X,Force,Sat)

% Initializations
mu     = 398600.4415;
rv     = X(1:3,1);
vv     = X(4:6,1);
stmMat    = reshape(X(7:42,1),6,6);
r      = norm(rv);
aVarEq = zeros(3);
bVarEq = eye(3,3);
cVarEq = zeros(3,3);
dVarEq = zeros(3,3);

% Compute two Body Force
accTwoBody   = -mu/r^3*rv;
dMatTwoBody  = zeros(3,3);
cMatTwoBody  = -mu/r^3*eye(3) + 3*mu*rv*rv'/r^5;

% Compute SRP terms
if Force.UseSRP 
  [accSRP, cMatSRP, dMatSRP] = GetSRPAccel(t,X,Force,Sat);
else
  accSRP = zeros(3,1); cMatSRP = zeros(3,3); dMatSRP = zeros(3,3);
end
%  Sum forces
Xdot(1:3,1) = X(4:6,1);
Xdot(4:6,1) = accTwoBody + accSRP;
cVarEq      = cMatTwoBody + 0*cMatSRP;
dVarEq      = dMatTwoBody + dMatSRP;

varMat = [aVarEq bVarEq; cVarEq dVarEq];
stmDot = varMat*stmMat;
Xdot(7:42) = reshape(stmDot,36,1);


