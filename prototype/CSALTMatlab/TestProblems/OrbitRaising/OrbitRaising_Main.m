%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import OrbitRaisingPathObject
import OrbitRaisingPointObject
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================
tic
%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = OrbitRaisingPathObject();
traj.pointFunctionObject = OrbitRaisingPointObject();
traj.plotFunctionName   = 'OrbitRaisingPlotFunction';
traj.showPlot           = false();
traj.plotUpdateRate     = 3;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 0;

%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================

method = 1;
if method == 1;
    phase1  = RadauPhase();
    phase1.initialGuessMode  = 'LinearUnityControl';
    meshIntervalFractions  = [-1; 1];
    %phase1.meshIntervalNumPoints     = 50*ones(1,1);
    meshIntervalNumPoints = 7*ones(1,1);
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.initialGuessMode  = 'LinearNoControl';
    phase1.SetTranscription('HermiteSimpson')
    meshIntervalFractions  = [0 1];
    %phase1.meshIntervalNumPoints     = 50*ones(1,1);
    meshIntervalNumPoints  = 10*ones(1,1);
end

%  Set time properties
timeLowerBound   = 0;
initialGuessTime = 0;
finalGuessTime   = 1;
timeUpperBound   = 3.32
%  Set state properties
numStateVars              = 5;
stateLowerBound           = [0.5 0 -10 -10 0.1]';
initialGuessState         = [1 0 0  1 1]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState           = [1 pi 0 .5 0.5]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound           = [5 4*pi 10 10 3]';
%  Set control properties
numControlVars            = 2;
controlUpperBound         = [10 10]';
controlLowerBound         = [-10 -10]';

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

polar(stateSol(:,2), stateSol(:,1))

figure(1)
subplot(2,2,1)
title('Radius vs. Time'); hold on
plot(times,stateSol(:,1),'bo-'); grid on;
subplot(2,2,2)
title('Theta vs. Time'); hold on
plot(times,stateSol(:,2),'bo-'); grid on;
subplot(2,2,3)
title('V_R vs. Time'); hold on
plot(times,stateSol(:,3),'bo-'); grid on;
subplot(2,2,4)
title('V_\theta vs. Time'); hold on
plot(times,stateSol(:,4),'bo-'); grid on;
return
figure(2)
subplot(2,1,1)
title('u_1 vs. Time'); hold on
plot(times(1:end-1,1),controlSol(:,1),'bo-'); grid on;
subplot(2,1,2)
title('u_2 vs. Time'); hold on
plot(times(1:end-1,1),controlSol(:,2),'bo-'); grid on;

