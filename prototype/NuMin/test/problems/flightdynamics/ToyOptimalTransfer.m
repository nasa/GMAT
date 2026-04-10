function x = ToyFormation

global ScaleVector To Xo Tf Xf
close all

%==========================================================================
%=========================== Define the segments ==========================
%==========================================================================

%-----  The initial orbits and epochs
To = 0; T1 =  2527.0;  T2 = 6847.0; T3 = 20448.0;  Tf = 20987.0;
Xo = [ -6300.280 3516.589 -675.12512 -3.25327 -6.46711 -1.47555]';
X1 = [ -5945.812 -10799.882 1942.380 7.20795 -1.14208 1.53441]';
X2 = [20000.797 10161.949 7544.113 0.72348 4.00652 0.00637]';
Xf = [ 6129.745 41171.675 6723.936 -2.99204 0.52330 -0.47664]';

%==========================================================================
%===================== Configure and Call Optimizer =======================
%==========================================================================

x0 = [T1;X1;T2;X2;T3];

PlotTrajectory(x0,1)
numVar    = size(x0,1);
TimeScale = 1000; PosScale = 1e3*ones(3,1); VelScale = 1*ones(3,1);
ScaleVector = [TimeScale;PosScale;VelScale;TimeScale;
    PosScale;VelScale;TimeScale];
x0 = inv(diag(ScaleVector))*x0;

%  Define optimizer settings and run the optimizer
% options = optimset('TolCon',1e-5,'TolFun',1e-5,'Display','iter', ...
%     'DerivativeCheck','Off','GradObj','On', 'GradConstr','On',...
%     'DiffMaxChange',.00001,'DiffMinChange',.000001,'MaxIter',300,...
%     'MaxFunEvals',3000,'Algorithm','sqp','ScaleProblem','obj-and-constr');
% [x,fval,exitflag] = fmincon(@CostFun,x0,[],[],[],[],...
%                                  [],[],@ConstraintFun,options);
%                              return

Options.DescentMethod     = 'SelfScaledBFGS';
Options.StepSearchMethod  = 'NocWright';
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 1550;
Options.MaxFunEvals       = 1500;
Options.TolCon            = 1e-5;
Options.TolX              = 1e-5;
Options.TolF              = 1e-6;
Options.TolGrad           = 1e-5;
Options.TolStep           = 1e-5;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';
Options.ConstraintMode    = 'miNLP';

sqpOpt = MiNLPClass(@CostFun,x0,[],[],[],[],...
    [],[],@ConstraintFun,Options);

[x,f,exitFlag,OutPut] = sqpOpt.Optimize()
x = diag(ScaleVector)*x;
PlotTrajectory(x,2)

%==========================================================================
%---------------------- Define the cost function
%==========================================================================
function [J,dJdx] = CostFun(x)

global ScaleVector To Xo Tf Xf rvoT1 vvoT1 rv1T1 vv1T1 rv1T2 vv1T2
global rv2T2 vv2T2 rv2T3 vv2T3 rvfT3 vvfT3 Phi1T2 Phi2T3

%  Unscale the input
x  = diag(ScaleVector)*x;
figure(1); PlotTrajectory(x,2);

%  Extract trajectory data
T1 = x(1,1); rv1T1 = x(2:4,1); vv1T1 = x(5:7,1);
T2 = x(8,1); rv2T2 = x(9:11,1); vv2T2 = x(12:14,1); T3 = x(15,1);

%  Prop initial and final orbits to T0 and Tf respectively
dT1o = (T1 - To);dT21  = (T2 - T1); dT32 = (T3 - T2); dTF3 = (T3 - Tf);
[rvoT1,vvoT1,PhioT1To] = PropOrbitandSTM(Xo,[0 dT1o],1);
[rvfT3,vvfT3,PhifT3TF] = PropOrbitandSTM(Xf,[0 dTF3],1);
[rv1T2,vv1T2,Phi1T2]   = PropOrbitandSTM([rv1T1;vv1T1],[0 dT21],1);
[rv2T3,vv2T3,Phi2T3]   = PropOrbitandSTM([rv2T2;vv2T2],[0 dT32],1);

