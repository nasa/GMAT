%==========================================================================
%% =====  Initializations
%==========================================================================
%ClearAll
global igrid iGfun jGvar traj
import SchwartzPathObject
import SchwartzPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj = Trajectory();
traj.pathFunctionObject = SchwartzPathObject();
traj.pointFunctionObject = SchwartzPointObject();
traj.showPlot = false();
traj.plotUpdateRate = 2;
traj.costLowerBound = -Inf;
traj.costUpperBound = Inf;
traj.maxMeshRefinementCount = 25;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1 = RadauPhase;
phase1.initialGuessMode = 'LinearNoControl';
meshIntervalFractions = [-1 1];
meshIntervalNumPoints = 10;
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 1;
timeLowerBound    = 0;
timeUpperBound    = 1;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-2 -2]';
initialGuessState = [1 1]';
finalGuessState   = [1 1]';
stateUpperBound   = [2 2]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [0.5]';
controlLowerBound = [-0.5]';

phase1.SetNumStateVars(numStateVars);
phase1.SetNumControlVars(numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions);
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBound);
phase1.SetStateUpperBound(stateUpperBound);
phase1.SetStateInitialGuess(initialGuessState);
phase1.SetStateFinalGuess(finalGuessState);
phase1.SetTimeLowerBound(timeLowerBound)
phase1.SetTimeUpperBound(timeUpperBound)
phase1.SetTimeInitialGuess(initialGuessTime)
phase1.SetTimeFinalGuess(finalGuessTime)
phase1.SetControlLowerBound(controlLowerBound)
phase1.SetControlUpperBound(controlUpperBound)

phase2 = RadauPhase;
phase2.initialGuessMode  = 'LinearNoControl';
meshIntervalNumPoints = [10];

%  Set time properties
initialGuessTime  = 1;
finalGuessTime    = 2.9;
timeLowerBound    = 1;
timeUpperBound    = 3;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-2 -2]';
initialGuessState = [1 1]';
finalGuessState   = [1 1]';
stateUpperBound   = [2 2]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [5]';
controlLowerBound = [-5]';

phase2.SetNumStateVars(numStateVars);
phase2.SetNumControlVars(numControlVars);
phase2.SetMeshIntervalFractions(meshIntervalFractions);
phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase2.SetStateLowerBound(stateLowerBound);
phase2.SetStateUpperBound(stateUpperBound);
phase2.SetStateInitialGuess(initialGuessState);
phase2.SetStateFinalGuess(finalGuessState);
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
[z,F,xmul,Fmul] = traj.Optimize();


for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    figure(1);
    subplot(2,1,1)
    hold on; grid on;
    plot(times,stateSol(:,1),times,stateSol(:,2))
    title('state history')
    subplot(2,1,2)
    hold on; grid on;
    if isa(phase1,'RadauPhase')
        plot(times(1:end-1),controlSol(:,1))
    else
        plot(times,controlSol(:,1))
    end
    title('control history')
end

