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
traj.pathFunctionName   = 'BrachistichronePathFunction_HS';
traj.pointFunctionName  = 'BrachistichronePointFunction_HS';
traj.plotFunctionName   = 'BrachistichronePlotFunction_HS';
traj.showPlot           = true();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = HermiteSimpsonPhase;
phase1.meshIntervalFractions   = [0 1];
phase1.meshIntervalNumPoints   = 25;

%  Set time properties
phase1.initialEpoch            = 0;
phase1.finalEpoch              = 20;
phase1.initialTimeLowerBound   = 0;
phase1.initialTimeUpperBound   = 0;
phase1.finalTimeLowerBound     = 0;
phase1.finalTimeUpperBound     = 100;

%  Set state and control properties
phase1.numStates                 = 3;
phase1.numControls               = 1;
phase1.initialGuessMode          = 'LinearNoControl';
phase1.initialStateLowerBound    = [0 0 0]';
phase1.initialGuessState         = [0 0 0]'; % [r0 theta0 vr0 vtheta0 m0]
phase1.initialStateUppperBound   = [0 0 0]';
phase1.initialGuessEpoch         = 0;
phase1.finalStateLowerBound      = [2 -2 -10]';
phase1.finalGuessState           = [2 -2 -2]'; % [rf thetaf vrf vthetaf mf]
phase1.finalStateUpperBound      = [2 -2 0]';
phase1.finalGuessEpoch           = 2;
phase1.stateLowerBound           = [-10 -10 -10]';
phase1.stateUpperBound           = [10 0 0]';
phase1.controlUpperBound         = [10]';
phase1.controlLowerBound         = [-10]';

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();
