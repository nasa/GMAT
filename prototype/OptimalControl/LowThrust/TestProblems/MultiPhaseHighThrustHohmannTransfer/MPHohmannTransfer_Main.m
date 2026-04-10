%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global iGfun jGvar traj igrid

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  set cost function properties
traj                    = Trajectory();
traj.pathFunctionName   = 'MPHohmannTransferPathFunction';
traj.pointFunctionName  = 'MPHohmannTransferPointFunction';
traj.plotFunctionName   = 'MPHohmannTransferPlotFunction';
traj.showPlot           = true();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.displayDebugStatus = false();

%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================

%%  Set Time Properties
phase1                           = RadauPhase();
phase1.meshIntervalFractions     = [ -1 1];                
phase1.meshIntervalNumPoints     = [15]';
phase1.numStates                 = 7;
phase1.numControls               = 3;
phase1.initialGuessMode          = 'LinearUnityControl';
% ===== Initial state data
phase1.initialTimeLowerBound     = 0;
phase1.initialEpoch              = 0;
phase1.initialTimeUpperBound     = 0;
phase1.initialGuessState         = [ 1.2 0 0 0  0.96 0 1.25];
phase1.initialStateLowerBound    = phase1.initialGuessState;
phase1.initialStateUppperBound   = phase1.initialGuessState;
% ===== Final state data
phase1.finalTimeLowerBound       = 0.1;
phase1.finalEpoch                = 0.33494625378467;
phase1.finalTimeUpperBound       = 1.63494625378467;
phase1.finalStateLowerBound      = [-4   -4  -4  -4 -4     -4  .900 ]';
phase1.finalGuessState           = [ 1.2 0.5  0   0  0.96  0   1.25];
phase1.finalStateUpperBound      = [ 4    4   4   4  4     4   1.400]';
% ===== bounds along the path but not at end points
phase1.stateLowerBound           = [-10 -10 -10 -10 -10 -10 .900 ]';
phase1.stateUpperBound           = [ 10  10  10  10  10  10 1.400]';
% ===== Control and constraint bounds
phase1.controlUpperBound         = [1 1 1]';
phase1.controlLowerBound         = [-1 -1 -1]';
phase1.algConstraintLowerBound   = [1 ]';
phase1.algConstraintUpperBound   = [1 ]';
phase1.eventConstraintLowerBound = []';
phase1.eventConstraintUpperBound = []';

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%%  Set Time Properties
phase2                           = RadauPhase();
phase2.meshIntervalFractions     = [ -1 1];                
phase2.meshIntervalNumPoints     = [15]';
phase2.numStates                 = 7;
phase2.numControls               = 3;
% ===== Initial state data
phase2.initialGuessMode          = 'LinearUnityControl';
phase2.initialTimeLowerBound     = 0;
phase2.initialEpoch              = 0.53494625378467;
phase2.initialTimeUpperBound     = 1;
phase2.initialStateLowerBound    = [-4  -4 -4   -4 -4 -4  .300 ]';
phase2.initialGuessState         = [ 1.2 1.2 0 -0.96 0  0  1.25];
phase2.initialStateUppperBound   = [ 4   4  4    4  4  4  1.900 ]';
% ===== Final state data
phase2.finalTimeLowerBound       = 3;
phase2.finalEpoch                = 3.53494625378467;
phase2.finalTimeUpperBound       = 4;
phase2.finalGuessState           = [ -1.2 1.2 0 -0.96 0 0 1.25];
phase2.finalStateLowerBound      = [-10 -10 -10 -10 -10 -10 .900 ]';
phase2.finalStateUpperBound      = [ 10  10  10  10  10  10 1.400]';

% ===== bounds along the path but not at end points
phase2.stateLowerBound           = [-10 -10 -10 -10 -10 -10 .900 ]';
phase2.stateUpperBound           = [ 10  10  10  10  10  10 1.400]';
phase2.controlUpperBound         = [1 1 1]';
phase2.controlLowerBound         = [-1 -1 -1]';
phase2.algConstraintLowerBound   = [0 ]';
phase2.algConstraintUpperBound   = [1 ]';
phase2.eventConstraintLowerBound = []';
phase2.eventConstraintUpperBound = []';

%==========================================================================
%% =====  Define Properties for Phase 3
%==========================================================================

%%  Set Time Properties
phase3                           = RadauPhase();
phase3.numStates                 = 7;
phase3.numControls               = 3;
phase3.initialGuessMode          = 'LinearUnityControl';
phase3.meshIntervalFractions     = [ -1 1];                
phase3.meshIntervalNumPoints     = [15]';
% ===== Initial state data
phase3.initialTimeLowerBound     = 2;
phase3.initialEpoch              = 3.53494625378467;
phase3.initialTimeUpperBound     = 6;
phase3.initialStateLowerBound    = [-10   -10  -10 -10  -10   -10  .900 ]';
phase3.initialGuessState         = [ -1.2 0.5    0   0  -0.96  0   1.25]; 
phase3.initialStateUppperBound   = [10     10   10  10   10    10  1.400 ]';
% ===== Final state data
phase3.finalTimeLowerBound       = 2.1;
phase3.finalEpoch                = 3.93494625378467;
phase3.finalTimeUpperBound       = 8.0;
phase3.finalStateLowerBound      = [-10 -10 -10 -10 -10   -10  0.900]';
phase3.finalGuessState           = [ -1.2 0  0    0 -0.96  0   1.25];
phase3.finalStateUpperBound      = [ 10  10  10  10  10    10  1.400]';
% ===== bounds along the path but not at end points
phase3.stateLowerBound           = [-10 -10 -10 -10 -10 -10 .900 ]';
phase3.stateUpperBound           = [ 10  10  10  10  10  10 1.400]';
phase3.controlUpperBound         = [1 1 1]';
phase3.controlLowerBound         = [-1 -1 -1]';
phase3.algConstraintLowerBound   = [1 ]';
phase3.algConstraintUpperBound   = [1 ]';
phase3.eventConstraintLowerBound = [];%[1.5 0 0];%[1.5 0 .20]';
phase3.eventConstraintUpperBound = [];%[1.5 0 5];%[1.5 0 5]';

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================
traj.phaseList        = {phase1,phase2,phase3};
traj.linkageConfig{1} = {phase1,phase2,phase3};
[z,F,xmul,Fmul]       = traj.Optimize();