%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj 

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                    = Trajectory();
traj.pathFunctionName   = 'OrbitRaisingPathFunction_HS';
traj.pointFunctionName  = 'OrbitRaisingPointFunction_HS';
traj.plotFunctionName   = 'OrbitRaisingPlotFunction_HS';
traj.showPlot           = false();
traj.plotUpdateRate     = 3;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set Time Properties
phase1                           = HermiteSimpsonPhase();
phase1.meshIntervalFractions     = [0; 1];
phase1.meshIntervalNumPoints     = 15*ones(1,1);
phase1.initialEpoch              = 0;
phase1.finalEpoch                = 3.32;
phase1.initialTimeLowerBound     = 0;
phase1.initialTimeUpperBound     = 0;
phase1.finalTimeLowerBound       = 3.32;
phase1.finalTimeUpperBound       = 3.32;

%  Set state and control properties
phase1.numStates                 = 5;
phase1.initialGuessMode          = 'LinearUnityControl';
phase1.initialGuessState         = [1 0 0 1 1]'; % [r0 theta0 vr0 vtheta0 m0]
phase1.initialGuessEpoch         = 0;
phase1.finalGuessState           = [1 pi 0 1 1]'; % [rf thetaf vrf vthetaf mf]
phase1.finalGuessEpoch           = 1;
phase1.initialStateLowerBound    = [1 0 0 1 1]';
phase1.initialStateUppperBound   = [1 0 0 1 1]';
phase1.finalStateLowerBound      = [-1 -pi 0 -10 0]';
phase1.finalStateUpperBound      = [10 pi 0 10 1]';
phase1.stateLowerBound           = [0.5 0 -10 -10 0.1]';
phase1.stateUpperBound           = [5 4*pi 10 10 3]';
phase1.numControls               = 2;
phase1.controlUpperBound         = [10 10]';
phase1.controlLowerBound         = [-10 -10]';
phase1.algConstraintLowerBound   = 1;
phase1.algConstraintUpperBound   = 1;
phase1.eventConstraintLowerBound = [0];
phase1.eventConstraintUpperBound = [0];

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();