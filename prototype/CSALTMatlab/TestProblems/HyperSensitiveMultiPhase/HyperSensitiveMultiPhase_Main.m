%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import HyperSensitiveMultiPhasePathObject
import HyperSensitiveMultiPhasePointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                           = Trajectory();
traj.pathFunctionObject         = HyperSensitiveMultiPhasePathObject();
traj.pointFunctionObject       = HyperSensitiveMultiPhasePointObject();
traj.plotFunctionName          = 'HyperSensitivePlotFunctionMultiPhase';
traj.showPlot                  = false();
traj.plotUpdateRate            = 2;
traj.costLowerBound            = 0;
traj.costUpperBound            = Inf;
traj.maxMeshRefinementCount    = 3;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set Time Properties
phase1                         = RadauPhase();
phase1.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [ -1 0 1];
meshIntervalNumPoints   = [15 5]';
%  Set time properties
timeLowerBound          = 0
initialGuessTime        = 0;
finalGuessTime          = 20;
timeUpperBound          = 30
%  Set state properties
numStateVars            = 1;
stateLowerBound         = -50;
initialGuessState       = .5;
finalGuessState         = .2;
stateUpperBound         = 50;
%  Set control properties
numControlVars          = 1;
controlUpperBound       = 50;
controlLowerBound       = -50;

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

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set Time Properties
phase2                         = RadauPhase();
phase2.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [ -1 0 1];
meshIntervalNumPoints   = [5 15]';
% Set time properties
timeLowerBound          = 30;
initialGuessTime        = 30;
finalGuessTime          = 50;
timeUpperBound          = 50;
% Set state properties
numStateVars            = 1;
stateLowerBound         = -50;
initialGuessState       = -.2;
finalGuessState         = 1;
stateUpperBound         = 50;
% Set control properties
numControlVars          = 1;
controlUpperBound       = 50;
controlLowerBound       = -50;

phase2.SetNumStateVars(numStateVars);
phase2.SetNumControlVars(numControlVars);
phase2.SetMeshIntervalFractions(meshIntervalFractions);
phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBound)
phase1.SetStateUpperBound(stateUpperBound)
phase1.SetStateInitialGuess(initialGuessState)
phase1.SetStateFinalGuess(finalGuessState)
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

traj.phaseList        = {phase1,phase2};


[z,F,xmul,Fmul]       = traj.Optimize();

for phaseIdx = 1:2
    
    timeVector = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    
    subplot(2,1,1)
    hold on
    plot(timeVector,stateSol,'k-')
    hold on;
    grid on
    title('State History vs. Time')
    axis([0 50 -.2 1])
    
    subplot(2,1,2)
    hold on
    plot(timeVector(1:end-1),controlSol)
    axis([0 50 -1 3])
    grid on
    title('Control History vs. Time')
end