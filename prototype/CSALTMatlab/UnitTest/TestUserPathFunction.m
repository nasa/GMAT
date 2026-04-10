clear all
clear classes
global useAnalyticJac

% %% Sparsity determination test
% fdFunc = UserPathFunction;
% fdFunc.functionName = 'ToyPathFunctionSpasityTest';
% useAnalyticJac = false();
% stateVec = [2.12314145 -3.2314 3 4 5 6]'
% controlVec = [-0.12314145 1.2314]';
% stateLowerBound = ones(6,1)*(-15)
% stateUpperBound = ones(6,1)*(15)
% time = .3;
% numStates = length(stateVec);
% numControls = length(controlVec)
% controlLowerBound = [-4 -4]';
% controlUpperBound = [4 4]';
% fdFunc.SetParameterDimensions(numStates,numControls)
% fdFunc.SetStateControlTime(stateVec,controlVec,time);
% fdFunc.Initialize(numStates,numControls);
% fdFunc.Evaluate()
% fdFunc.ComputeStateSparsity(stateLowerBound,stateUpperBound);
% fdFunc.ComputeControlSparsity(controlLowerBound,controlUpperBound);
% fdFunc.ComputeTimeSparsity(-3,5);
% 
% %%  Finite differencing of Jacobian test
% % Create function object and intialize it
% fdFunc = UserPathFunction;
% fdFunc.functionName = 'ToyPathFunction';
% useAnalyticJac = false();
% stateVec = [2.12314145 -3.2314]';
% controlVec = [-0.12314145 1.2314]';
% time = .3;
% numStates = length(stateVec);
% numControls = length(controlVec);
% fdFunc.SetParameterDimensions(numStates,numControls)
% fdFunc.SetStateControlTime(stateVec,controlVec,time);
% fdFunc.Initialize(numStates,numControls);
% fdFunc.Evaluate()
% 
% %  Evaluate finite diff and analytic partials
% useAnalyticJac = true();
% udFunc = UserPathFunction;
% udFunc.functionName = 'ToyPathFunction';
% udFunc.SetParameterDimensions(numStates,numControls)
% udFunc.SetStateControlTime(stateVec,controlVec,time);
% udFunc.Initialize(numStates,numControls);
% udFunc.Evaluate()
% 
% udFunc.dynFuncStateJac - fdFunc.dynFuncStateJac
% udFunc.algFuncStateJac - fdFunc.algFuncStateJac
% udFunc.costFuncStateJac - fdFunc.costFuncStateJac
% 
% udFunc.dynFuncControlJac - fdFunc.dynFuncControlJac
% udFunc.algFuncControlJac - fdFunc.algFuncControlJac
% udFunc.costFuncControlJac - fdFunc.costFuncControlJac
% 
% udFunc.dynFuncTimeJac - fdFunc.dynFuncTimeJac
% udFunc.algFuncTimeJac - fdFunc.algFuncTimeJac
% udFunc.costFuncTimeJac - fdFunc.costFuncTimeJac
% 
% stateUpperBound = [10 10]';
% stateLowerBound = [-10 -10]';
% 
% udFunc.ComputeStateSparsity(stateLowerBound,stateUpperBound);
% udFunc.ComputeControlSparsity(stateLowerBound,stateUpperBound);
% udFunc.ComputeTimeSparsity(-3,5);
% return
% 
% %%  Finite differencing of Jacobian test for Toy Problem
% fdFunc = UserPathFunction;
% fdFunc.functionName = 'ToyHSProblem';
% useAnalyticJac = false();
% stateVec = [.96345];
% controlVec = [.34156];
% time = .3;
% numStates = length(stateVec);
% numControls = length(controlVec);
% fdFunc.SetParameterDimensions(numStates,numControls)
% fdFunc.SetStateControlTime(stateVec,controlVec,time);
% fdFunc.Initialize(numStates,numControls);
% fdFunc.Evaluate()
% 
% %  Evaluate finite diff and analytic partials
% useAnalyticJac = true();
% udFunc = UserPathFunction;
% udFunc.functionName = 'ToyHSProblem';
% udFunc.SetParameterDimensions(numStates,numControls)
% udFunc.SetStateControlTime(stateVec,controlVec,time);
% udFunc.Initialize(numStates,numControls);
% udFunc.Evaluate()
% 
% udFunc.dynFuncStateJac - fdFunc.dynFuncStateJac
% %udFunc.algFuncStateJac - fdFunc.algFuncStateJac
% udFunc.costFuncStateJac - fdFunc.costFuncStateJac
% 
% udFunc.dynFuncControlJac - fdFunc.dynFuncControlJac
% %udFunc.algFuncControlJac - fdFunc.algFuncControlJac
% udFunc.costFuncControlJac - fdFunc.costFuncControlJac
% 
% udFunc.dynFuncTimeJac - fdFunc.dynFuncTimeJac
% %udFunc.algFuncTimeJac - fdFunc.algFuncTimeJac
% udFunc.costFuncTimeJac - fdFunc.costFuncTimeJac

%%  Finite differencing of Jacobian test for OrbitRaising
fdFunc = UserPathFunction;
fdFunc.functionName = 'OrbitRaisingPathFunction_HS';
useAnalyticJac = false();
stateVec = [1 2 3 4 5]';
controlVec = [-2 -3]';
time = .3;
numStates = length(stateVec);
numControls = length(controlVec);
fdFunc.SetParameterDimensions(numStates,numControls)
fdFunc.SetStateControlTime(stateVec,controlVec,time);
fdFunc.Initialize(numStates,numControls);
fdFunc.Evaluate()

%  Evaluate finite diff and analytic partials
%useAnalyticJac = true();
udFunc = UserPathFunction;
udFunc.functionName = 'OrbitRaisingPathFunction_HS';
udFunc.SetParameterDimensions(numStates,numControls)
udFunc.SetStateControlTime(stateVec,controlVec,time);
udFunc.Initialize(numStates,numControls);
udFunc.Evaluate()

udFunc.dynFuncStateJac - fdFunc.dynFuncStateJac
udFunc.algFuncStateJac - fdFunc.algFuncStateJac
%udFunc.costFuncStateJac - fdFunc.costFuncStateJac

udFunc.dynFuncControlJac - fdFunc.dynFuncControlJac
udFunc.algFuncControlJac - fdFunc.algFuncControlJac
%udFunc.costFuncControlJac - fdFunc.costFuncControlJac

udFunc.dynFuncTimeJac - fdFunc.dynFuncTimeJac
udFunc.algFuncTimeJac - fdFunc.algFuncTimeJac
%udFunc.costFuncTimeJac - fdFunc.costFuncTimeJac

%return





