%==========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
global igrid iGfun jGvar traj
import RayleighPathObject
import RayleighPointObject
import exec.Trajectory
import exec.RadauPhase

%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                    = Trajectory();
traj.pathFunctionObject  = RayleighPathObject();
traj.pointFunctionObject = RayleighPointObject();
traj.showPlot           = false();
traj.plotUpdateRate     = 2;
traj.costLowerBound     = -Inf;
traj.costUpperBound     = Inf;
traj.maxMeshRefinementCount = 7;

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearNoControl';
meshIntervalFractions   = -1:.25:1;
meshIntervalNumPoints   = 3*ones(length(meshIntervalFractions)-1,1);
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 4.5;
timeLowerBound    = 0;
timeUpperBound    = 4.5;
%  Set state state properties
numStateVars      = 2;
stateLowerBound   = [-10 -10]';
initialGuessState = [-5 -5]';
finalGuessState   = [0 0]';
stateUpperBound   = [10 10]';
%  Set control properties
numControlVars    = 1;
controlUpperBound = [1]';
controlLowerBound = [-1]';

phase1.SetNumStateVars(numStateVars);
phase1.SetNumControlVars(numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions);
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBound);
phase1.SetStateUpperBound(stateUpperBound);
phase1.SetStateInitialGuess(initialGuessState);
phase1.SetStateFinalGuess(finalGuessState);
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
timesSol = traj.phaseList{1}.TransUtil.GetTimeVector()
for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    figure(1);
    subplot(2,1,1)
    hold on; grid on;
    plot(times,stateSol(:,1),times,stateSol(:,2))
    title('state history')
    subplot(2,1,2)
    hold on; grid on;
    if isa(phase1,'RadauPhase')
        plot(times(1:end-1),controlSol(:,1))
    else
        plot(times,controlSol(:,1))
    end
    title('control history')
end

bettsTimes = [0.0000000E+00
6.2500000E-02
1.2500000E-01
2.5000000E-01
3.7500000E-01
5.0000000E-01
5.8333333E-01
6.6666667E-01
7.5000000E-01
8.3333333E-01
9.1666667E-01
1.0000000E+00
1.0416667E+00
1.0833333E+00
1.1666667E+00
1.3333333E+00
1.5000000E+00
1.5833333E+00
1.6666667E+00
1.6944444E+00
1.7222222E+00
1.7500000E+00
2.0000000E+00
2.2500000E+00
2.5000000E+00
2.7500000E+00
2.8333333E+00
2.8750000E+00
2.9166667E+00
3.0000000E+00
3.2500000E+00
3.5000000E+00
4.0000000E+00
4.5000000E+00
]

bettsStates =[ -5.0000000E+00 -5.0000000E+00
-5.2798925E+00 -4.0251971E+00
-5.5086923E+00 -3.3249611E+00
-5.8527922E+00 -2.2154138E+00
-6.0650506E+00 -1.1726473E+00
-6.1407134E+00 -6.7635553E-03
-6.1047430E+00  8.8572302E-01
-5.9910457E+00  1.8494403E+00
-5.7970194E+00  2.7928036E+00
-5.5298982E+00  3.5824216E+00
-5.2067210E+00  4.1319871E+00
-4.8476475E+00  4.4525106E+00
-4.6600271E+00  4.5472903E+00
-4.4692431E+00  4.6036135E+00
-4.0841110E+00  4.6232453E+00
-3.3234125E+00  4.4776249E+00
-2.5986153E+00  4.1985088E+00
-2.2565875E+00  4.0023778E+00
-1.9330314E+00  3.7530165E+00
-1.8301124E+00  3.6559195E+00
-1.7299798E+00  3.5539698E+00
-1.6326038E+00  3.4583216E+00
-8.5632677E-01  2.7931163E+00
-2.2726289E-01  2.2421521E+00
 2.6137512E-01  1.6516498E+00
 5.8638659E-01  9.1383214E-01
 6.5023115E-01  6.1273266E-01
 6.7237574E-01  4.4859380E-01
 6.8755045E-01  2.8201560E-01
 6.9868513E-01 -4.5494280E-03
 6.2120906E-01 -5.4271272E-01
 4.5712492E-01 -7.2157023E-01
 1.3059346E-01 -4.9987486E-01
 0.0000000E+00  0.0000000E+00
];

bettsControl = [  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  1.0000000E+00
  9.0940130E-01
  7.4655517E-01
  4.1696382E-01
 -4.9906027E-02
 -3.8252556E-01
 -7.8751872E-01
 -9.3435836E-01
 -1.0000000E+00
 -1.0000000E+00
 -1.0000000E+00
 -1.0000000E+00
 -1.0000000E+00
 -1.0000000E+00
 -1.0000000E+00
 -1.0000000E+00
 -8.7943797E-01
 -5.9251204E-01
  5.2687071E-03
  3.1061940E-01
  4.1309329E-01
  2.7769295E-01
]

interpStates = interp1(timesSol,stateSol,bettsTimes,'spline')
interpControls = interp1(timesSol(1:end-1),controlSol,bettsTimes(1:end-1),'spline')
interpStates - bettsStates
interpControls - bettsControl(1:end-1)

1 - interpStates./bettsStates
1 - interpControls./bettsControl(1:end-1)

plot(timesSol(1:end-1),controlSol);hold on
plot(bettsTimes,bettsControl,'k*')