%Nathan Parrish
%GSFC 2016
%
%First implementation of the DRO -> L2 low thrust transfer problem using
%the GSFC optimal control prototype.

%% =====  Initializations
clear all
close all
clc
global igrid iGfun jGvar traj 
import DROtoL2PathObject
import DROtoL2PointObject
import exec.Trajectory
import exec.RadauPhase

global optim_tol
optim_tol = 1e-2;


% Load initial guess
f = 'initial_guess';

global t_guess x_guess u_guess
%times:
t_guess = importdata(fullfile(f,'t_guess.dat'));
%states:
x_guess = importdata(fullfile(f,'x_guess.dat'));
%controls:
u_guess = importdata(fullfile(f,'u_guess.dat'));
%parameters:
% p_guess = importdata(fullfile(f,'p_guess.dat'));


%% =====  Define Properties for the Trajectory

%  Create trajectory and configure user function names 
traj                    = Trajectory();
traj.pathFunctionObject  = DROtoL2PathObject();
traj.pointFunctionObject = DROtoL2PointObject();
traj.plotFunctionName   = 'DROtoL2PlotFunction';
traj.guessFunctionName  = 'DROtoL2GuessFunction';
% traj.showPlot           = true();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = 0;
traj.costUpperBound     = 1000;
traj.maxMeshRefinementCount = 10;

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

global Nphase phase_type
Nphase = 1;
phase_type = 2; %thrust optional

%% =====  Define Properties for Phase 1

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode         = 'UserGuessFunction';
% phase1.initialGuessMode         = 'LinearNoControl';
meshIntervalFractions   = linspace(-1, 1, 2); 
% meshIntervalNumPoints   = round(60/length(meshIntervalFractions)) * ones(1, length(meshIntervalFractions)-1); 
meshIntervalNumPoints   = 10 * ones(1, length(meshIntervalFractions)-1); 
% meshIntervalFractions   = [-1 1]; 
% meshIntervalNumPoints   = [50]; 

%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 4.50492518;
timeLowerBound    = 0;
timeUpperBound    = 100;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = [-2; -2; -2;...
    -2; -2; -2;...
    1000/mscale];
stateUpperBound   = [2; 2; 2;...
    2; 2; 2;...
    1500/mscale];
initialGuessState = [ 1.0580674891467
        0.0585680252494724
     -4.12647691546781e-28
         0.272023839130877
        -0.364651438588559
      5.37693109951801e-17
                       1.5]; %
finalGuessState   = [ 1.1294862635098
         -0.10132418468342
        -0.103240089657033
        -0.083234727395094
        -0.138391036163704
         0.128700247647937
          1.47228514179783]; % 

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

phase1.SetStateInitialGuess(initialGuessState)
phase1.SetStateFinalGuess(finalGuessState)

phase1.SetTimeLowerBound(timeLowerBound)
phase1.SetTimeUpperBound(timeUpperBound)

phase1.SetTimeInitialGuess(initialGuessTime)
phase1.SetTimeFinalGuess(finalGuessTime)

phase1.SetControlLowerBound(controlLowerBound)
phase1.SetControlUpperBound(controlUpperBound)
 


%% =====  Define the linkage configuration and optimize

traj.phaseList = {phase1};
traj.Initialize()