%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import LinearTangentSteeringPathObj
import LinearTangentSteeringPointObj
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pointFunctionObject = LinearTangentSteeringPointObj();
traj.pathFunctionObject  = LinearTangentSteeringPathObj();
% traj.plotFunctionName   = 'LinearTangentSteeringPlotFunction';
traj.showPlot           = true();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 10;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
method = 1;
if method == 1;
    phase1 = RadauPhase;
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [-1; 1];
    meshIntervalNumPoints = 10;
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.SetTranscription('HermiteSimpson');
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [5];
end

initialGuessTime  = 0;
finalGuessTime    = 1;
timeLowerBound    = 0;
timeUpperBound    = 3;
%  Set state state properties
numStateVars      = 4;
stateLowerBound   = [-10 -10 -10 -10]';
initialGuessState = [0 0 0 0]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState   = [12 45 5 0]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [100 100 100 100 ]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [2]';
controlLowerBound = [-2]';

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

stateSol = traj.phaseList{1}.DecVector.GetStateArray();
times = traj.phaseList{1}.TransUtil.GetTimeVector();
plot(times,stateSol(:,1),times,stateSol(:,2),times,stateSol(:,3),times,stateSol(:,4))
figure(2)
controlSol = traj.phaseList{1}.DecVector.GetControlArray();
numControls = length(controlSol);
plot(times(1:numControls),controlSol)

