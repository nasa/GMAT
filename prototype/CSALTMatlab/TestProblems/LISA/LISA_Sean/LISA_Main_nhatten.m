% prelims
clear all
close all
clc
format long g

cd('C:/GMAT/CSALT_OutOfSourceBuild_Repo/gmat-git/prototype/CSALTMatlab/TestProblems/LISA/LISA_Sean')

% add csalt and snopt to matlab path
snoptPath = 'C:/GMAT/Tutorial/CSALTMATLAB/ThirdParty/snopt';
csaltPath = 'C:/GMAT/CSALT_OutOfSourceBuild_Repo/gmat-git/prototype/CSALTMatlab';
addpath(genpath(snoptPath));
addpath(genpath(csaltPath));

global traj scaleUtil % Required for SNOPT function wrapper.. Can't pass a Trajectory to SNOPT.

% Import CSALT classes
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Non-Dimensionalization Properties
%==========================================================================

% Define quanitities used in non-dimensionalization and re-dimensionalization
% Assumes Earth is central body
gravParam = 398600.4415;
RE = 6378.1363;
config.gravParam = gravParam;
config.DU  = 1e6;
config.TU  = sqrt(config.DU^3/config.gravParam);
config.VU = config.DU/config.TU;
config.ACCU = config.DU/config.DU^2;
config.MU  = 10000;
config.StateScaleMat = diag([1/config.DU; 1/config.DU; 1/config.DU; 1/config.VU; 1/config.VU; 1/config.VU]);
config.DynScaleMat = config.TU*config.StateScaleMat;
config.StateUnscaleMat = inv(config.StateScaleMat);
config.DynUnscaleMat = inv(config.DynScaleMat);
config.refTimeMJD = 2464114.99962963 - 2430000;

%% =====  Define Properties for the Trajectory

%  Create trajectory and configure user function names
traj = Trajectory();

% Add the path, boundary function, and phases to the trajectory
import LISAPathObject_nhatten
import LISAPointObject_nhatten
import LISAPointObject_RmagConstraint_nhatten
traj.pathFunctionObject = LISAPathObject_nhatten();
%traj.pointFunctionObject = LISAPointObject_nhatten();
traj.pointFunctionObject = LISAPointObject_RmagConstraint_nhatten();

% Set plot configurations and other settings
%traj.plotFunctionName = 'BrachistichronePlotFunction';
traj.plotFunctionName = '';
traj.showPlot = false();
traj.plotUpdateRate = 100;
traj.costLowerBound = -Inf;
traj.costUpperBound = Inf;
traj.maxMeshRefinementCount = 10;
traj.refineInitialMesh = false;
traj.maxMajorIterations = 50;
traj.maxTotalIterations = 100000;
traj.optimalityTol = 1e-4;
traj.feasibilityTol = 1e-5;

%% ===== Define scale utilities and set constraints
scaleUtil = ScaleUtility();
scaleUtil.earthScaling = config;

% Apply scale factors in point and patch function objects
traj.pointFunctionObject.SetScaleUtil(scaleUtil);
traj.pathFunctionObject.SetScaleUtil(scaleUtil);

% Use guess function and get initial states
initTime = 0; % scaled
[initialStateConstraint,~] = LISAGuessFunction_nhatten(initTime,1,2); % last 2 args aren't used, don't matter

% Set constraint on initial time
traj.pointFunctionObject.SetInitTime(initTime); % the reference date

% Set constraint on final time
traj.pointFunctionObject.SetFinalTime(365.25*86400/config.TU); % one year from the reference date

% Set constraint on initial state
traj.pointFunctionObject.SetInitState(initialStateConstraint');

%% =====  Define Properties for the Phase 

% Create a Radau phase and set preliminary mesh configuration
phase1 = RadauPhase;
phase1.initialGuessMode = 'UserGuessFunction';
%phase1.initialGuessMode = 'LinearNoControl';
meshIntervalFractions = [-1 0 1];
meshIntervalNumPoints = 4*(ones(length(meshIntervalFractions)-1,1));

%  Set time bounds and guesses: unscaled at first, then scale them
initialGuessTime  = 0; % relative to epoch (sec)
finalGuessTime    = 380 * 86400; % relative to epoch (sec)
timeLowerBound    = -86400; % relative to epoch (sec)
timeUpperBound    = 400 * 86400; % relative to epoch (sec)

% scale the time bounds and guesses
initialGuessTime = initialGuessTime / config.TU;
finalGuessTime = finalGuessTime / config.TU;
timeLowerBound = timeLowerBound / config.TU;
timeUpperBound = timeUpperBound / config.TU;

%  Set state bounds and guesses
numStateVars      = 18; % 3 spacecraft, each with 6 states
stateLowerBoundPerSpacecraft = config.StateScaleMat * [-10000000000 -10000000000 -10000000000 -1000 -1000 -1000]'; % scaled
stateUpperBoundPerSpacecraft = config.StateScaleMat * [10000000000 10000000000 10000000000 1000 1000 1000]'; % scaled
stateLowerBound   = [stateLowerBoundPerSpacecraft; stateLowerBoundPerSpacecraft; stateLowerBoundPerSpacecraft]; % scaled
stateUpperBound   = [stateUpperBoundPerSpacecraft; stateUpperBoundPerSpacecraft; stateUpperBoundPerSpacecraft]; % scaled

% shouldn't need state guesses if using a UserGuessFunction, right?
% well, apparently you do because it errors if you don't at least set these
initialGuessState = initialStateConstraint';
finalGuessState   = initialStateConstraint';


%  Set control bounds (guess is set to 1s)
numControlVars = 1;
controlUpperBound = 0;
controlLowerBound = 0;

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
phase1.SetRefineInitialMesh(false);

%% ===== Add phase to trajectory
traj.phaseList = {phase1};
traj.guessFunctionName = 'LISAGuessFunction_nhatten';

%% =====  Run The Optimizer and plot the solution
traj.Initialize();
[z,info] = traj.Optimize();

stateSol = traj.GetStateArray(1);
timeSol = traj.GetTimeArray(1);

figure(1)
plot(stateSol(:,1), stateSol(:,2), 'b')
hold on
plot(stateSol(:,7), stateSol(:,8), 'r')
hold on
plot(stateSol(:,13), stateSol(:,14), 'g')
hold off

% figure(2)
% n=length(timeSol);
% plot(stateSol(1:n,1), stateSol(1:n,2), 'b')
% hold on
% plot(stateSol(1:n,7), stateSol(1:n,8), 'r')
% hold on
% plot(stateSol(1:n,13), stateSol(1:n,14), 'g')
% hold off