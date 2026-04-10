ClearAll
global igrid iGfun jGvar traj SEPForces config LV callCount
import DawnPathObject
import DawnPointObject
import LaunchVehicle
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

callCount = 0;

init_eph('DE405.dat');

time2mjd = 2430000.5;

config.method = 1;
config.Control = 'Cartesian';

config.gravParam = 132712440017.99;
config.DU  = 149597871;
config.TU  = sqrt(config.DU^3/config.gravParam);
config.MU  = 1000;
config.VU = config.DU/config.TU;
config.TimeOffSet = 24370-10;
config.StateScaleMat = diag([ones(3,1)/config.DU;  ones(3,1)/config.VU; 1/config.MU]);
config.StateAccScaleMat = diag([
    ones(3,1)*(config.TU/config.DU);
    ones(3,1)*(config.TU^2/config.DU);
    (config.TU/config.MU)]);
config.StateUnScaleMat = inv(config.StateScaleMat);
config.StateAccUnScaleMat = inv(config.StateAccScaleMat);


% config.maxFlightTime = 3000 * 86400 / config.TU;
% config.LaunchWindow = 90 * 86400 / config.TU;

SevenAU = 7.0*config.DU;

LV = LaunchVehicle();
LV.SetLaunchVehicleType(-2)
LV.SetCustomCoefficients([-6.121e-8,2.166e-5,-.0032671,.33055411,-27.329897,1282.81977])

% phaseTimes = [0 60 226 984 1149 2139 2636]*86400;
phaseTimes = ([0 45 173 300 440 527 755 1382 1674 2090 2218 2555 2580 2700])*86400;

%==========================================================================
%% =====  Initializations
%==========================================================================
MyPathConfig
DawnDynamicsConfig
pathObj = DawnPathObject();
pathObj.SEPForces = SEPForces;
pathObj.config = config;

addpath(genpath('/Users/jknittel/Utilities/mice'));
cspice_furnsh( '/Users/jknittel/Universe/naif0011.tls');
cspice_furnsh( '/Users/jknittel/Universe/PlanetsLight.bsp');
cspice_furnsh( '/Users/jknittel/Universe/2000001.bsp');
cspice_furnsh( '/Users/jknittel/Universe/2000004.bsp');
cspice_furnsh( '/Users/jknittel/Universe/de421.bsp');

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                      = Trajectory();
traj.pathFunctionObject   = pathObj;
traj.pointFunctionObject  = DawnPointObject();
traj.guessFunctionName    = 'DawnEMTGGuessFunction';
traj.plotFunctionName     = 'DawnPlotFunction';
traj.showPlot             = true();
traj.plotUpdateRate       = 100;
traj.costLowerBound       = -Inf;
traj.costUpperBound       = Inf;
traj.maxMeshRefinementCount = 10;

bogusGuessI = config.StateScaleMat*[-2.612780114384178E+04;  1.328257092868841E+05;  5.757956041029914E+04; -2.981220584493626E-02; -4.955837697510423E-03; -2.146951323568623E-03; 3]*1000;

bogusGuessF = config.StateScaleMat*[-2.444427441837995E+05;  3.779124150799458E+04;  2.392934774236356E+03; -3.405553850853295E-03; -1.982966469562482E-02; -9.003561269272474E-03; 2]*1000;

if strcmp(config.Control,'Spherical')
    controlBoundU = [ 10000  10000 1]';
    controlBoundL = [-10000 -10000 0]';
elseif strcmp(config.Control,'Cartesian')
    controlBoundU = [1 1 1]';
    controlBoundL = [-1 -1 -1]';
end

%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================


if config.method == 1;
    phase1 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase1 = ImplicitRungeKuttaPhase;
    phase1.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase1 = ImplicitRungeKuttaPhase;
    phase1.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

%  Set mesh properities
% phase1                         = RadauPhase;
phase1.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = 0;
initialGuessTime  = phaseTimes(1)/config.TU;
finalGuessTime    = phaseTimes(2)/config.TU;
timeUpperBound    = phaseTimes(3)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = 0;
controlLowerBound = 0;

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

