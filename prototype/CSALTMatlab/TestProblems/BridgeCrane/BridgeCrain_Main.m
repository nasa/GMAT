%==========================================================================
%% =====  Initializations
%==========================================================================
close all
clear all
global igrid iGfun jGvar traj 
import BridgeCranePathObject
import BridgeCranePointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                        = Trajectory();
traj.pathFunctionObject     = BridgeCranePathObject();
traj.pointFunctionObject    = BridgeCranePointObject();
traj.showPlot               = false();
traj.plotUpdateRate         = 2;
traj.costLowerBound         = -Inf;
traj.costUpperBound         = Inf;
traj.maxMeshRefinementCount = 4;

%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
%phase1.initialGuessMode         = 'UserGuessFunction';
phase1.initialGuessMode         = 'LinearUnityControl';
meshIntervalFractions   = [-1 -.75 -.5 -.25 0 .25 .5 .75 1];
meshIntervalNumPoints   = [3 3 3 3 3 3 3 3];
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 8;
timeLowerBound    = 0.1;
timeUpperBound    = 12;
%  Set state state properties
numStateVars      = 4;
%                    x
stateLowerBound   = [-20 -20 -20 -20]';
initialGuessState = [0  0  0  0]'; 
finalGuessState   = [15  0  0  0]'; 
stateUpperBound   = [20 20 20 20]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [1]';
controlLowerBound = [-1]';

phase1.SetNumStateVars(numStateVars)
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
    hold on;
    plot(times,stateSol(:,1),'x-')
	grid on
    figure(2)
    hold on;
    if isa(phase1,'RadauPhase')
        plot(times(1:end-1),controlSol(:,1),'x-')
    else
        plot(times,controlSol(:,1),'x-')
    end
	grid on
end
disp('Done!')