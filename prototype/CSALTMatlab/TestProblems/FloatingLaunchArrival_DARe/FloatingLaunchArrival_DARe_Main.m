ClearAll
global igrid iGfun jGvar traj SEPForces config LV
import FloatingLaunchArrival_DARePathObject
import FloatingLaunchArrival_DARePointObject
import LaunchVehicle
import exec.Trajectory
import exec.RadauPhase

init_eph('DE405.dat');

config.gravParam = 132712440017.99;
config.DU  = 149597871;
config.TU  = sqrt(config.DU^3/config.gravParam);
config.MU  = 1000;
config.VU = config.DU/config.TU;
config.TimeOffSet = 29366.400011573;
config.StateScaleMat = diag([ones(3,1)/config.DU;  ones(3,1)/config.VU; 1/config.MU]);
config.StateAccScaleMat = diag([
    ones(3,1)*(config.TU/config.DU);
    ones(3,1)*(config.TU^2/config.DU);
    (config.TU/config.MU)]);
config.StateUnScaleMat = inv(config.StateScaleMat);
config.StateAccUnScaleMat = inv(config.StateAccScaleMat);

config.maxFlightTime = 3.25 * 365.25 * 86400 / config.TU;
config.launchWindow = 30 * 86400 / config.TU;

ThreeAU = 3*config.DU;

LV = LaunchVehicle();
LV.SetLaunchVehicleType(12)

%phaseTimes = [0 60*86400 2*365.25*86400 74586988.5135788 9.32337356419735E+07];
phaseTimes = [0  60 660 851 1079]*86400;

%==========================================================================
%% =====  Initializations
%==========================================================================
MyPathConfig
DynamicsConfig
pathObj = FloatingLaunchArrival_DARePathObject();
pathObj.SEPForces = SEPForces;
pathObj.config = config;

addpath(genpath('/Users/jknittel/Utilities/mice'));
cspice_furnsh( '/Users/jknittel/Universe/naif0011.tls');
cspice_furnsh( '/Users/jknittel/Universe/2000336.bsp');
cspice_furnsh( '/Users/jknittel/Universe/de421.bsp');

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                      = Trajectory();
traj.pathFunctionObject   = pathObj;
traj.pointFunctionObject  = FloatingLaunchArrival_DARePointObject();
traj.guessFunctionName    = 'DAREGuessFunction';
traj.showPlot             = false();
traj.plotUpdateRate       = 2;
traj.costLowerBound       = -Inf;
traj.costUpperBound       = Inf;
traj.maxMeshRefinementCount = 0;

bogusGuessI = config.StateScaleMat*[-6515.97236231483
    931.372927012761
    -1066.34795707823
    -0.888689684465484
    -7.58686200413001
    -1.1949439525609
    500];

bogusGuessF = config.StateScaleMat*[5831.737783903891
    2885.501720473778
    1460.368242609286
    -3.485967401347486
    6.894283442829861
    0.2997220259503012
    500];

controlBoundU = [1 1 1]'*100;
controlBoundL = [-1 -1 -1]'*100;

%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode         = 'UserGuessFunction';

meshIntervalFractions   = [-1 1];
meshIntervalNumPoints   = [12];

