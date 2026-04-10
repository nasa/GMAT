% Set the derivative option
snseti('Derivative Option',0);
% Set the derivative verification level
snseti('Verify Level',-1);
snseti('Feasibility tolerance',1e-4)
% Set name of SNOPT print file
snprint('snoptmain.out');
% Print CPU times at bottom of SNOPT print file
snseti('Timing level',3);
% Echo SNOPT Output to MATLAB Command Window 
snscreen on;

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
AA      = [];
iAfun   = [];
jAvar   = [];
userfun = 'SNOPTFunctionWrapper';

[iGfun,jGvar] = find(traj.sparsityPattern);