%  Set mesh properities
if config.method == 1;
    phase2 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase2 = ImplicitRungeKuttaPhase;
    phase2.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase2 = ImplicitRungeKuttaPhase;
    phase2.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase2.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    =    phaseTimes(1)/config.TU;
initialGuessTime  =    phaseTimes(2)/config.TU;
finalGuessTime    =    phaseTimes(3)/config.TU;
timeUpperBound    =    phaseTimes(4)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase2.SetNumStateVars(numStateVars)
phase2.SetNumControlVars(numControlVars);
phase2.SetMeshIntervalFractions(meshIntervalFractions);
phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints);
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

%  Set mesh properities
if config.method == 1;
    phase3 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase3 = ImplicitRungeKuttaPhase;
    phase3.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase3 = ImplicitRungeKuttaPhase;
    phase3.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase3.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = phaseTimes(2)/config.TU;
initialGuessTime  = phaseTimes(3)/config.TU;
finalGuessTime    = phaseTimes(4)/config.TU;
timeUpperBound    = phaseTimes(5)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = 0;
controlLowerBound = 0;

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

%  Set mesh properities
if config.method == 1;
    phase4 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase4 = ImplicitRungeKuttaPhase;
    phase4.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase4 = ImplicitRungeKuttaPhase;
    phase4.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase4.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    =    phaseTimes(3)/config.TU;
initialGuessTime  =    phaseTimes(4)/config.TU;
finalGuessTime    =    phaseTimes(5)/config.TU;
timeUpperBound    =    phaseTimes(6)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
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
%% =====  Define Properties for Phase 5
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase5 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase5 = ImplicitRungeKuttaPhase;
    phase5.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase5 = ImplicitRungeKuttaPhase;
    phase5.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase5.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = phaseTimes(4)/config.TU;
initialGuessTime  = phaseTimes(5)/config.TU;
finalGuessTime    = phaseTimes(6)/config.TU;
timeUpperBound    = phaseTimes(7)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = 0;
controlLowerBound = 0;

phase5.SetNumStateVars(numStateVars)
phase5.SetNumControlVars(numControlVars);
phase5.SetMeshIntervalFractions(meshIntervalFractions);
phase5.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase5.SetStateLowerBound(stateLowerBound)
phase5.SetStateUpperBound(stateUpperBound)
phase5.SetStateInitialGuess(initialGuessState)
phase5.SetStateFinalGuess(finalGuessState)
phase5.SetTimeLowerBound(timeLowerBound)
phase5.SetTimeUpperBound(timeUpperBound)
phase5.SetTimeInitialGuess(initialGuessTime)
phase5.SetTimeFinalGuess(finalGuessTime)
phase5.SetControlLowerBound(controlLowerBound)
phase5.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 6
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase6 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase6 = ImplicitRungeKuttaPhase;
    phase6.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase6 = ImplicitRungeKuttaPhase;
    phase6.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase6.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = phaseTimes(5)/config.TU;
initialGuessTime  = phaseTimes(6)/config.TU;
finalGuessTime    = phaseTimes(7)/config.TU;
timeUpperBound    = phaseTimes(8)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = 0;
controlLowerBound = 0;

phase6.SetNumStateVars(numStateVars)
phase6.SetNumControlVars(numControlVars);
phase6.SetMeshIntervalFractions(meshIntervalFractions);
phase6.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase6.SetStateLowerBound(stateLowerBound)
phase6.SetStateUpperBound(stateUpperBound)
phase6.SetStateInitialGuess(initialGuessState)
phase6.SetStateFinalGuess(finalGuessState)
phase6.SetTimeLowerBound(timeLowerBound)
phase6.SetTimeUpperBound(timeUpperBound)
phase6.SetTimeInitialGuess(initialGuessTime)
phase6.SetTimeFinalGuess(finalGuessTime)
phase6.SetControlLowerBound(controlLowerBound)
phase6.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 7
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase7 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase7 = ImplicitRungeKuttaPhase;
    phase7.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase7 = ImplicitRungeKuttaPhase;
    phase7.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase7.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    =    phaseTimes(6)/config.TU;
