%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar auxdata bounds guess traj
import ShuttleReEntryPathObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names 
traj                    = Trajectory();
traj.pathFunctionObject  = ShuttleReEntryPathObject();
traj.pointFunctionObject = ShuttleReEntryPointObject();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 10;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

cft2m = 0.3048;
cft2km = cft2m/1000;
cslug2kg = 14.5939029;
%-------------------------------------%
%             Problem Setup           %
%-------------------------------------%
auxdata.Re = 20902900*cft2m;              % Equatorial Radius of Earth (m)
auxdata.S  = 2690*cft2m^2;                % Vehicle Reference Area (m^2)
auxdata.cl(1) = -0.2070;                  % Parameters for lift coefficient
auxdata.cl(2) = 1.6756;       
auxdata.cd(1) = 0.0785;       
auxdata.cd(2) = -0.3529;       
auxdata.cd(3) = 2.0400;
auxdata.b(1)  = 0.07854;      
auxdata.b(2)  = -0.061592;    
auxdata.b(3)  = 0.00621408;
auxdata.H     = 23800*cft2m;              % Density Scale Height (m)
auxdata.al(1) = -0.20704;    
auxdata.al(2) = 0.029244;
auxdata.rho0  = 0.002378*cslug2kg/cft2m^3;% Sea Level Atmospheric Density (slug/ft^3)
auxdata.mu    = 1.4076539e16*cft2m^3;     % Earth Gravitational Parameter (ft^^3/s^2) 
auxdata.mass  = 6309.433*cslug2kg;      

% inital conditions
t0 = 0;
alt0 = 260000*cft2m;
rad0 = alt0+auxdata.Re;
lon0 = 0;
lat0 = 0;
speed0 = 25600*cft2m;
fpa0   = -1*pi/180;
azi0   = 90*pi/180;

% terminal conditions
altf = 80000*cft2m;
radf = altf+auxdata.Re;
speedf = 2500*cft2m;
fpaf   = -5*pi/180;
azif   = -90*pi/180;

%----------------------------------------------------%
% Lower and Upper Limits on Time, State, and Control %
%----------------------------------------------------%
tfMin = 0;            tfMax = 3000;
radMin = auxdata.Re;  radMax = rad0;
lonMin = -pi;         lonMax = -lonMin;
latMin = -70*pi/180;  latMax = -latMin;
speedMin = 10;        speedMax = 45000;
fpaMin = -80*pi/180;  fpaMax =  80*pi/180;
aziMin = -180*pi/180; aziMax =  180*pi/180;
aoaMin = -90*pi/180;  aoaMax = -aoaMin;
bankMin = -90*pi/180; bankMax =   1*pi/180;

bounds.phase.initialtime.lower = t0;
bounds.phase.initialtime.upper = t0;
bounds.phase.finaltime.lower = tfMin; 
bounds.phase.finaltime.upper = tfMax;
bounds.phase.initialstate.lower = [rad0, lon0, lat0, speed0, fpa0, azi0];
bounds.phase.initialstate.upper = [rad0, lon0, lat0, speed0, fpa0, azi0];
bounds.phase.state.lower = [radMin, lonMin, latMin, speedMin, fpaMin, aziMin];
bounds.phase.state.upper = [radMax, lonMax, latMax, speedMax, fpaMax, aziMax];
bounds.phase.finalstate.lower = [radf, lonMin, latMin, speedf, fpaf, aziMin];
bounds.phase.finalstate.upper = [radf, lonMax, latMax, speedf, fpaf, aziMax];
bounds.phase.finalstate.upper = [radf, lonMax, latMax, speedf, fpaf, aziMax];
bounds.phase.control.lower = [aoaMin, bankMin];
bounds.phase.control.upper = [aoaMax, bankMax];

%----------------------%
% Set up Initial Guess %
%----------------------%
tGuess = [0; 1000];
radGuess = [rad0; radf];
lonGuess = [lon0; lon0+10*pi/180];
latGuess = [lat0; lat0+10*pi/180];
speedGuess = [speed0; speedf];
fpaGuess = [fpa0; fpaf];
aziGuess = [azi0; azif];
aoaGuess = [0; 0];
bankGuess = [0; 0];

guess.phase.state   = [radGuess, lonGuess, latGuess, speedGuess, fpaGuess, aziGuess];
guess.phase.control = [aoaGuess, bankGuess];
guess.phase.time    = tGuess;

phase1 = RadauPhase;
phase1.initialGuessMode = 'LinearUnityControl';
meshIntervalFractions = -1:.2:1;
meshIntervalNumPoints = 4*ones(length(meshIntervalFractions)-1,1);
%  Set time properties
initialGuessTime = guess.phase.time(1);
finalGuessTime = guess.phase.time(2);
timeLowerBound = bounds.phase.initialtime.lower';
timeUpperBound = bounds.phase.initialtime.upper';
%  Set state state properties
numStateVars = length(bounds.phase.state.lower)';
stateLowerBound = bounds.phase.state.lower';
initialGuessState = guess.phase.state(1,:)';
finalGuessState = guess.phase.state(2,:)';
stateUpperBound = bounds.phase.state.upper';

%  Set control properties
numControlVars    = length(bounds.phase.control.upper);
controlUpperBound = bounds.phase.control.upper';
controlLowerBound = bounds.phase.control.lower';

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
%traj.Initialize();
[z,F,xmul,Fmul] = traj.Optimize();

stateSol = traj.phaseList{1}.DecVector.GetStateArray();
controlSol = traj.phaseList{1}.DecVector.GetControlArray();
times = traj.phaseList{1}.TransUtil.GetTimeVector();
figure(1)
subplot(3,2,1)
plot(times,stateSol(:,1))
subplot(3,2,2)
plot(times,stateSol(:,2))
subplot(3,2,3)
plot(times,stateSol(:,3))
subplot(3,2,4)
plot(times,stateSol(:,4))
subplot(3,2,5)
plot(times,stateSol(:,5))
subplot(3,2,6)
plot(times,stateSol(:,6))
grid on


