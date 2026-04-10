% Set the derivative option
snseti('Derivative Option',1);
% Set the derivative verification level
snseti('Verify Level',-1);
% Set name of SNOPT print file
snprint('snoptmain.out');
% Print CPU times at bottom of SNOPT print file
snseti('Timing level',3);
% Echo SNOPT Output to MATLAB Command Window 
snscreen on;

% Bounds on State and Control
r0 = 1; theta0 = 0; vr0 = 0; vtheta0 = 1; m0 = 1;
vrf = 0;  
rmin = 0.5; rmax = 5;
thetamin = 0; thetamax = 4*pi;
vrmin = -10; vrmax = 10;
vthetamin = -10; vthetamax = 10;
mmin = 0.1; mmax = m0;
u1min = -10; u1max = 10;
u2min = -10; u2max = 10;
t0min = 0; t0max = 0;
tfmin = 3.32; tfmax = 3.32;
NLGR = traj.phaseList{1}.numRadauPoints;

% Supply an initial guess
rguess = linspace(r0,1.5,NLGR+1).';
thetaguess = linspace(theta0,theta0,NLGR+1).';
vrguess = linspace(vr0,vrf,NLGR+1).';
vthetaguess = linspace(vtheta0,vtheta0,NLGR+1).';
mguess = linspace(m0,m0,NLGR+1).';
u1guess = linspace(1,1,NLGR).';
u2guess = linspace(0,0,NLGR).';
t0guess = 0;
tfguess = 3.32;
z0 = [rguess;thetaguess;vrguess;vthetaguess;mguess;u1guess;u2guess;t0guess;tfguess];

% Set initial guess on basis and Lagrange multipliers to zero
zmul = zeros(size(z0));
zstate = zmul;
Fmul = zeros(size(Fmin));
Fstate = Fmul;
ObjAdd = 0;
% Row 1 is the objective function row
ObjRow = 1;
% Assume for simplicity that all constraints 
% are purely nonlinear
AA = [];
iAfun = [];
jAvar = [];
userfun = 'orbitRaisingFunWrapper';

[iGfun,jGvar] = find(traj.sparsityPattern);