initialGuessTime  =    phaseTimes(7)/config.TU;
finalGuessTime    =    phaseTimes(8)/config.TU;
timeUpperBound    =    phaseTimes(9)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase7.SetNumStateVars(numStateVars)
phase7.SetNumControlVars(numControlVars);
phase7.SetMeshIntervalFractions(meshIntervalFractions);
phase7.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase7.SetStateLowerBound(stateLowerBound)
phase7.SetStateUpperBound(stateUpperBound)
phase7.SetStateInitialGuess(initialGuessState)
phase7.SetStateFinalGuess(finalGuessState)
phase7.SetTimeLowerBound(timeLowerBound)
phase7.SetTimeUpperBound(timeUpperBound)
phase7.SetTimeInitialGuess(initialGuessTime)
phase7.SetTimeFinalGuess(finalGuessTime)
phase7.SetControlLowerBound(controlLowerBound)
phase7.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 8
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase8 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase8 = ImplicitRungeKuttaPhase;
    phase8.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase8 = ImplicitRungeKuttaPhase;
    phase8.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase8.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = phaseTimes(8)/config.TU;
initialGuessTime  = phaseTimes(9)/config.TU;
finalGuessTime    = phaseTimes(10)/config.TU;
timeUpperBound    = phaseTimes(11)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase8.SetNumStateVars(numStateVars)
phase8.SetNumControlVars(numControlVars);
phase8.SetMeshIntervalFractions(meshIntervalFractions);
phase8.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase8.SetStateLowerBound(stateLowerBound)
phase8.SetStateUpperBound(stateUpperBound)
phase8.SetStateInitialGuess(initialGuessState)
phase8.SetStateFinalGuess(finalGuessState)
phase8.SetTimeLowerBound(timeLowerBound)
phase8.SetTimeUpperBound(timeUpperBound)
phase8.SetTimeInitialGuess(initialGuessTime)
phase8.SetTimeFinalGuess(finalGuessTime)
phase8.SetControlLowerBound(controlLowerBound)
phase8.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 9
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase9 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase9 = ImplicitRungeKuttaPhase;
    phase9.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase9 = ImplicitRungeKuttaPhase;
    phase9.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase9.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = phaseTimes(9)/config.TU;
initialGuessTime  = phaseTimes(10)/config.TU;
finalGuessTime    = phaseTimes(11)/config.TU;
timeUpperBound    = phaseTimes(12)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = 0;
controlLowerBound = 0;

phase9.SetNumStateVars(numStateVars)
phase9.SetNumControlVars(numControlVars);
phase9.SetMeshIntervalFractions(meshIntervalFractions);
phase9.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase9.SetStateLowerBound(stateLowerBound)
phase9.SetStateUpperBound(stateUpperBound)
phase9.SetStateInitialGuess(initialGuessState)
phase9.SetStateFinalGuess(finalGuessState)
phase9.SetTimeLowerBound(timeLowerBound)
phase9.SetTimeUpperBound(timeUpperBound)
phase9.SetTimeInitialGuess(initialGuessTime)
phase9.SetTimeFinalGuess(finalGuessTime)
phase9.SetControlLowerBound(controlLowerBound)
phase9.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 10
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase10 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase10 = ImplicitRungeKuttaPhase;
    phase10.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase10 = ImplicitRungeKuttaPhase;
    phase10.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase10.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    =    phaseTimes(10)/config.TU;
initialGuessTime  =    phaseTimes(11)/config.TU;
finalGuessTime    =    phaseTimes(12)/config.TU;
timeUpperBound    =    phaseTimes(13)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase10.SetNumStateVars(numStateVars)
phase10.SetNumControlVars(numControlVars);
phase10.SetMeshIntervalFractions(meshIntervalFractions);
phase10.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase10.SetStateLowerBound(stateLowerBound)
phase10.SetStateUpperBound(stateUpperBound)
phase10.SetStateInitialGuess(initialGuessState)
phase10.SetStateFinalGuess(finalGuessState)
phase10.SetTimeLowerBound(timeLowerBound)
phase10.SetTimeUpperBound(timeUpperBound)
phase10.SetTimeInitialGuess(initialGuessTime)
phase10.SetTimeFinalGuess(finalGuessTime)
phase10.SetControlLowerBound(controlLowerBound)
phase10.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 11
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase11 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase11 = ImplicitRungeKuttaPhase;
    phase11.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase11 = ImplicitRungeKuttaPhase;
    phase11.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase11.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    = phaseTimes(11)/config.TU;;
