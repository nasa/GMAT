
%% ===== Initializations
global traj  % Required for SNOPT function wrapper.. Can't pass a Trajectory to SNOPT.

% Import CSALT classes
import exec.Trajectory
import exec.RadauPhase

%% =====  Define Properties for the Phase 

% Create a Radau phase and set preliminary mesh configuration
phase1 = RadauPhase;
phase1.initialGuessMode = 'LinearNoControl';
meshIntervalFractions = [-1 0 1];
meshIntervalNumPoints = [5 5];

%  Set time bounds and gueses
initialGuessTime  = 0;
finalGuessTime    = .3;
timeLowerBound    = 0;
timeUpperBound    = 100;

%  Set state bounds and guesses
numStateVars      = 3;
stateLowerBound   = [-10 -10 -10]';
initialGuessState = [0 0 0]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState   = [.4 -.5 -1]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [10 0 0]';

%  Set control bounds (guess is set to 1s)
numControlVars = 1;
controlUpperBound = 10;
controlLowerBound = -10;

% Call set methods to configure the data
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

%% =====  Define Properties for the Trajectory

%  Create trajectory and configure user function names
traj = Trajectory();

% Add the path, boundary function, and phases to the trajectory
import BrachistichronePathObject
import BrachistichronePointObject
traj.pathFunctionObject = BrachistichronePathObject();
traj.pointFunctionObject = BrachistichronePointObject();
traj.phaseList = {phase1};

% Set plot configurations and other settings
traj.plotFunctionName = 'BrachistichronePlotFunction';
traj.showPlot = false();
traj.plotUpdateRate = 100;
traj.costLowerBound = -Inf;
traj.costUpperBound = Inf;
traj.maxMeshRefinementCount = 2;
traj.maxMajorIterations = 25;

%% =====  Run The Optimizer and plot the solution
traj.Initialize();
[z,info] = traj.Optimize();

stateSol = traj.GetStateArray(1);
plot(stateSol(:,1),stateSol(:,2))
grid on


