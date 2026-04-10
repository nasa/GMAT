%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import MoonLanderPathObject
import MoonLanderPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject = MoonLanderPathObject();
traj.pointFunctionObject = MoonLanderPointObject();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 3;

%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = -1:.25:1;
meshIntervalNumPoints   = 5*ones(length(meshIntervalFractions)-1,1);
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 1.5;
timeLowerBound    = 0;
timeUpperBound    = 2;
%  Set state state properties
numStateVars      = 3;
stateLowerBound   = [-20 -20 0.01]';
initialGuessState = [1 -0.783 1]';
finalGuessState   = [0 0 .3]';
stateUpperBound   = [20 20 1]';

%  Set control properties
numControlVars    = 1;
controlUpperBound = [1.227]';
controlLowerBound = [0]';

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
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
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

