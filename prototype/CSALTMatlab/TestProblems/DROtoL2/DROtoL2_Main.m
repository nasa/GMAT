% July 2016. Nathan Parrish. Original version
% 7/18/19    S. Hughes.  Modified for tutorial.  Clean up code. 

%% =====  Initializations and imports
clear all
close all
clc
global traj 
import exec.Trajectory
import exec.RadauPhase
import DROtoL2PathObject
import DROtoL2PointObject

%% =====  Constants
mscale = 1000.0;
PERIOD_DRO = 1.27688184304004; %TU
PERIOD_L2Halo = 3.22481529232264; %TU
MU = 0.012150585609624037;
TU = 375699.81732246041;
DU = 384747.96285603708;
day = 86400;
m0 = 1500 / mscale; % initial spacecraft mass (kg/mscale)
mustar = 0.012150585609624037; %"mass ratio"
thrust_limit = 1.0; %N

global constants
constants.MU = MU;
constants.TU = TU;
constants.DU = DU;
constants.Isp = 3000;
constants.g0 = 9.80665;
constants.mscale = mscale;
constants.PERIOD_DRO = PERIOD_DRO;
constants.PERIOD_L2Halo = PERIOD_L2Halo;
constants.day = day;
constants.m0 = m0;
constants.thrust_limit = thrust_limit;
constants.min_dist_moon = 0.01; %DU
constants.min_dist_earth = 0.01; %DU
constants.mustar = mustar;

global Nphase phase_type
Nphase = 1; % number of phases
phase_type = 2; %thrust optional

%% =====  Define Properties for Phase 1

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode         = 'UserGuessFunction';
meshIntervalFractions   = linspace(-1, 1, 10); 
meshIntervalNumPoints   = 5 * ones(1, length(meshIntervalFractions)-1); 

%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 4.50492518;
timeLowerBound    = 0;
timeUpperBound    = 100;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = [-2; -2; -2;-2; -2; -2;1000/mscale];
stateUpperBound   = [2; 2; 2;2; 2; 2;1500/mscale];

%  Set control properties
numControlVars    = 3;
controlUpperBound = thrust_limit * ones(3,1);
controlLowerBound = -thrust_limit * ones(3,1);

phase1.SetNumStateVars(numStateVars)
phase1.SetNumControlVars(numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions); 
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBound)
phase1.SetStateUpperBound(stateUpperBound)
phase1.SetStateInitialGuess([zeros(1,6),1200/mscale])
phase1.SetStateFinalGuess([zeros(1,6),1200/mscale])
phase1.SetTimeLowerBound(timeLowerBound)
phase1.SetTimeUpperBound(timeUpperBound)
phase1.SetTimeInitialGuess(initialGuessTime)
phase1.SetTimeFinalGuess(finalGuessTime)
phase1.SetControlLowerBound(controlLowerBound)
phase1.SetControlUpperBound(controlUpperBound)

%% =====  Define Properties for the Trajectory

%  Create trajectory and configure user function names 
traj = Trajectory();
traj.pathFunctionObject  = DROtoL2PathObject();
traj.pointFunctionObject = DROtoL2PointObject();
traj.plotFunctionName = 'DROtoL2PlotFunction';
traj.guessFunctionName = 'DROtoL2NewGuessFunction';
traj.showPlot = true;
traj.plotUpdateRate = 2;
traj.costLowerBound = 0;
traj.costUpperBound = 1000;
traj.maxMeshRefinementCount = 6;
traj.maxMajorIterations = 75;
traj.maxTotalIterations = 100000;
traj.optimalityTol = 1e-2;
traj.feasibilityTol = 1e-5;

%% =====  Run The Optimizer and plot the solution

traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();

% Plot the solution
PlotDROtoL2Solution



