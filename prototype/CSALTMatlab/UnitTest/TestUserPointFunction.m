%clear all
%clear classes
global useAnalyticJac

%% Create an object that finite differences Jacobian
fdFunc = UserPointFunction;
fdFunc.functionName = 'OrbitRaisingPointFunction_HS';
useAnalyticJac = false();
initStateVec = [2 3 4 5 6]';
finalStateVec = [8 9 10 11 12]';
initTime = .3;
finalTime = 9;
numStates = length(initStateVec);
fdFunc.SetParameterDimensions(numStates);
fdFunc.SetInitialState(initStateVec);
fdFunc.SetFinalState(finalStateVec);
fdFunc.SetInitialTime(initTime);
fdFunc.SetFinalTime(finalTime);
fdFunc.Initialize(numStates);
fdFunc.Evaluate()

%  The object uses analytic Jacobian
udFunc = UserPointFunction;
udFunc.functionName = 'OrbitRaisingPointFunction_HS';
useAnalyticJac = true();
udFunc.SetParameterDimensions(numStates);
udFunc.SetInitialState(initStateVec);
udFunc.SetFinalState(finalStateVec);
udFunc.SetInitialTime(initTime);
udFunc.SetFinalTime(finalTime);
udFunc.Initialize(numStates);
udFunc.Evaluate()

udFunc.boundFuncInitTimeJac - fdFunc.boundFuncInitTimeJac
udFunc.boundFuncFinalTimeJac - fdFunc.boundFuncFinalTimeJac
udFunc.boundFuncInitStateJac - fdFunc.boundFuncInitStateJac
udFunc.boundFuncFinalStateJac - fdFunc.boundFuncFinalStateJac

return

%% Create an object that finite differences Jacobian
fdFunc = UserPointFunction;
fdFunc.functionName = 'ToyPointFunction';
useAnalyticJac = false();
initStateVec = [2.12314145 -3.2314 3 ]';
finalStateVec = [0.12314145 6.2314 -8 ]';
initTime = .3;
finalTime = 9;
numStates = length(initStateVec);
fdFunc.SetParameterDimensions(numStates);
fdFunc.SetInitialState(initStateVec);
fdFunc.SetFinalState(finalStateVec);
fdFunc.SetInitialTime(initTime);
fdFunc.SetFinalTime(finalTime);
fdFunc.Initialize(numStates);
fdFunc.Evaluate()

%  The object uses analytic Jacobian
udFunc = UserPointFunction;
udFunc.functionName = 'ToyPointFunction';
useAnalyticJac = true();
udFunc.SetParameterDimensions(numStates);
udFunc.SetInitialState(initStateVec);
udFunc.SetFinalState(finalStateVec);
udFunc.SetInitialTime(initTime);
udFunc.SetFinalTime(finalTime);
udFunc.Initialize(numStates);
udFunc.Evaluate()

udFunc.boundFuncInitTimeJac - fdFunc.boundFuncInitTimeJac
udFunc.boundFuncFinalTimeJac - fdFunc.boundFuncFinalTimeJac
udFunc.boundFuncInitStateJac - fdFunc.boundFuncInitStateJac
udFunc.boundFuncFinalStateJac - fdFunc.boundFuncFinalStateJac


%  Test Jacobian 
stateLowerBound = ones(3,1)*(-15);
stateUpperBound = ones(3,1)*(15);
timeLowerBound = -10;
timeUpperBound = 30;
udFunc.ComputeStateSparsity(stateUpperBound,stateLowerBound)
udFunc.ComputeTimeSparsity(timeUpperBound,timeLowerBound)

udFunc.boundFuncInitStateJacPattern
udFunc.boundFuncFinalStateJacPattern
udFunc.boundFuncInitTimeJacPattern
udFunc.boundFuncFinalTimeJacPattern

%%  Test a sparse problem example
%  Configure the problem
spFunc = UserPointFunction;
spFunc.functionName = 'ToyPointFunctionSparsityTest';
useAnalyticJac = false();
initStateVec = [2.12314145 -3.2314 3 ]';
finalStateVec = [0.12314145 6.2314 -8 ]';
initTime = .3;
finalTime = 9;
spFunc.SetParameterDimensions(numStates);
spFunc.SetInitialState(initStateVec);
spFunc.SetFinalState(finalStateVec);
spFunc.SetInitialTime(initTime);
spFunc.SetFinalTime(finalTime);
spFunc.Initialize(numStates);
spFunc.ComputeStateSparsity(stateUpperBound,stateLowerBound)
spFunc.ComputeTimeSparsity(timeUpperBound,timeLowerBound)
%  Compute sparsity and compare to truth
truth = [0 1 0;1 1 1; 1 1 0];
spFunc.boundFuncInitStateJacPattern - truth
truth = [0 1 0;1 1 1;0 1 0];
spFunc.boundFuncFinalStateJacPattern - truth
truth = [1 0 1]';
spFunc.boundFuncInitTimeJacPattern - truth
truth = [1 1 0]';
spFunc.boundFuncFinalTimeJacPattern - truth
