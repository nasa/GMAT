%==========================================================================
%% =====  Initializations
%==========================================================================
%ClearAll
global igrid iGfun jGvar traj
import TimeVaryingConstraintPathObject
import TimeVaryingConstraintPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject = TimeVaryingConstraintPathObject();
traj.pointFunctionObject = TimeVaryingConstraintPointObject();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 3;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearUnityControl';
phase1.meshIntervalFractions   = [-1 0 1];
phase1.meshIntervalNumPoints   = [20 20];
%  Set time properties
phase1.initialGuessTime  = 0;
phase1.finalGuessTime    = 1;
phase1.timeLowerBound    = 0;
phase1.timeUpperBound    = 1;
%  Set state state properties
phase1.numStateVars      = 2;
phase1.stateLowerBound   = [-2 -2]';
phase1.initialGuessState = [1 1]';
phase1.finalGuessState   = [0 1]';
phase1.stateUpperBound   = [2 2]';
%  Set control properties
phase1.numControlVars    = 1;
phase1.controlUpperBound = [20]';
phase1.controlLowerBound = [-20]';

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();

for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.timeVector;
    figure(1);
    subplot(2,1,1)
    hold on; grid on;
    plot(times,stateSol(:,1),times,stateSol(:,2))
    for idx = 1:length(times)
       con(idx) = -stateSol(idx,2) + 8*(times(idx) - 0.5)^2 - 0.5;
    end
    plot(times,con,'r-')
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
