%==========================================================================
%% =====  Initializations
%==========================================================================
% ClearAll
global igrid iGfun jGvar traj 

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                           = Trajectory();
traj.pathFunctionName          = 'HyperSensitivePathFunctionMultiPhase';   
traj.plotFunctionName          = 'HyperSensitivePlotFunctionMultiPhase'; 
traj.showPlot                  = true();
traj.plotUpdateRate            = 2;
traj.costLowerBound            = 0;
traj.costUpperBound            = Inf;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set Time Properties
phase1                         = RadauPhase();
phase1.meshIntervalFractions   = [ -1 -.5 1];
phase1.meshIntervalNumPoints   = [10 10 ]';
phase1.initialTimeLowerBound   = 0;
phase1.initialEpoch            = 0;
phase1.initialTimeUpperBound   = 0;
phase1.finalTimeLowerBound     = 30;
phase1.finalEpoch              = 20;
phase1.finalTimeUpperBound     = 30;

%  Set state and control properties
phase1.numStates               = 1;
phase1.initialGuessMode        = 'LinearNoControl';
phase1.initialStateLowerBound  = 1;
phase1.initialGuessState       = .5;
phase1.initialStateUppperBound = 1;
phase1.finalStateLowerBound    = -1;
phase1.finalGuessState         = .2;
phase1.finalStateUpperBound    = 1;
phase1.stateLowerBound         = -50;
phase1.stateUpperBound         = 50;
phase1.numControls             = 1;
phase1.controlUpperBound       = 50;
phase1.controlLowerBound       = -50;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set Time Properties
phase2                         = RadauPhase();
phase2.meshIntervalFractions   = [ -1 -.5 1];
phase2.meshIntervalNumPoints   = [10 20]';
phase2.initialTimeLowerBound   = 20;
phase2.initialEpoch            = 30;
phase2.initialTimeUpperBound   = 30;
phase2.finalTimeLowerBound     = 50;
phase2.finalEpoch              = 50;
phase2.finalTimeUpperBound     = 50;

%  Set state and control properties
phase2.numStates               = 1;
phase2.numControls             = 1;
phase2.initialGuessMode        = 'LinearNoControl';
phase2.initialStateLowerBound  = -.2;
phase2.initialGuessState       = -.2;
phase2.initialStateUppperBound = .2;
phase2.finalStateLowerBound    = 1;
phase2.finalGuessState         = 1;
phase2.finalStateUpperBound    = 1;
phase2.stateLowerBound         = -50;
phase2.stateUpperBound         = 50;
phase2.controlUpperBound       = 50;
phase2.controlLowerBound       = -50;

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList        = {phase1,phase2};
traj.linkageConfig{1} = {phase1,phase2};
[z,F,xmul,Fmul]       = traj.Optimize();