%  Set time properties
timeLowerBound    = 0/config.TU;
initialGuessTime  = phaseTimes(1)/config.TU;
finalGuessTime    = phaseTimes(2)/config.TU;
timeUpperBound    = 2*phaseTimes(5)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-ThreeAU -ThreeAU -ThreeAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[ThreeAU ThreeAU ThreeAU 45 45 45 2000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

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

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

phase2                         = RadauPhase;
phase2.initialGuessMode         = 'UserGuessFunction';

meshIntervalFractions   = [-1 1];
meshIntervalNumPoints   = [20];

%  Set time properties
timeLowerBound    = 0.9*phaseTimes(2)/config.TU;
initialGuessTime  = phaseTimes(2)/config.TU;
finalGuessTime    = phaseTimes(3)/config.TU;
timeUpperBound    = 1.1*phaseTimes(3)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-ThreeAU -ThreeAU -ThreeAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[ThreeAU ThreeAU ThreeAU 45 45 45 2000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase2.SetNumStateVars(numStateVars)
phase2.SetNumControlVars(numControlVars)
phase2.SetMeshIntervalFractions(meshIntervalFractions)
phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints)
phase2.SetStateLowerBound(stateLowerBound)
phase2.SetStateUpperBound(stateUpperBound)
phase2.SetStateInitialGuess(initialGuessState)
phase2.SetStateFinalGuess(finalGuessState)
phase2.SetTimeLowerBound(timeLowerBound)
phase2.SetTimeUpperBound(timeUpperBound)
phase2.SetTimeInitialGuess(initialGuessTime)
phase2.SetTimeFinalGuess(finalGuessTime)
phase2.SetControlLowerBound(controlLowerBound)
phase2.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 3
%==========================================================================

phase3                         = RadauPhase;
phase3.initialGuessMode         = 'UserGuessFunction';

meshIntervalFractions   = [-1 1];
meshIntervalNumPoints   = [10];

%  Set time properties
timeLowerBound    = 0.9*phaseTimes(3)/config.TU;
initialGuessTime  = phaseTimes(3)/config.TU;
finalGuessTime    = phaseTimes(4)/config.TU;
timeUpperBound    = 1.2*phaseTimes(4)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-ThreeAU -ThreeAU -ThreeAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[ThreeAU ThreeAU ThreeAU 45 45 45 2000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = controlBoundU ;
controlLowerBound = controlBoundL;

phase3.SetNumStateVars(numStateVars)
phase3.SetNumControlVars(numControlVars);
phase3.SetMeshIntervalFractions(meshIntervalFractions);
phase3.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase3.SetStateLowerBound(stateLowerBound)
phase3.SetStateUpperBound(stateUpperBound)
phase3.SetStateInitialGuess(initialGuessState)
phase3.SetStateFinalGuess(finalGuessState)
phase3.SetTimeLowerBound(timeLowerBound)
phase3.SetTimeUpperBound(timeUpperBound)
phase3.SetTimeInitialGuess(initialGuessTime)
phase3.SetTimeFinalGuess(finalGuessTime)
phase3.SetControlLowerBound(controlLowerBound)
phase3.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 4
%==========================================================================

phase4                         = RadauPhase;
phase4.initialGuessMode        = 'UserGuessFunction';

meshIntervalFractions   = [-1 1];
meshIntervalNumPoints   = [10];

%  Set time properties
timeLowerBound    = phaseTimes(1)/config.TU;
initialGuessTime  = phaseTimes(4)/config.TU;
finalGuessTime    = phaseTimes(5)/config.TU;
timeUpperBound    = 2*phaseTimes(5)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-ThreeAU -ThreeAU -ThreeAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[ThreeAU ThreeAU ThreeAU 45 45 45 2000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU ;
controlLowerBound = controlBoundL;

phase4.SetNumStateVars(numStateVars)
phase4.SetNumControlVars(numControlVars);
phase4.SetMeshIntervalFractions(meshIntervalFractions);
phase4.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase4.SetStateLowerBound(stateLowerBound)
phase4.SetStateUpperBound(stateUpperBound)
phase4.SetStateInitialGuess(initialGuessState)
phase4.SetStateFinalGuess(finalGuessState)
phase4.SetTimeLowerBound(timeLowerBound)
phase4.SetTimeUpperBound(timeUpperBound)
phase4.SetTimeInitialGuess(initialGuessTime)
phase4.SetTimeFinalGuess(finalGuessTime)
phase4.SetControlLowerBound(controlLowerBound)
phase4.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1,phase2,phase3,phase4};
% traj.linkageConfig{1} = {phase1,phase2};
% traj.linkageConfig{2} = {phase2,phase3};
% traj.linkageConfig{3} = {phase3,phase4};
[z,F,xmul,Fmul] = traj.Optimize();

%==========================================================================
%% =====  Plot the solution
%==========================================================================
clf
numPhasesToPlot = 4;
for phaseIdx = 1:numPhasesToPlot
	figure(1); hold on;
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();

    hold on
    fac = 1/norm(max(controlSol));
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    if ~isinf(fac)
        lineWidth = 2;
        lineColor = 'kx-';
    else
        lineWidth = 1;
        lineColor = 'bx-';
    end
        plot3(stateSol(:,1),stateSol(:,2),stateSol(:,3),lineColor,'LineWidth',lineWidth);
    for i = 1:length(controlSol)
        startX = stateSol(i,1);
        startY = stateSol(i,2);
        startZ = stateSol(i,3);
        endX = stateSol(i,1) + fac*controlSol(i,1);
        endY = stateSol(i,2) + fac*controlSol(i,2);
        endZ = stateSol(i,3) + fac*controlSol(i,3);
        plot3([startX;endX],[startY;endY],[startZ;endZ],'r-');
    end
    axis([-3 3 -3 3 -3 3])
	
	figure(2); hold on;
	if isa(traj.phaseList{phaseIdx},'RadauPhase') && traj.phaseList{phaseIdx}.GetNumControlVars()
	    plot(times(1:end-1),controlSol(:,1),'DisplayName',['Phase ',num2str(phaseIdx),' x-control'])
	    plot(times(1:end-1),controlSol(:,2),'DisplayName',['Phase ',num2str(phaseIdx),' y-control'])
	    plot(times(1:end-1),controlSol(:,3),'DisplayName',['Phase ',num2str(phaseIdx),' z-control'])
	elseif traj.phaseList{phaseIdx}.GetNumControlVars()
		plot(times,controlSol(:,1))
	end
end


figure(1)
cnt = 0;
for time = 0:0.01:4
    mjd = config.TimeOffSet + time*365.25;
    cnt = cnt + 1;
    
    [stateEarth] = cspice_spkezr('EARTH', ...
        (mjd - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');
    [stateAsteroid] = cspice_spkezr('2000336',...
        (mjd - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');
    earthPos(cnt,:) = stateEarth(1:3)/config.DU;
    astPos(cnt,:) = stateAsteroid(1:3)/config.DU;
end
hold on
plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
plot3(astPos(:,1),astPos(:,2),astPos(:,3),'k-')
view([0 0 1])
axis equal
axis([-3 3 -3 3 -3 3])
grid on

%figure(2)
%legend on