%  Calculate the cost function
dV1 = vv1T1 - vvoT1; dv1 = sqrt(dV1'*dV1);
dV2 = vv2T2 - vv1T2; dv2 = sqrt(dV2'*dV2);
dV3 = vvfT3 - vv2T3; dv3 = sqrt(dV3'*dV3);
J   = dv1 + dv2 + dv3;

%  Calculate the gradient of the cost function
[av] = OrbitDot(0,[rvoT1;vvoT1;zeros(36,1)]); avoT1 = av(4:6,1);
[av] = OrbitDot(0,[rv1T2;vv1T2;zeros(36,1)]); av1T2 = av(4:6,1);
[av] = OrbitDot(0,[rv2T3;vv2T3;zeros(36,1)]); av2T3 = av(4:6,1);
[av] = OrbitDot(0,[rvfT3;vvfT3;zeros(36,1)]); avfT3 = av(4:6,1);
%----- Time derivatives
dJdx(1,1)     = -dV1'/dv1*(avoT1) +  dV2'/dv2*av1T2;
dJdx(8,1)     = -dV2'/dv2*(av1T2) +  dV3'/dv3*av2T3;
dJdx(15,1)    =  dV3'/dv3*(avfT3 - av2T3);
%----- Position derivatives
dJdx(2:4,1)   = (-dV2'/dv2*Phi1T2(4:6,1:3))';
dJdx(9:11,1)  = (-dV3'/dv3*Phi2T3(4:6,1:3))';
%----- Velocity derivatives
dJdx(5:7,1)   = (dV1'/dv1 - dV2'/dv2*Phi1T2(4:6,4:6))';
dJdx(12:14,1) = (dV2'/dv2 - dV3'/dv3*Phi2T3(4:6,4:6))';
%----- Nondimensionalize
for i = 1:size(x,1)
    dJdx(i,:) = dJdx(i,:)*ScaleVector(i,1);
end
J = J/10;
dJdx = dJdx/10;

%==========================================================================
%----- Define the constraint function
%==========================================================================
function [gi,ge,dgidx,dgedx] = ConstraintFun(x)

global ScaleVector To Xo Tf Xf rvoT1 vvoT1 rv1T1 vv1T1 rv1T2 vv1T2
global rv2T2 vv2T2 rv2T3 vv2T3 rvfT3 vvfT3 Phi1T2 Phi2T3 dgedx

%  Unscale the input
x = diag(ScaleVector)*x;

%  Calculate the constraints
ge = zeros(9,1);
ge(1:3,1) = rv1T1 - rvoT1;
ge(4:6,1) = rv2T2 - rv1T2;
ge(7:9,1) = rvfT3 - rv2T3;

%  Calculate the constraint Jacobian
dgedx            =  zeros(15,9);
dgedx(1,1:3)     = -vvoT1';
dgedx(1,4:6)     =  vv1T2';
dgedx(2:4,1:3)   =  eye(3);
dgedx(2:4,4:6)   = -Phi1T2(1:3,1:3)';
dgedx(5:7,4:6)   = -Phi1T2(1:3,4:6)';
dgedx(8,4:6)     = -vv1T2';
dgedx(9:11,4:6)  =  eye(3);
dgedx(8,7:9)     =  vv2T3;
dgedx(9:11,7:9)  = -Phi2T3(1:3,1:3)';
dgedx(12:14,7:9) = -Phi2T3(1:3,4:6)';
dgedx(15,7:9)    =  (vvfT3 - vv2T3)';

% Nondimensionalize the Jacobian
for i = 1:size(x,1)
    dgedx(i,:) = dgedx(i,:)*ScaleVector(i,1);
end
ge    = ge/1000;
dgedx = dgedx/1000;

% Tell matlab there are no inequality constraints
gi    = [];
dgidx = [];

%==========================================================================
%========== The two body orbit propagator
%==========================================================================
function [rvmat,vvmat,Phimat] = PropOrbitandSTM(xv0,timeVec,flag)

X0 = [xv0;reshape(eye(6),36,1)];
options = odeset('AbsTol',1e-7,'RelTol',1e-7);
[thist, Xhist] = ode113(@OrbitDot, timeVec, X0, options);

if flag == 1
    %----- Only output the last ephemeris point
    n   = size(thist,1);
    rvmat  = Xhist(n,1:3)';
    vvmat  = Xhist(n,4:6)';
    Phimat = reshape(Xhist(n,7:42),6,6);
else
    %----- Output the entire ephemeris
    rvmat  = Xhist(:,1:3);
    vvmat  = Xhist(:,4:6);
    Phimat = Xhist(:,7:42);
end

%==========================================================================
%========== The orbital and variational equations
%==========================================================================
function [xdot] = OrbitDot(t,X)

%  constants and definitions
mu          = 398600.4415;
I3          = eye(3);
rv          = X(1:3,1);
vv          = X(4:6,1);
r           = norm(rv);
Phi         = reshape(X(7:42,1),6,6);

% orbit equations of motion
xdot(1:3,1) = vv;
xdot(4:6,1) = -mu/r^3*rv;

% variational equations
A           = zeros(6,6);
A(1:3,4:6)  = I3;
A(4:6,1:3)  = -mu/r^3*I3 + 3*mu*rv*rv'/r^5;
Phidot      = A*Phi;
xdot(7:42)  = reshape(Phidot,36,1);

%==========================================================================
%============================= Plot the Trajecrory ========================
%==========================================================================
function PlotTrajectory(x,LineWidth)
return
global ScaleVector To Xo Tf Xf
figure(1); hold off;
T1  = x(1,1); rv1T1 = x(2:4,1); vv1T1 = x(5:7,1);
T2  = x(8,1); rv2T2 = x(9:11,1); vv2T2 = x(12:14,1); T3  = x(15,1);

%  Propagate and plot the initial and final orbits
[rvomat,vvomat] = PropOrbitandSTM(Xo,[0 10000],0);
[rvfmat,vvfmat] = PropOrbitandSTM(Xf,[0 86400],0);
m = size(rvomat,1);
plot3(rvomat(:,1),rvomat(:,2),rvomat(:,3),'g','LineWidth',LineWidth); hold on;
axis equal;
axis([-6e4 6e4 -6e4 6e4]); view([0 0 1]); drawnow;
m = size(rvfmat,1);
plot3(rvfmat(:,1),rvfmat(:,2),rvfmat(:,3),'r','LineWidth',LineWidth)

%----- Propagate the segments
dt1o = (T1 - To); dt3f = (T3 - Tf); dt21 = (T2 - T1); dt32 = (T3 - T2);
[rvomat,vvomat] = PropOrbitandSTM(Xo,[0 dt1o],0);
[rvfmat,vvfmat] = PropOrbitandSTM(Xf,[0 dt3f],0);
[rv1mat,vv1mat] = PropOrbitandSTM([rv1T1;vv1T1],[0 dt21],0);
[rv2mat,vv2mat] = PropOrbitandSTM([rv2T2;vv2T2],[0 dt32],0);

%----- Plot the segments
m = size(rvomat,1);
plot3(rvomat(1,1),rvomat(1,2),rvomat(1,3),'ro')
plot3(rvomat(m,1),rvomat(m,2),rvomat(m,3),'go')
m = size(rvfmat,1);
plot3(rvfmat(1,1),rvfmat(1,2),rvfmat(1,3),'go')
plot3(rvfmat(m,1),rvfmat(m,2),rvfmat(m,3),'ro')
m = size(rv1mat,1);
plot3(rv1mat(:,1),rv1mat(:,2),rv1mat(:,3),'b','LineWidth',LineWidth)
plot3(rv1mat(1,1),rv1mat(1,2),rv1mat(1,3),'go')
plot3(rv1mat(m,1),rv1mat(m,2),rv1mat(m,3),'ro')
m = size(rv2mat,1);
plot3(rv2mat(:,1),rv2mat(:,2),rv2mat(:,3),'k','LineWidth',LineWidth)
plot3(rv2mat(1,1),rv2mat(1,2),rv2mat(1,3),'go')
plot3(rv2mat(m,1),rv2mat(m,2),rv2mat(m,3),'ro')

drawnow
