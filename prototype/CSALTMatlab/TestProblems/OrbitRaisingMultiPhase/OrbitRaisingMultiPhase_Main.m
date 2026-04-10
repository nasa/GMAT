%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj 
import OrbitRaisingMultiPhasePathObject
import OrbitRaisingMultiPhasePointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                    = Trajectory();
traj.pathFunctionObject  = OrbitRaisingMultiPhasePathObject();
traj.pointFunctionObject  = OrbitRaisingMultiPhasePointObject();
traj.plotFunctionName   = 'OrbitRaisingMultiPhasePlotFunction';
traj.showPlot           = false();
traj.plotUpdateRate     = 3;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 0;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================
phase1                           = RadauPhase();
phase1.initialGuessMode          = 'LinearUnityControl';
meshIntervalFractions     = [-1; 1];
%phase1.meshIntervalNumPoints     = 50*ones(1,1);
meshIntervalNumPoints     = 7*ones(1,1);
%  Set time properties
timeLowerBound   = 0;
initialGuessTime = 0;
finalGuessTime   = 1;
timeUpperBound   = 3.32
%  Set state properties
numStateVars              = 5;
stateLowerBound           = [0.5 0 -10 -10 0.1]';
initialGuessState         = [1 0 0 1 1]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState           = [1 .5 0 1 1]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound           = [5 4*pi 10 10 3]';
%  Set control properties
numControlVars            = 2;
controlUpperBound         = [10 10]';
controlLowerBound         = [-10 -10]';

phase1.SetNumStateVars(numStateVars);
phase1.SetNumControlVars(numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions);
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBound)
phase1.SetStateUpperBound(stateUpperBound)
phase1.SetStateInitialGuess(initialGuessState)
phase1.SetStateFinalGuess(finalGuessState)
phase1.SetTimeLowerBound(timeLowerBound)
phase1.SetTimeUpperBound(timeUpperBound)
phase1.SetTimeInitialGuess(initialGuessTime)
phase1.SetTimeFinalGuess(finalGuessTime)
phase1.SetControlLowerBound(controlLowerBound)
phase1.SetControlUpperBound(controlUpperBound)

phase2                           = RadauPhase();
phase2.initialGuessMode          = 'LinearUnityControl';
meshIntervalFractions     = [-1; 1];
%phase2.meshIntervalNumPoints     = 50*ones(1,1);
meshIntervalNumPoints     = 7*ones(1,1);
%  Set time properties
timeLowerBound   = 0;
initialGuessTime = 0;
finalGuessTime   = 1;
timeUpperBound   = 3.32
%  Set state properties
numStateVars              = 5;
stateLowerBound           = [0.5 0 -10 -10 0.1]';
initialGuessState         = [1 .5 0 1 1]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState           = [1 pi 0 1 1]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound           = [5 4*pi 10 10 3]';
%  Set control properties
numControlVars            = 2;
controlUpperBound         = [10 10]';
controlLowerBound         = [-10 -10]';

phase2.SetNumStateVars(numStateVars);
phase2.SetNumControlVars(numControlVars);
phase2.SetMeshIntervalFractions(meshIntervalFractions);
phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase2.SetStateLowerBound(stateLowerBound)
phase2.SetStateUpperBound(stateUpperBound)
phase2.SetStateInitialGuess(initialGuessState)
phase2.SetStateFinalGuess(finalGuessState)
phase2.SetTimeLowerBound(timeLowerBound)
phase2.SetTimeUpperBound(timeUpperBound)
phase2.SetTimeInitialGuess(initialGuessTime)
phase2.SetTimeFinalGuess(finalGuessTime)
phase2.SetControlLowerBound(controlLowerBound)
phase2.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1,phase2};
%traj.linkageConfig{1} = {phase1,phase2};
[z,F,xmul,Fmul] = traj.Optimize();