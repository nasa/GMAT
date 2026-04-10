%% =====  Intializations
clear classes;
clear mex;
clear all;
close all;
clc; clearvars; 
global iGfun jGvar traj igrid

%% =====  Define the phase and trajectory properties

%  set cost function properties
traj                    = Trajectory();
traj.pathFunctionName   = 'OrbitRaisingPathFunction2';
traj.pointFunctionName  = 'OrbitRaisingPointFunction2';
traj.plotFunctionName   = 'OrbitRaisingPlotFunction2';
traj.showPlot           = true();
traj.plotUpdateRate     = 3;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;

%  Set Time Properties
phase1                           = Phase();
phase1.meshIntervalFractions     = [-1; 1];
phase1.meshIntervalNumPoints     = 20*ones(1,1);
phase1.initialEpoch              = 0;
phase1.finalEpoch                = 10.32;
phase1.initialTimeLowerBound     = 0;
phase1.initialTimeUpperBound     = 0;
phase1.finalTimeLowerBound       = 0.32;
phase1.finalTimeUpperBound       = 15.32;

%  Set state and control properties
phase1.numStates                 = 5;
phase1.initialGuessMode          = 'LinearNoControl';
phase1.initialGuessState         = [1 0 0 1 1]'; % [r0 theta0 vr0 vtheta0 m0]
phase1.initialGuessEpoch         = 0;
phase1.finalGuessState           = [1.5 pi/2 0 1 1]'; % [rf thetaf vrf vthetaf mf]
phase1.finalGuessEpoch           = 1;
%                                 r t rd td m
phase1.initialStateLowerBound    = [1 0 0 1 1]';
phase1.initialStateUppperBound   = [1 0 0 1 1]';
phase1.finalStateLowerBound      = [-1 -pi 0 -10 0]';
phase1.finalStateUpperBound      = [10 pi 0 10 1]';
phase1.stateLowerBound           = [0.5 0 -10 -10 0.1]';
phase1.stateUpperBound           = [5 4*pi 10 10 3]';
phase1.numControls               = 2;
phase1.controlUpperBound         = [10 10]';
phase1.controlLowerBound         = [-10 -10]';
phase1.algConstraintLowerBound   = 0;
phase1.algConstraintUpperBound   = 1;
phase1.eventConstraintLowerBound = [1.5 0]';
phase1.eventConstraintUpperBound = [1.5 0]';


%% =====  Initialize the trajectory and phases and get ready for run

% Determine the sparsity pattern for the combined 
% cost function and constraint Jacobian
traj.phaseList = {phase1};
[z,F,xmul,Fmul] = traj.Optimize();