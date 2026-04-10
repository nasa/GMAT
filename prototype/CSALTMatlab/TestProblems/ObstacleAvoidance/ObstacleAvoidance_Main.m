%==========================================================================
%% =====  Initializations
%==========================================================================
%ClearAll
global igrid iGfun jGvar traj
import ObstacleAvoidancePathObject
import ObstacleAvoidancePointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = ObstacleAvoidancePathObject();
traj.pointFunctionObject = ObstacleAvoidancePointObject();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 10;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearUnityControl';
meshIntervalFractions   = [-1 -.4 .4 1];
meshIntervalNumPoints   = [10 10 10];
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 1;
timeLowerBound    = 0;
timeUpperBound    = 1;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [0 0]';
initialGuessState = [0 0]';
finalGuessState   = [1.2 1.6]';
stateUpperBound   = [1.2 1.6]';
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

%==========================================================================
%% =====  Plot the solution
%==========================================================================
for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    figure(1);
    hold on; grid on;
    plot(stateSol(:,1),stateSol(:,2))
    title('state history')
    figure(2)
    hold on; grid on;
    if isa(phase1,'RadauPhase')
        plot(times(1:end-1),controlSol(:,1))
    else
        plot(times,controlSol(:,1))
    end
    title('control history')
end

%  Plot the exclusion regions
figure(1)
cnt = 0;
radius = pi/10;
xoffset = 0.4;
yoffset = 0.5;
for theta = 0:0.05:2*pi+.2
    cnt = cnt + 1;
    angle(cnt) = theta;
    x(cnt) = radius*cos(theta) + xoffset;
    y(cnt) = radius*sin(theta) + yoffset;
end
plot(x,y,'r-')

cnt = 0;
radius = pi/10;
xoffset = 0.8;
yoffset = 1.5;
for theta = 0:0.05:2*pi+.2
    cnt = cnt + 1;
    angle(cnt) = theta;
    x(cnt) = radius*cos(theta) + xoffset;
    y(cnt) = radius*sin(theta) + yoffset;
end
plot(x,y,'c-')
axis equal
axis([0 1.2 0 2.0])
