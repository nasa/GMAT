%% =====  Intializations
ClearAll
global igrid iGfun jGvar traj
import HyperSensitivePathObject
import HyperSensitivePointObject
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%% =====  Define the phase and trajectory properties

%  Create trajectory and its settings
traj                           = Trajectory();
%traj.pathFunctionName       = 'HyperSensitivePathFunction';
traj.pathFunctionObject       = HyperSensitivePathObject();
traj.pointFunctionObject       = HyperSensitivePointObject();
traj.plotFunctionName        = 'HyperSensitivePlotFunction';
traj.showPlot                = false();
traj.costLowerBound          = 0;
traj.costUpperBound          = Inf;
traj.plotUpdateRate          = 3;
traj.maxMeshRefinementCount  = 0;

%%  Configure Phase 1

numStateVars = 1;

method = 1;
if method == 1;
    phase1                         = RadauPhase;
    phase1.initialGuessMode        = 'LinearNoControl';
    meshIntervalFractions   = [ -1 -0.98 0 0.98  1];
    meshIntervalNumPoints   = [7 5 5 7]';
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.SetTranscription('RungeKutta8');
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [10];
end

%  Time configuration
timeLowerBound          = 0;
initialGuessTime        = 0;
finalGuessTime          = 10000;
timeUpperBound          = 10000;
%  State configuration
numStateVars      = numStateVars;
stateLowerBound   = -50;
initialGuessState = 1;
finalGuessState   = 1.5;
stateUpperBound   = 50;
%  Control configuration
controlUpperBound = 50;
controlLowerBound = -50;
numControlVars    = 1;

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

%% =====  Initialize the trajectory and phases and get ready for run

% Determine the sparsity pattern for the combined
% cost function and constraint Jacobian
traj.phaseList = {phase1};
traj.ifScaling = false();
[z,info] = traj.OptimizeIPOPT();

%  Interpolate solution
timeVector = traj.phaseList{1}.TransUtil.GetTimeVector();
stateSol = traj.phaseList{1}.DecVector.GetStateArray();
controlSol = traj.phaseList{1}.DecVector.GetControlArray();
newTimeVector = min(timeVector):0.1:max(timeVector);

newStateArray = zeros(length(newTimeVector),traj.phaseList{1}.Config.GetNumStateVars());
newControlArray = zeros(length(newTimeVector(1:end-1)),traj.phaseList{1}.GetNumControlVars());
for stateIdx = 1:traj.phaseList{1}.Config.GetNumStateVars()
    newStateArray(:,stateIdx) = interp1(timeVector,stateSol(:,stateIdx),newTimeVector);
end

for controlIdx = 1:traj.phaseList{1}.GetNumControlVars()
    newControlArray(:,controlIdx) = interp1(timeVector(1:end-1),controlSol(:,controlIdx),newTimeVector(1:end-1),'linear','extrap');
end
%%
figure;plot(timeVector,stateSol,'k*-')
hold on;
plot(newTimeVector,newStateArray)
grid on
title('State History vs. Time')
axis([0 10000 -.2 1])

figure;plot(timeVector(1:end-1),controlSol)
axis([0 10000 -1 3])
grid on
title('Control History vs. Time')
