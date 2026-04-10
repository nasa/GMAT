%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import BreakwellPathObject
import BreakwellPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = BreakwellPathObject();
traj.pointFunctionObject  = BreakwellPointObject();
% traj.plotFunctionName   = 'LinearTangentSteeringPlotFunction';
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 1;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [-1  1];
meshIntervalNumPoints   = [7];
% phase1                         = HermiteSimpsonPhase;
% phase1.initialGuessMode         = 'LinearUnityControl';
% phase1.meshIntervalFractions   = [0  1];
% phase1.meshIntervalNumPoints   = [5];
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = .3;
timeLowerBound    = 0;
timeUpperBound    = .5;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-2 -2]';
initialGuessState = [0 1]';
finalGuessState   = [.1 0]';
stateUpperBound   = [2 2]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [10]';
controlLowerBound = [-10]';

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

phase2                         = RadauPhase;
phase2.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [-1  1];
meshIntervalNumPoints   = [7];
% phase2                         = HermiteSimpsonPhase;
% phase2.initialGuessMode         = 'LinearUnityControl';
% phase2.meshIntervalFractions   = [0  1];
% phase2.meshIntervalNumPoints   = [5];
%  Set time properties
initialGuessTime  = 0.3;
finalGuessTime    = 0.6;
timeLowerBound    = 0.25;
timeUpperBound    = 0.7;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-2 -2]';
initialGuessState = [.1 0]';
finalGuessState   = [.1 0]';
stateUpperBound   = [2 0]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [10]';
controlLowerBound = [-10]';

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

phase3                         = RadauPhase;
phase3.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [-1  1];
meshIntervalNumPoints   = [7];
% phase3                       = HermiteSimpsonPhase;
% phase3.initialGuessMode      = 'LinearUnityControl';
% phase3.meshIntervalFractions = [0  1];
% phase3.meshIntervalNumPoints = [5];
%  Set time properties
initialGuessTime  = .6;
finalGuessTime    = 1;
timeLowerBound    = .5;
timeUpperBound    = 1;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-2 -2]';
initialGuessState = [.1 0]';
finalGuessState   = [0 -1]';
stateUpperBound   = [2 2]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [10]';
controlLowerBound = [-10]';

phase3.SetNumStateVars(numStateVars);
phase3.SetNumControlVars(numControlVars);
phase3.SetMeshIntervalFractions(meshIntervalFractions);
phase3.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase3.SetStateLowerBound(stateLowerBound)
phase3.SetStateUpperBound(stateUpperBound)
phase3.SetStateInitialGuess(initialGuessState)
phase3.SetStateFinalGuess(finalGuessState)
phase3.SetTimeLowerBound(timeLowerBound)
phase3.SetTimeUpperBound(timeUpperBound)
phase3.SetTimeInitialGuess(initialGuessTime)
phase3.SetTimeFinalGuess(finalGuessTime)
phase3.SetControlLowerBound(controlLowerBound)
phase3.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1,phase2,phase3};
[z,F,xmul,Fmul] = traj.Optimize();

for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    figure(1);
    hold on;
    plot(times,stateSol(:,1),times,stateSol(:,2))
    figure(2)
    hold on;
    if isa(phase1,'RadauPhase')
        plot(times(1:end-1),controlSol(:,1))
    else
        plot(times,controlSol(:,1))
    end
end
