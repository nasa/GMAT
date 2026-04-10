

%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global traj configEarth configSun initialState propDuration phaseList
global scaleUtil
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase
thePathObject = LISAPathObject();
thePointObject = LISAPointObject();

%==========================================================================
%% =====  Define Non-Dimensionalization Properties
%==========================================================================

%=== Quanitities used in non-dimensionalization and re-dimensionalization

%----  Earth Scaling
configEarth.gravParam = 398600.4415;
configEarth.RE  = 6378.1363;
configEarth.DU  = 42000;
configEarth.TU  = sqrt(configEarth.DU^3/configEarth.gravParam);
configEarth.ACCU = configEarth.TU^2/configEarth.DU;
configEarth.MU  = 1000;
configEarth.VU = configEarth.DU/configEarth.TU;
configEarth.StateScaleMat = diag([1/configEarth.DU; ones(5,1); 1/configEarth.MU]);
configEarth.StateAccScaleMat = configEarth.TU*configEarth.StateScaleMat;
configEarth.StateUnScaleMat = inv(configEarth.StateScaleMat);
configEarth.StateAccUnScaleMat = inv(configEarth.StateAccScaleMat);

configSun.gravParam = 132712439935.4841;
configSun.DU  = 149597870.7;
configSun.TU  = sqrt(configSun.DU^3/configSun.gravParam);
configSun.ACCU = configSun.TU^2/configSun.DU;
configSun.MU  = 1000;
configSun.VU = configSun.DU/configSun.TU;
configSun.StateScaleMat = diag([1/configSun.DU; ones(5,1); 1/configSun.MU]);
configSun.StateAccScaleMat = configSun.TU*configSun.StateScaleMat;
configSun.StateUnScaleMat = inv(configSun.StateScaleMat);
configSun.StateAccUnScaleMat = inv(configSun.StateAccScaleMat);

%  Configure the scale utility
scaleUtil = ScaleUtility();
scaleUtil.earthScaling = configEarth;
scaleUtil.sunScaling = configSun;

%===============================================================
%==========================================================================
%% ===== USER INPUTS: Define Initial and Final State Values
%==========================================================================

thePointObject.SetInitialMass(1900);
thePointObject.SetInitialConditionsByName('AlternateLaunch-30DayCheckout')
thePointObject.SetFinalConditionsByName('ESA-LISA3Nominal')

refEpoch = thePointObject.GetInitialEpoch();%34050.0208306428;
refEpochTDB = timeconv(refEpoch,'a1','tdb');%34050.02120276393;

%==========================================================================
%% =====  Non-dimensionalize and set user data
%==========================================================================

%==========================================================================
%% =====  Configure the force model
%==========================================================================
ForceModelConfig
thePathObject.ForceConfig = theForceModel;

%==========================================================================
%% =====  Define Trajectory Properties
%==========================================================================
traj = Trajectory();
traj.pathFunctionObject  = thePathObject;
traj.pointFunctionObject = thePointObject;
traj.plotFunctionName  = 'LISAPlotFunction';
traj.showPlot = false();
traj.plotUpdateRate = 5;
traj.costLowerBound = -Inf;
traj.costUpperBound = Inf;
traj.maxMeshRefinementCount = 30;
traj.refineInitialMesh = true;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================
phase1 = RadauPhase;
phase1.initialGuessMode = 'UserGuessFunction';
meshIntervalFractions = -1:.1:1;
meshIntervalNumPoints = 5*(ones(length(meshIntervalFractions)-1,1));
meshIntervalFractions = [-1:.3:1,1]
meshIntervalFractions = linspace(-1,1,2)
meshIntervalNumPoints = 3*(ones(length(meshIntervalFractions)-1,1));

%===  Set time properties
phase1.centralBody = 1;
phase1.refEpoch = refEpoch;
phase1.phaseName = 'LISAPhase1';
initialA1ModJulianEpoch = 34050.99962787928;
finalA1ModifiedJulianEoch = thePointObject.GetFinalEpoch();
initialGuessTime  = scaleUtil.ScaleTime(initialA1ModJulianEpoch,phase1.centralBody,phase1.refEpoch);
finalGuessTime    = scaleUtil.ScaleTime(finalA1ModifiedJulianEoch,phase1.centralBody,phase1.refEpoch);
timeLowerBound    = scaleUtil.ScaleTime(initialA1ModJulianEpoch-1000,phase1.centralBody,phase1.refEpoch);
timeUpperBound    = scaleUtil.ScaleTime(finalA1ModifiedJulianEoch+1000,phase1.centralBody,phase1.refEpoch);

%===  Set state state properties
numStateVars      = 7;
stateLowerBound   = [0 -100 -100 -100 -100 -100 -100]';
%  Warning: the values for state guesses assume you are NOT
% using phase1.initialGuessMode = 'UserGuessFunction';
% Changing that mode will result failure to convert
initialGuessState = stateLowerBound;
finalGuessState   = [ 100 100 100 100 100 100 100]'
stateUpperBound   = finalGuessState

%===  Set control properties
numControlVars    = 4;
controlUpperBound = [1  2  2  2]';
controlLowerBound = [0 -2 -2 -2]';

phase1.SetRefineInitialMesh(true);
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

% Configure the list of phases.
phaseList = {phase1};

% Add the scale utility to all phases
for phaseIdx = 1:length(phaseList)
    phaseList{phaseIdx}.scaleUtil = scaleUtil;
end

thePointObject.SetScaleUtil(scaleUtil);
thePathObject.SetScaleUtil(scaleUtil);

thePathObject.phaseList = phaseList;
thePointObject.phaseList = phaseList;

traj.phaseList = {phase1};
traj.guessFunctionName = 'LISAGuessFunction';
[z,F,xmul,Fmul] = traj.Optimize();

PlotSolution(traj,'g-');