initialGuessTime  = phaseTimes(12)/config.TU;
finalGuessTime    = phaseTimes(13)/config.TU;
timeUpperBound    = phaseTimes(14)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 0;
controlUpperBound = 0;
controlLowerBound = 0;

phase11.SetNumStateVars(numStateVars)
phase11.SetNumControlVars(numControlVars);
phase11.SetMeshIntervalFractions(meshIntervalFractions);
phase11.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase11.SetStateLowerBound(stateLowerBound)
phase11.SetStateUpperBound(stateUpperBound)
phase11.SetStateInitialGuess(initialGuessState)
phase11.SetStateFinalGuess(finalGuessState)
phase11.SetTimeLowerBound(timeLowerBound)
phase11.SetTimeUpperBound(timeUpperBound)
phase11.SetTimeInitialGuess(initialGuessTime)
phase11.SetTimeFinalGuess(finalGuessTime)
phase11.SetControlLowerBound(controlLowerBound)
phase11.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 12
%==========================================================================

%  Set mesh properities
if config.method == 1;
    phase12 = RadauPhase;
	meshIntervalFractions   =  [-1,-.75,-.5,-.25,0,.25,.5,.75,1];
	meshIntervalNumPoints   = [3,3,3,3,3,3,3,3];
elseif config.method == 2
    phase12 = ImplicitRungeKuttaPhase;
    phase12.SetTranscription('Trapezoid');
    % meshIntervalFractions = [0 .5 1];
    % meshIntervalNumPoints = [5 5];
	meshIntervalFractions   =  [0,1];
	meshIntervalNumPoints   = [10];
elseif config.method == 3
    phase12 = ImplicitRungeKuttaPhase;
    phase12.SetTranscription('HermiteSimpson');
    meshIntervalFractions = [0 .5 1];
    meshIntervalNumPoints = [7 7];
end

phase12.initialGuessMode         = 'UserGuessFunction';

%  Set time properties
timeLowerBound    =    phaseTimes(12)/config.TU;
initialGuessTime  =    phaseTimes(13)/config.TU;
finalGuessTime    =    phaseTimes(14)/config.TU;
timeUpperBound    =    3000*86400/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase12.SetNumStateVars(numStateVars)
phase12.SetNumControlVars(numControlVars);
phase12.SetMeshIntervalFractions(meshIntervalFractions);
phase12.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase12.SetStateLowerBound(stateLowerBound)
phase12.SetStateUpperBound(stateUpperBound)
phase12.SetStateInitialGuess(initialGuessState)
phase12.SetStateFinalGuess(finalGuessState)
phase12.SetTimeLowerBound(timeLowerBound)
phase12.SetTimeUpperBound(timeUpperBound)
phase12.SetTimeInitialGuess(initialGuessTime)
phase12.SetTimeFinalGuess(finalGuessTime)
phase12.SetControlLowerBound(controlLowerBound)
phase12.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% ===== Plot the boundary orbits
%==========================================================================

figure(1)
cnt = 0;
clear earthPos marsPos ceresPos vestaPos
for time = 0:0.01:7
    mjd = config.TimeOffSet + time*365.25;
    cnt = cnt + 1;

    [stateEarth] = cspice_spkezr('EARTH', ...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateMars] = cspice_spkezr('MARS',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateCeres] = cspice_spkezr('2000001',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateVesta] = cspice_spkezr('2000004',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    earthPos(cnt,:) = stateEarth(1:3)/config.DU;
    marsPos(cnt,:)  = stateMars(1:3)/config.DU;
    ceresPos(cnt,:) = stateCeres(1:3)/config.DU;
    vestaPos(cnt,:) = stateVesta(1:3)/config.DU;
end
hold on
plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
plot3(marsPos(:,1),marsPos(:,2),marsPos(:,3),'k-')
plot3(ceresPos(:,1),ceresPos(:,2),ceresPos(:,3),'k-')
plot3(vestaPos(:,1),vestaPos(:,2),vestaPos(:,3),'k-')
view([0 0 1])
axis equal
axis([-3 3 -3 3 -3 3])
grid on

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1,phase2,phase3,phase4,phase5,phase6,phase7,phase8,phase9,phase10,phase11,phase12};
[z,F,xmul,Fmul] = traj.Optimize();

