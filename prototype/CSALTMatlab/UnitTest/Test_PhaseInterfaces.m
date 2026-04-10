
OrbitRaisingTestConfig
%HyperSenstiveTestConfig

%==========================================================================
%% =====  Mimic how trajectory drives the phase class
%==========================================================================

% Configure the phase according to setup
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

% Set phase number, this is done by trajectory
phase1.SetPhaseNum(1);

%% This is code from Trajectory.InitializePhases()
phase1.pathFunctionObj = pathFuncObj;
phase1.Initialize();
numPhaseConstraints = phase1.Config.GetNumTotalConNLP();
numPhaseDecisionParams = phase1.Config.GetNumDecisionVarsNLP();
totalnumConstraints = phase1.Config.GetNumTotalConNLP();

%% This is code from Trajectory.PrepareToOptimize()
phase1.PrepareToOptimize()

stateGuess = phase1.DecVector.GetStateArray()
controlGuess = phase1.DecVector.GetControlArray()

%% Set the decision vector
phase1.SetDecisionVector(decVector)

%  This tests only computation of algebraic path constraints
if phase1.Config.GetHasAlgPathCons()
    phase1.ComputeAlgPathConstraints()
    [funcValues,jacValues] = phase1.algPathNLPFuncUtil.ComputeFuncAndJac(phase1.userAlgFuncData)
end

%% Now request the data that trajectory requests from phase

% Get function values
totalnumCons = phase1.GetNumTotalConNLP()
conVec = phase1.GetConstraintVector()
costFunc = phase1.GetCostFunction()

% Get the Jacobians and sparsity patterns
costJacobian = phase1.GetCostJacobian()
conJacobian = phase1.GetConJacobian()
costSparsity = phase1.GetCostSparsityPattern()
conSparsity = phase1.GetConSparsityPattern()
