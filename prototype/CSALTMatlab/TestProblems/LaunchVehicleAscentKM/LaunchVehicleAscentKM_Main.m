%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
clear mex
global igrid iGfun jGvar traj
import LaunchVehicleAscentKMPathObj
import LaunchVehicleAscentKMPointObj
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = LaunchVehicleAscentKMPathObj();
traj.pointFunctionObject = LaunchVehicleAscentKMPointObj();
% traj.plotFunctionName   = 'LinearTangentSteeringPlotFunction';
traj.guessFunctionName = 'LaunchVehicleAscentKMGuessFunction';
traj.showPlot               = false();
traj.plotUpdateRate         = 2;
traj.costLowerBound         = -Inf;
traj.costUpperBound         = Inf;
traj.maxMeshRefinementCount = 10;
traj.ifScaling              = true;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
meshIntervalFractions   = [-1 -0.33 0.33  1];
meshIntervalNumPoints   = [6 6 8];

phase1 = ImplicitRungeKuttaPhase
phase1.SetTranscription('RungeKutta6');
meshIntervalFractions   = [0  1];
meshIntervalNumPoints   = [15];

phase1.initialGuessMode        = 'UserGuessFunction';
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 150;
timeLowerBound    = 0;
timeUpperBound    = 200;
%  Set state state properties
numStateVars      = 4;
stateLowerBound   = [0 0 0 0]';
initialGuessState = [0 0 0 0]'; % [x y vx vy]
finalGuessState   = [12e4 1.8e5  7726.3  0]'/1000; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [1.8e5 1.8e5 10000 10000]'/1000;
%  Set control properties
numControlVars    = 2;
controlUpperBound = [10 10]';
controlLowerBound = [-10 -10]';

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
%traj.Initialize()
[z,F,xmul,Fmul] = traj.Optimize();

stateSol = traj.phaseList{1}.DecVector.GetStateArray();
controlSol = traj.phaseList{1}.DecVector.GetControlArray();
times = traj.phaseList{1}.TransUtil.GetTimeVector();
figure(1)
subplot(2,1,1)
plot(times,stateSol(:,1),times,stateSol(:,2))
title('Position vs. Time')
xlabel('Time, sec')
ylabel('Position, km')
legend('x-coordinate','y-coordinate')
grid on

subplot(2,1,2)
plot(times,stateSol(:,3),times,stateSol(:,4))
title('Velocity vs. Time')
xlabel('Time, sec')
ylabel('Velocity, km/s')
legend('vx ','vy')
grid on

figure(2)
for i = 1:length(controlSol)
    theta(i) = atan2(controlSol(i,2),controlSol(i,1))*180/pi;
end
if isa(phase1,'RadauPhase')
    plot(times(1:end-1),theta)
else
    plot(times,theta)
end
title('Control vs. Time')
xlabel('Time, sec')
ylabel('Control, degrees')
grid on


figure(3)
plot(stateSol(:,1),stateSol(:,2))
title('X vs. Y Time History')
xlabel('x-coordinate, km')
ylabel('y-coordinate, km')
grid on

