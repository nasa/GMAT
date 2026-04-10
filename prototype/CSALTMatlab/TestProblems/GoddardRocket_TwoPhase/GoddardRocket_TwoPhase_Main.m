%==========================================================================
%% =====  Initializations
%==========================================================================
close all
clear all
global igrid iGfun jGvar traj 
import GoddardRocket_TwoPhasePathObject
import GoddardRocket_TwoPhasePointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                        = Trajectory();
traj.pathFunctionObject     = GoddardRocket_TwoPhasePathObject();
traj.pointFunctionObject    = GoddardRocket_TwoPhasePointObject();
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
meshIntervalNumPoints   = [5 5 5 5 5 5 5 5];
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = .15;
timeLowerBound    = 0;
timeUpperBound    = 1;
%  Set state state properties
numStateVars      = 3;
%                    h     v   m
stateLowerBound   = [1     0  .6]';
initialGuessState = [1     0   1]'; 
finalGuessState   = [1.1   0  .6]'; 
stateUpperBound   = [1e5  1e5  1]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [3.5]';
controlLowerBound = [3.0]';

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
%% =====  Define Properties for Phase 1
%==========================================================================

%  Set mesh properities
phase2                         = RadauPhase;
%phase1.initialGuessMode         = 'UserGuessFunction';
phase2.initialGuessMode         = 'LinearNoControl';
meshIntervalFractions   = [-1 -.75 -.5 -.25 0 .25 .5 .75 1];
meshIntervalNumPoints   = [5 5 5 5 5 5 5 5];
%  Set time properties
initialGuessTime  = .15;
finalGuessTime    = .25;
timeLowerBound    = 0;
timeUpperBound    = 1;
%  Set state state properties
numStateVars      = 3;
%                    h     v   m
stateLowerBound   = [1     0  .6]';
initialGuessState = [1.1  .2  .6]'; 
finalGuessState   = [1.2   0  .6]'; 
stateUpperBound   = [1e5  1e5  1]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [0]';
controlLowerBound = [0]';

phase2.SetNumStateVars(numStateVars)
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
[z,F,xmul,Fmul] = traj.Optimize();

for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
	for i = 1:3
	    figure(i);
	    hold on;
	    plot(times,stateSol(:,i),'x-')
		grid on
	end
    figure(4)
    hold on;
    if isa(phase1,'RadauPhase')
        plot(times(1:end-1),controlSol(:,1),'x-')
    else
        plot(times,controlSol(:,1),'x-')
    end
	grid on
end
disp('Done!')