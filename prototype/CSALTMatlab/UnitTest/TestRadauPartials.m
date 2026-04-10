ClearAll
global igrid iGfun jGvar traj 

%% =====  Define the phase and trajectory properties

%  Create trajectory and its settings
traj                           = Trajectory();
traj.derivativeMethod          = 'FiniteDiff';
traj.scalingMethod             = 'None';
traj.pathFunctionName          = 'HyperSensitivePathFunction';   
traj.pointFunctionName         = 'HyperSensitivePointFunction';
traj.plotFunctionName          = 'HyperSensitivePlotFunction'; 
traj.showPlot                  = false();
traj.costLowerBound            = 0;
traj.costUpperBound            = Inf;
traj.plotUpdateRate            = 3;

%%  Configure Phase 1
phase1                         = RadauPhase;
phase1.initialGuessMode        = 'LinearNoControl';
%phase1.meshIntervalFractions   = linspace(-1,1,40+1).';
%phase1.meshIntervalNumPoints   = 4*ones(40,1);
phase1.meshIntervalFractions   = [ -1 -0.25 0 0.25  1];                
phase1.meshIntervalNumPoints   = [15 5 5 15]';
%  Time configuration
phase1.timeLowerBound          = 0;
phase1.initialGuessTime        = 0;
phase1.finalGuessTime          = 50;
phase1.timeUpperBound          = 50;
%  State configuration
phase1.numStateVars            = 1;
phase1.stateLowerBound         = -50;
phase1.initialGuessState       = 1;
phase1.finalGuessState         = 1;
phase1.stateUpperBound         = 50;
%  Control configuration
phase1.controlUpperBound       = 50;
phase1.controlLowerBound       = -50;
phase1.numControlVars          = 1;
traj.phaseList = {phase1};
traj.Initialize();

load DebugRadau.mat
JAutomatic = J;
traj.SetDecisionVector(z,true);
C = traj.GetCostConstraintFunctions(true);
JAnalytic = traj.GetJacobian(true);

traj.SetDecisionVector(z)
traj.GetCostConstraintFunctions(true);

jCostAnalytic = JAnalytic(1,:);
jConAnalytic = JAnalytic(2:end,:);
jCostAutomatic = J(1,:);
jConAutomatic = J(2:end,:);


jConAutomatic - jConAnalytic
figure(1)
spy(JAutomatic)
figure(2)
spy(JAnalytic)


return

traj.phaseList{1}.costJac - J(1,:)
traj.phaseList{1}.jacArray - J(2:end,:)
figure(1)
spy(traj.phaseList{1}.jacArray)
figure(2)
spy(J(2:end,:))


figure(1)
spy(J(2:end,:))
figure(2)
spy(traj.phaseList{1}.jacArray)
figure(3)
spy(J(1,:))
figure(4)
spy(traj.phaseList{1}.costJac)

traj.phaseList{1}.jacArray(79:80,117:118) 
J(80:81,117:118)
return

ClearAll
OrbitRaising_Main
J = traj.GetJacobian();
traj.phaseList{1}.costJac - J(1,:)
traj.phaseList{1}.jacArray - J(2:end,:)

return
%load RadauParitialsDebugData_HS.mat
traj.phaseList{1}.SetDerivativeMethod(true);
z = traj.phaseList{1}.DecVector.decisionVector.x;
traj.SetDecisionVector(z);
traj.GetCostConstraintFunctions();
fdCostJac = traj.phaseList{1}.costJac;
J =traj.GetJacobian;
adCostJac = J(1,:);

fdConJac = traj.phaseList{1}.jacArray;
adConJac = J(2:end,:);

fdCostJac - adCostJac
fdConJac - adConJac



