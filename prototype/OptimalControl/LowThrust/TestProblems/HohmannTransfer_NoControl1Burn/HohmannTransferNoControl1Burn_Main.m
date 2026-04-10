%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global iGfun jGvar traj igrid

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Set cost function properties
traj                    = Trajectory();
traj.pathFunctionName   = 'HohmannTransferNoControlPathFunction1Burn';
traj.pointFunctionName  = 'HohmannTransferNoControlPointFunction1Burn';
traj.plotFunctionName   = 'HohmannTransferNoControlPlotFunction1Burn';
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
phase1.meshIntervalNumPoints     = [30]';
phase1.numStates                 = 7;
phase1.numControls               = 3;
phase1.initialGuessMode          = 'LinearUnityControl';
% ===== Initial state data
phase1.initialTimeLowerBound     = 0;
phase1.initialEpoch              = 0;
phase1.initialTimeUpperBound     = 0;
phase1.initialGuessState         = [ 1.2 0 0 0  0.96 0 1.25];
phase1.initialStateLowerBound    = [          1.08182071932204
                         0
                         0
                         0
         0.961440367214808
                         0
                      1.25];
phase1.initialStateUppperBound   = [          1.08182071932204
                         0
                         0
                         0
         0.961440367214808
                         0
                      1.25];
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
phase2.meshIntervalNumPoints     = [30]';
phase2.numStates                 = 7;
phase2.numControls               = 0;
% ===== Initial state data
phase2.initialGuessMode          = 'LinearUnityControl';
phase2.initialTimeLowerBound     = 0;
phase2.initialEpoch              = 0.53494625378467;
phase2.initialTimeUpperBound     = 1;
phase2.initialStateLowerBound    = [-4  -4 -4   -4 -4 -4  .300 ]';
phase2.initialGuessState         = [ 1.2 1.2 0 -0.96 0  0  1.25];
phase2.initialStateUppperBound   = [ 4   4  4    4  4  4  1.900 ]';
% ===== Final state data
phase2.finalTimeLowerBound       = 0;
phase2.finalEpoch                = 3.53494625378467;
phase2.finalTimeUpperBound       = 4;
phase2.finalGuessState           = [ -1.2 1.2 0 -0.96 0 0 1.25];
phase2.finalStateLowerBound      = [-10 -10 -10 -10 -10 -10 .900 ]';
phase2.finalStateUpperBound      = [ 10  10  10  10  10  10 1.400]';

% ===== bounds along the path but not at end points
phase2.stateLowerBound           = [-10 -10 -10 -10 -10 -10 .900 ]';
phase2.stateUpperBound           = [ 10  10  10  10  10  10 1.400]';
phase2.controlUpperBound         = []';
phase2.controlLowerBound         = []';
phase2.algConstraintLowerBound   = []';
phase2.algConstraintUpperBound   = []';
phase2.eventConstraintLowerBound = [1.11317784162123 0 0];%[1.5 0 .20]';
phase2.eventConstraintUpperBound = [1.11317784162123 0 3];%[1.5 0 5]';



%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================
traj.phaseList        = {phase1,phase2};
traj.linkageConfig{1} = {phase1,phase2};
[z,F,xmul,Fmul]       = traj.Optimize();