%==========================================================================
%% =====  Plot the solution
%==========================================================================
%clf
numPhasesToPlot = 12;
[constraintFunctions,ifEquality] = traj.GetScaledConstraintFunctions();
constraintNames = traj.GetConstraintVectorNames();

for i = 1:length(ifEquality)
	if ifEquality(i) && abs(constraintFunctions(i)) > 1e-6
		disp([constraintNames{i},' (equality) is possibly infeasible with value: ',num2str(constraintFunctions(i))])
	elseif constraintFunctions(i) < -1e-6 || constraintFunctions(i) > 1+1e-6
		disp([constraintNames{i},' is possibly infeasible with scaled value: ',num2str(constraintFunctions(i),20)])
	end
end

for phaseIdx = 1:numPhasesToPlot
    disp(['Phase ',num2str(phaseIdx)])
    frac_str = 'meshIntervalFractions   =  [';
    num_str = 'meshIntervalNumPoints   = [';
    fracs = traj.phaseList{phaseIdx}.GetMeshIntervalFractions();
    nums = traj.phaseList{phaseIdx}.GetMeshIntervalNumPoints();
    for i_f = 1:length(fracs)
       frac_str = [frac_str,num2str(fracs(i_f))];
       if i_f ~= 1
            num_str = [num_str,num2str(nums(i_f-1))];
       end
       if i_f ~= length(fracs)
          frac_str = [frac_str,','];
          if i_f ~= 1
            num_str = [num_str,','];
          end
       else
          frac_str = [frac_str,'];'];
          num_str = [num_str,'];'];           
       end
    end
    disp(frac_str)
    disp(num_str)
	    
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();

    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    
    timeTypeVec = ones(1,length(times));
    timeTypeVec(end) = 2;
	
    if isa(traj.phaseList{phaseIdx},'exec.RadauPhase') 
        timeVec = times(1:end-1)*config.TU/86400/365.25;
    else
        timeVec = times(1:end)*config.TU/86400/365.25;
    end
	
	if traj.phaseList{phaseIdx}.GetNumControlVars()
		nThruster = zeros(1,length(times)-1);
		power_in = zeros(1,length(times)-1);
% 		net_mdot = zeros(1,length(times)-1);
% 		net_thrust = zeros(1,length(times)-1);

		for i = 1:length(controlSol)
			[Xdot,A]= pathObj.SEPForces.ComputeForcesGivenControl(times(i),config.StateUnScaleMat*stateSol(i,:)',controlSol(i,:)');
			nThruster(i) = pathObj.SEPForces.Spacecraft.ThrusterSet.numToFire;
			power_in(i) = traj.pathFunctionObject.SEPForces.Spacecraft.GetThrustPower();
