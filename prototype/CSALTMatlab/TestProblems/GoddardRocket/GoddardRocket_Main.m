%==========================================================================
%% =====  Initializations
%==========================================================================
close all
clear all
global igrid iGfun jGvar traj 
import GoddardRocketPathObject
import GoddardRocketPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                        = Trajectory();
traj.pathFunctionObject     = GoddardRocketPathObject();
traj.pointFunctionObject    = GoddardRocketPointObject();
traj.showPlot               = false();
traj.plotUpdateRate         = 2;
traj.costLowerBound         = -Inf;
traj.costUpperBound         = Inf;
traj.maxMeshRefinementCount = 4;

%unitsType = 'gpops';
unitsType = 'psopt';

traj.pathFunctionObject.SetUnitsType(unitsType);
traj.pathFunctionObject.InitializeConstants();
traj.pointFunctionObject.SetUnitsType(unitsType);

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
switch unitsType
	case 'psopt'
		timeLowerBound    = 0;
		initialGuessTime  = 0;
		finalGuessTime    = .35;
		timeUpperBound    = 1;
	case 'gpops'
		timeLowerBound    = 0;
		initialGuessTime  = 0;
		finalGuessTime    = 45;
		timeUpperBound    = 100;
end

%  Set state state properties
numStateVars      = 3;
%                            h         v    m
switch unitsType
	case 'psopt'
		stateLowerBound   = [1         0   .6]';
		initialGuessState = [1         0    1]'; 
		finalGuessState   = [1.3       0   .6]'; 
		stateUpperBound   = [1e5     1e5    1]';
	case 'gpops'
		stateLowerBound   = [0         0    1]';
		initialGuessState = [0         0    3]'; 
		finalGuessState   = [19000     0    1]'; 
		stateUpperBound   = [1e10   1e10    3]';
end

%  Set control properties
numControlVars    = 1;
switch unitsType
	case 'psopt'
		controlUpperBound = [3.5]';
		controlLowerBound = [0]';
	case 'gpops'
		controlUpperBound = [193]';
		controlLowerBound = [0]';
end

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