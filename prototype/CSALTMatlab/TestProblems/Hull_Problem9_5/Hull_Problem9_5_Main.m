%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import Hull_Problem9_5PathObject
import Hull_Problem9_5PointObject
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase
%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = Hull_Problem9_5PathObject();
traj.pointFunctionObject = Hull_Problem9_5PointObject();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 6;
traj.ifScaling          = false();
traj.costScaling       = 1e-1;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
method = 1;
if method == 1;
    phase1 = RadauPhase;
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [-1 -.5 0 0.5 1];
    meshIntervalNumPoints = [5 5 5 5];
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.SetTranscription('RungeKutta8');
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [2];
end

%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 1;
timeLowerBound    = 0;
timeUpperBound    = 1;
%  Set state state properties
numStateVars      = 1;
stateLowerBound   = [-10]';
initialGuessState = [0]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState   = [1]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [10]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [10]';
controlLowerBound = [-10]';

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
traj.Initialize();
C = traj.GetCostConstraintFunctions();
J = traj.GetJacobian();

[z,F,xmul,Fmul] = traj.Optimize();

timesSol = traj.phaseList{1}.TransUtil.GetTimeVector();
stateSol = traj.phaseList{1}.DecVector.GetStateArray();
controlSol = traj.phaseList{1}.DecVector.GetControlArray();

for timeIdx = 1:length(timesSol)
    t = timesSol(timeIdx);
    uExact(timeIdx,1) = -t/2 + 0.5;
    xExact(timeIdx,1) = -t^2/4 + t/2;
end

xExact - stateSol
uExact(1:end-1) - controlSol

plot(timesSol,stateSol,'k-');
hold on
plot(timesSol,xExact,'k*')
grid on


