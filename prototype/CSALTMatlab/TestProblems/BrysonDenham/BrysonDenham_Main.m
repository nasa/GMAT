%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import BrysonDenhamPathObj
import BrysonDenhamPointObj
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = BrysonDenhamPathObj();
traj.pointFunctionObject = BrysonDenhamPointObj();
% traj.plotFunctionName   = 'LinearTangentSteeringPlotFunction';
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 15;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [-1  1];
meshIntervalNumPoints   = 15;

%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 2;
timeLowerBound    = 0;
timeUpperBound    = 2;
%  Set state state properties
numStateVars      = 3;
stateLowerBound   = [0 -10 -10]';
initialGuessState = [0 0 0 ]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState   = [0 -1 4]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [1/9 10 10 ]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = 10';
controlLowerBound = -10';

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
controlSol = traj.phaseList{1}.DecVector.GetControlArray();
times = traj.phaseList{1}.TransUtil.GetTimeVector();
figure(1)
plot(times,stateSol(:,1),times,stateSol(:,2),times,stateSol(:,3))
figure(2)
if isa(phase1,'RadauPhase')
    plot(times(1:end-1),controlSol(:,1))
else
    plot(times,controlSol(:,1))
end
