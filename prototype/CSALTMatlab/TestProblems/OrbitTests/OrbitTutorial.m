%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj scaleUtility
import OrbitTutorialPathFunc
import OrbitTutorialPointFunc
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();

%traj.plotFunctionName   = 'BrachistichronePlotFunction';
%traj.guessFunctionName  = 'BrachistichroneGuessFunction';
traj.showPlot           = false();
traj.plotUpdateRate     = 100;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 2;
traj.ifScaling          = false();
traj.costScaling       = 1e-1;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Configure Scaling
gravParam = 132712440017.99;
DU = 1.49597870e8;
TU = 2*pi*sqrt(DU^3/gravParam);
VU = DU/TU;
MU = 4000;
RefEpoch = 30542.0;

%  Set mesh properities
scaleUtility = ScaleUtil();
scaleUtility.DU = DU;
scaleUtility.TU = TU;
scaleUtility.VU = VU;
scaleUtility.MU = MU;
scaleUtility.MDOTU = MU/TU;
scaleUtility.ACCU = VU/TU;
scaleUtility.RefEpoch = RefEpoch;

pointFunc = OrbitTutorialPointFunc();
pathFunc = OrbitTutorialPathFunc();
pathFunc.ScaleUtility = scaleUtility;
pointFunc.ScaleUtility = scaleUtility;
pathFunc.gravParam = gravParam;

traj.pathFunctionObject  = pathFunc;
traj.pointFunctionObject = pointFunc;
traj.guessFunctionName = 'GuessWithUnityControl';

method = 1;
if method == 1;
    phase1 = RadauPhase;
    phase1.initialGuessMode = 'UserGuessFunction';%'UserGuessFunction';
    meshIntervalFractions = [-1 -0.5 0 0.5 1];
    meshIntervalNumPoints = [3 3 3 3];
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.SetTranscription('RungeKutta8');
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [2];
end


%  Set time properties
initialGuessTimeUnScaled  = 30542.0;
finalGuessTimeUnScaled    = 30542.0 + 250;
timeLowerBoundUnScaled    = 30537.58129245887;
timeUpperBoundUnScaled    = 32406.49962790855;

%  Set state state properties
numStateVars      = 7;
stateLowerBoundUnScaled   = [ -1.49598e+09 -1.49598e+09 -1.49598e+09 ...
                              -100 -100 -100 1]';
initialGuessStateUnScaled = [125291184.0 -75613036.0 -32788527.0 ...
                                 13.438 25.234 10.903 4000]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessStateUnScaled   = [125291184.0 -75613036.0 -32788527.0 ...
                                 13.438 25.234 10.903 4000]'; % [rf thetaf vrf vthetaf mf]
stateUpperBoundUnScaled   = [ 1.49598e+09  1.49598e+09 1.49598e+09 ...
                              100 100 100 5000]';
%  Set control properties
numControlVars    = 3;
controlUpperBound = [0 0 0]';
controlLowerBound = [0 0 0]';

% ===== Scale input data
stateLowerBoundScaled = scaleUtility.ScaleState(stateLowerBoundUnScaled)
stateUpperBoundScaled = scaleUtility.ScaleState(stateUpperBoundUnScaled)
timeLowerBoundScaled = scaleUtility.ScaleTime(timeLowerBoundUnScaled)
timeUpperBoundScaled = scaleUtility.ScaleTime(timeUpperBoundUnScaled)
initialGuessStateScaled = scaleUtility.ScaleState(initialGuessStateUnScaled)
finalGuessStateScaled = scaleUtility.ScaleState(finalGuessStateUnScaled)
initialGuessTimeScaled = scaleUtility.ScaleTime(initialGuessTimeUnScaled)
finalGuessTimeScaled = scaleUtility.ScaleTime(finalGuessTimeUnScaled)

phase1.SetNumStateVars(numStateVars)
phase1.SetNumControlVars(numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions);
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBoundScaled)
phase1.SetStateUpperBound(stateUpperBoundScaled)
phase1.SetStateInitialGuess(initialGuessStateScaled)
phase1.SetStateFinalGuess(finalGuessStateScaled)
phase1.SetTimeLowerBound(timeLowerBoundScaled)
phase1.SetTimeUpperBound(timeUpperBoundScaled)
phase1.SetTimeInitialGuess(initialGuessTimeScaled)
phase1.SetTimeFinalGuess(finalGuessTimeScaled)
phase1.SetControlLowerBound(controlLowerBound)
phase1.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1};
traj.Initialize();

% DenseHessianTest

[z,info] = traj.Optimize();

stateSolNonDim = traj.phaseList{1}.DecVector.GetStateArray();
plot3(stateSolNonDim(:,1),stateSolNonDim(:,2),stateSolNonDim(:,3));
axis equal
axis([-2 2 -2 2 -2 2])
%
grid on

numRows = size(stateSolNonDim,1);

for rowIdx = 1:numRows
   stateSolDim(rowIdx,:) = scaleUtility.UnScaleState(stateSolNonDim(rowIdx,:)')';
end




