
%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import ConwayOrbitExamplePathObject
import ConwayOrbitExamplePointObject
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = ConwayOrbitExamplePathObject();
traj.pointFunctionObject = ConwayOrbitExamplePointObject();
traj.plotFunctionName  = 'ConwayOrbitExamplePlotFunction';
traj.showPlot          = false();
traj.plotUpdateRate    = 200;
traj.costLowerBound    = -Inf;
traj.costUpperBound    = Inf;
traj.maxMeshRefinementCount = 25;
traj.ifScaling         = false;
traj.costScaling       = 1e-1;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

method = 1;
if method == 1;
    phase1 = RadauPhase();
    phase1.initialGuessMode = 'LinearUnityControl';
    meshIntervalFractions = [-1 -.5 0 .5 1];
    meshIntervalNumPoints = [6 6 6 6];
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.SetTranscription('RungeKutta8')
    phase1.initialGuessMode = 'LinearUnityControl';
    meshIntervalFractions   = [0 1];
    meshIntervalNumPoints   = [20];
end


%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 50;
timeLowerBound    = 0;
timeUpperBound    = 100;
%  Set state state properties
numStateVars      = 4;
stateLowerBound   = [-10 -6*(2*pi) -10 -10]';
initialGuessState = [1.1 0 0 1/sqrt(1.1)]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState   = [5  3*2*pi 1 1]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [10 6*(2*pi) 10 10]';
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

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();

stateVec = traj.phaseList{1}.DecVector.GetStateArray();
polar(stateVec(:,2),stateVec(:,1))
