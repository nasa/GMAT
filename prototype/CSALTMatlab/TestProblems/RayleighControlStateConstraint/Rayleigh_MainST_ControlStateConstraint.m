%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import RayleighPathObject
import RayleighPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = RayleighPathObjectST_ControlStateConstraint();
traj.pointFunctionObject = RayleighPointObjectST_ControlStateConstraint();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 7;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearNoControl';
meshIntervalFractions   = -1:.25:1;
meshIntervalNumPoints   = 3*ones(length(meshIntervalFractions)-1,1);
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 4.5;
timeLowerBound    = 0;
timeUpperBound    = 4.5;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-10 -10]';
initialGuessState = [-5 -5]';
finalGuessState   = [0 0]';
stateUpperBound   = [10 10]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [10]';
controlLowerBound = [-10]';

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

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();

funcs = traj.GetCostConstraintFunctions();
gmatSolution = funcs(1);
sosSolution = 44.8044433;
diff = (gmatSolution - sosSolution)/sosSolution;
if diff > 1e-7
    error('Error in Raleigh with Control path constraint')
end

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
