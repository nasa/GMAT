%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import RauAutomaticaPathObj
import RauAutomaticaPointObj
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject = RauAutomaticaPathObj();
traj.pointFunctionObject   = RauAutomaticaPointObj();
%traj.plotFunctionName   = 'RaoAutomaticaPlotFunction';
%traj.guessFunctionName  = 'RaoAutomaticaGuessFunction';
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 0;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
method = 1;
if method == 1;
    phase1 = RadauPhase;
    phase1.initialGuessMode = 'LinearUnityControl';
    meshIntervalFractions = [-1 1];
    meshIntervalNumPoints = [10];
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase();
    phase1.SetTranscription('HermiteSimpson');
    phase1.initialGuessMode = 'LinearNoControl';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [6];
end

%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 1.5;
timeLowerBound    = -5;
timeUpperBound    = 5;
%  Set state state properties
numStateVars      = 1;
stateLowerBound   = [-10]';
initialGuessState = [0]';
finalGuessState   = [2]';
stateUpperBound   = [10]';
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

stateSol = traj.phaseList{1}.DecVector.GetStateArray();
controlSol = traj.phaseList{1}.DecVector.GetControlArray();
times = traj.phaseList{1}.TransUtil.GetTimeVector();
figure(1)
plot(times,stateSol(:,1))
figure(2)
if isa(phase1,'RadauPhase')
    plot(times(1:end-1),controlSol(:,1))
else
    plot(times,controlSol(:,1))
end

cnt = 0;
for t = 0:.1:2
    cnt = cnt + 1;
    a = 1 + 3*exp(5*t/2);
    time(cnt) = t;
    y(cnt) =4/a;
    u(cnt) = y(cnt)/2;
end

figure(3)
plot(time,y)
figure(4)
plot(time,u)