% 			net_mdot(i) = -pathObj.SEPForces.Spacecraft.ThrusterSet.net_mdot*1e6;
% 			net_thrust(i) = abs(pathObj.SEPForces.Spacecraft.ThrusterSet.net_thrust)*1e4;
		end
	
		figure(3); hold on;
		plot(timeVec,nThruster')
		plot(timeVec,power_in')
% 		plot(times(1:length(times)-1),net_mdot')
% 		plot(times(1:length(times)-1),net_thrust')
    end
    
    
%     SphericalControlCAESARNewGuessFunction(times,timeTypeVec,traj.phaseList{phaseIdx}.GetNumControlVars(),1);
    
	figure(1); hold on;
    fac = .3;
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    if traj.phaseList{phaseIdx}.GetNumControlVars()
        lineWidth = 2;
        lineColor = 'b';
    else
        lineWidth = 2;
        lineColor = 'b';
    end
    plot3(stateSol(:,1),stateSol(:,2),stateSol(:,3),lineColor,'LineWidth',lineWidth);
    for i = 1:length(controlSol)
        startX = stateSol(i,1);
        startY = stateSol(i,2);
        startZ = stateSol(i,3);
        
%         dir1 = controlSol(i,3) * sin(controlSol(i,1)) * cos(controlSol(i,2));
%         dir2 = controlSol(i,3) * sin(controlSol(i,1)) * sin(controlSol(i,2));
%         dir3 = controlSol(i,3) * cos(controlSol(i,1));
        dir1 = controlSol(i,1);
        dir2 = controlSol(i,2);
        dir3 = controlSol(i,3);
        
        endX = stateSol(i,1) + fac*dir1;
        endY = stateSol(i,2) + fac*dir2;
        endZ = stateSol(i,3) + fac*dir3;
        plot3([startX;endX],[startY;endY],[startZ;endZ],'r-');
    end
    axis([-3 3 -3 3 -3 3])
    xlab = xlabel('Heliocentric X, AU');
    ylab = ylabel('Heliocentric Y, AU');
    set(xlab,'fontsize',15)
    set(ylab,'fontsize',15)
	
    if isa(traj.phaseList{phaseIdx},'exec.RadauPhase') 
        timeVec = times(1:end-1)*config.TU/86400/365.25;
    else
        timeVec = times(1:end)*config.TU/86400/365.25;
    end
	
	figure(2); hold on;
	if isa(traj.phaseList{phaseIdx},'RadauPhase') && traj.phaseList{phaseIdx}.GetNumControlVars()
% 	    plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,1),'DisplayName',['Phase ',num2str(phaseIdx),' x-control'])
% 	    plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,2),'DisplayName',['Phase ',num2str(phaseIdx),' y-control'])
% 	    plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,3),'DisplayName',['Phase ',num2str(phaseIdx),' z-control'])
	    plot(timeVec,controlSol(:,1),'DisplayName',['Phase ',num2str(phaseIdx),' x-control'])
	    plot(timeVec,controlSol(:,2),'DisplayName',['Phase ',num2str(phaseIdx),' y-control'])
	    plot(timeVec,controlSol(:,3),'DisplayName',['Phase ',num2str(phaseIdx),' z-control'])
	end
	
	figure(4); hold on;
	plot(times*config.TU/86400/365.25,stateSol(:,7)*config.MU,'k','linewidth',2)
    
    

end

figure(4)
% lg = legend('EMTG initial guess','GMAT optimal control');
% set(lg,'fontsize',15)
xlab = xlabel('Time since launch, years');
ylab = ylabel('Wet Mass, kg');
set(xlab,'fontsize',15)
set(ylab,'fontsize',15)

figure(2)
lg = legend('x control','y control','z control');
set(lg,'fontsize',15,'location','eastoutside')
xlab = xlabel('Time since launch, years');
% ylab = ylabel('rad or pct');
ylab = ylabel('pct');
set(xlab,'fontsize',15)
set(ylab,'fontsize',15)
    
    
%figure(2)
%legend on
figure(1)
cnt = 0;
clear earthPos astPos
for time = 0:0.01:7
    mjd = config.TimeOffSet + time*365.25;
    cnt = cnt + 1;

    [stateEarth] = cspice_spkezr('EARTH', ...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateMars] = cspice_spkezr('MARS',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateCeres] = cspice_spkezr('2000001',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateVesta] = cspice_spkezr('2000004',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    earthPos(cnt,:) = stateEarth(1:3)/config.DU;
    marsPos(cnt,:)  = stateMars(1:3)/config.DU;
    ceresPos(cnt,:) = stateCeres(1:3)/config.DU;
    vestaPos(cnt,:) = stateVesta(1:3)/config.DU;
end
hold on
plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
plot3(marsPos(:,1),marsPos(:,2),marsPos(:,3),'k-')
plot3(ceresPos(:,1),ceresPos(:,2),ceresPos(:,3),'k-')
plot3(vestaPos(:,1),vestaPos(:,2),vestaPos(:,3),'k-')
view([0 0 1])
axis equal
axis([-3 3 -3 3 -3 3])
grid on
