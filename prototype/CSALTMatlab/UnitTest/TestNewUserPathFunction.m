%% ========================================================================
% This unit tester performs basic testing of the classes listed in the
% import statements below.
%% ========================================================================
import userfunutils.UserPathFunction
import userfunutils.FunctionInputData
import userfunutils.PathFunctionContainer
import userfunutils.UserPathFunctionManager
import userfunutils.FunctionOutputData

%  These are example user functions
import userfunutils.DummyPathFunction
import userfunutils.DummyPathFunction2

%% ========================================================================
% Unit test a simple user path function
%% ========================================================================

%  Create the path function container with new function data objects(cost, alg, dynamics)
funcData= PathFunctionContainer();
funcData.Initialize();

%  Create function input data
numStateVars = 3;
numControlVars = 2;
paramData = FunctionInputData();
paramData.Initialize(numStateVars,numControlVars);
paramData.SetStateVec([.1 .2 .3]');
paramData.SetControlVec([.4 .5]');
paramData.SetTime(0.5);
paramData.SetPhaseNum (1);
paramData.SetIsPerturbing(false());

%  Create bounds data needed for initialization
boundData.stateLowerBound = -[5 5 5]';
boundData.stateUpperBound =[5 5 5]';
boundData.controlLowerBound = -[5  5]';
boundData.controlUpperBound =[5 5]';
boundData.timeUpperBound = 10;
boundData.timeLowerBound = -10;

%  Create the user function object, intialize it, and test call to evaluate
pathFunc = userfunutils.DummyPathFunction();
pathFunc.Initialize(paramData,funcData);
[funcData] = pathFunc.EvaluateUserFunction(paramData,funcData);

%  Test function values (Jacobian is tested later in this file)
% === cost function data tests
disp(' The following test cases failed ' )
maxError = max(funcData.costData.GetFunctionValues()-0.0006);
if maxError > 1e-14
    disp('error in user cost function evaluation')
end
maxError = max(funcData.costData.GetNumFunctions()-1);
if maxError > 1e-14
    disp('error in number of user cost functions')
end
if ~funcData.costData.hasUserFunction
    disp('error in hasUserFunction flag for cost function')
end
if funcData.costData.GetHasUserStateJac
    disp('error in hasUserStateJac flag for cost function')
end
if funcData.costData.GetHasUserControlJac
    disp('error in hasUserControlJac flag for cost function')
end
if funcData.costData.GetHasUserTimeJac
    disp('error in hasUserTimeJac flag for cost function')
end
% === dynamics function unit tests
truthData = [-0.145054
    0.233982];
maxError = max(funcData.dynData.GetFunctionValues()-truthData);
if maxError > 1e-14
    disp('error in user cost function evaluation')
end
maxError = max(funcData.dynData.GetNumFunctions()-2);
if maxError > 1e-14
    disp('error in number of user dynamics functions')
end
if ~funcData.dynData.hasUserFunction
    disp('error in hasUserFunction flag for dynamics function')
end
if funcData.dynData.GetHasUserStateJac
    disp('error in hasUserStateJac flag for dynamics function')
end
if funcData.dynData.GetHasUserControlJac
    disp('error in hasUserControlJac flag for dynamics function')
end
if funcData.dynData.GetHasUserStateJac
    disp('error in hasUserTimeJac flag for dynamics function')
end
% === alg function unit tests
truthData = [-0.455700580773814
    0.735076132272247];
maxError = max(funcData.algData.GetFunctionValues()-truthData);
if maxError > 1e-14
    disp('error in user algebraic function evaluation')
end
maxError = max(funcData.dynData.GetNumFunctions()   -2);
if maxError > 1e-14
    disp('error in number of user algebraic functions')
end
if ~funcData.algData.hasUserFunction
    disp('error in hasUserFunction flag for algebraic function')
end
if ~funcData.algData.hasUserFunction
    disp('error in hasUserFunction flag for algebraic function')
end
if funcData.algData.GetHasUserStateJac
    disp('error in hasUserStateJac flag for algebraic function')
end
if funcData.algData.GetHasUserControlJac
    disp('error in hasUserControlJac flag for algebraic function')
end
if funcData.algData.GetHasUserTimeJac
    disp('error in hasUserTimeJac flag for algebraic function')
end

%% ========================================================================
% Unit test Intializing/calling a user path func via the function manager
%% ========================================================================

%  Create new user data container
clear funcData;

%  Create the path function container (cost, alg, dynamics) and intialize
%  it with the data objects create above
funcData = PathFunctionContainer();
funcData.Initialize();

%  Create function input data
numStateVars = 3;
numControlVars = 2;
paramData = FunctionInputData();
paramData.Initialize(numStateVars,numControlVars)
paramData.SetStateVec([.1 .2 .3]');
paramData.SetControlVec([.4 .5]');
paramData.SetTime(0.5);
paramData.SetPhaseNum (1);
paramData.SetIsPerturbing(false());

boundData.stateLowerBound = -[5 5 5]';
boundData.stateUpperBound =[5 5 5]';
boundData.controlLowerBound = -[5  5]';
boundData.controlUpperBound =[5 5]';
boundData.timeUpperBound = 10;
boundData.timeLowerBound = -10;

%  Create a new function manager
funcManager = UserPathFunctionManager();
pathFunc = DummyPathFunction();
funcManager.Initialize(pathFunc,paramData,funcData,boundData);

%  Test the interface called to evaluate functions
paramData.SetStateVec([.1 .2 .3]');
paramData.SetControlVec([.4 .5]');
paramData.SetTime(0.5);
funcData = funcManager.EvaluateUserJacobian(paramData,funcData);

%  Truth data for partials
algStateJac = [
    -0.00339292006587698  -0.000848230016469244  -0.00169646003293849
    -0.00169646003293849  -0.000282743338823081  -0.000376991118430775];

algControlJac = [
    -0.314159265358979        -0.376991118430775
    -0.376991118430775        -0.201061929829747];

algTimeJac = pi*[  -0.75
    1];

dynControlJac = [                      -0.1                     -0.12
    -0.12                    -0.064];

dynStateJac = [
    -0.00108                  -0.00027                  -0.00054
    -0.00054                    -9e-05                  -0.00012];
dynTimeJac = [  -0.75
    1];

costControlJac = [  0.0015 0.0012];
costStateJac = [ 0.006 0.003    0.002];
costTimeJac =  0.0012;

%  Test the partials. NOTE: need to have useAnalyticPartials set to false
%  in dummy function to test here!
% Dynamics partials
maxError = max(max(funcData.dynData.GetStateJac()- dynStateJac));
if maxError > 1e-8
    disp('error in finite difference dyn state Jacobian')
end
maxError = max(max(funcData.dynData.GetControlJac() - dynControlJac));
if maxError > 1e-8
    disp('error in finite difference dyn control Jacobian')
end
maxError = max(max(funcData.dynData.GetTimeJac() - dynTimeJac));
if maxError > 1e-7
    disp('error in finite difference dyn time Jacobian')
end
%  Alg fun partials
maxError = max(max(funcData.algData.GetStateJac()- algStateJac));
if maxError > 1e-8
    disp('error in finite difference alg state Jacobian')
end
maxError = max(max(funcData.algData.GetControlJac() - algControlJac));
if maxError > 1e-8
    disp('error in finite difference alg control Jacobian')
end
maxError = max(max(funcData.algData.GetTimeJac() - algTimeJac));
if maxError > 1e-6
    disp('error in finite difference alg time Jacobian')
end
%  Cost partials
maxError = max(max(funcData.costData.GetStateJac()- costStateJac));
if maxError > 1e-8
    disp('error in finite difference cost state Jacobian')
end
maxError = max(max(funcData.costData.GetControlJac() - costControlJac));
if maxError > 1e-8
    disp('error in finite difference cost control Jacobian')
end
maxError = max(max(funcData.costData.GetTimeJac() - costTimeJac));
if maxError > 1e-8
    disp('error in finite difference cost time Jacobian')
end

%% ========================================================================
%  Unit test the sparsity determination
%% ========================================================================

%  Create new user data container
clear funcData;

%  Create the path function container (cost, alg, dynamics) and intialize
%  it with the data objects create above
funcData= PathFunctionContainer();
funcData.Initialize();

%  Create function input data
numStateVars = 3;
numControlVars = 2;
paramData = FunctionInputData();
paramData.Initialize(numStateVars,numControlVars)
paramData.SetStateVec(2*[.1 .2 .3]');
paramData.SetControlVec(2*[.4 .5]');
paramData.SetTime(0.534156);
paramData.SetPhaseNum (2);

%  Set bounds data required for initialization
boundData.stateLowerBound = -[5 5 5]';
boundData.stateUpperBound =[5 5 5]';
boundData.controlLowerBound = -[ 5 5]'/2;
boundData.controlUpperBound =[ 5 5]'/2;
boundData.timeUpperBound = 10;
boundData.timeLowerBound = -10;

%  Create a new function manager and intialize it
funcManager = UserPathFunctionManager();
pathFunc = DummyPathFunction2();
funcManager.Initialize(pathFunc,paramData,funcData,boundData);
funcData = funcManager.EvaluateUserFunction(paramData,funcData);
dynFuncProps = funcManager.GetDynFuncProperties();
algFuncProps = funcManager.GetAlgFuncProperties();
costFuncProps = funcManager.GetCostFuncProperties();

%  === dynamics
truthData = [1 0 1; 1 1 0];
if ~isequal(truthData,dynFuncProps.GetStateJacPattern())
    disp('Error in dyn func state jacobian pattern')
end
truthData = [1 1; 0 1];
if ~isequal(truthData,dynFuncProps.GetControlJacPattern())
    disp('Error in dyn func control jacobian pattern')
end
truthData = [0;1];
if ~isequal(truthData,dynFuncProps.GetTimeJacPattern())
    disp('Error in dyn func control jacobian pattern')
end
%  === cost
truthData = [1 0 1];
if ~isequal(truthData,costFuncProps.GetStateJacPattern())
    disp('Error in cost func state jacobian pattern')
end
truthData = [1 0];
if ~isequal(truthData,costFuncProps.GetControlJacPattern())
    disp('Error in cost func control jacobian pattern')
end
truthData = 1;
if ~isequal(truthData,costFuncProps.GetTimeJacPattern())
    disp('Error in cost func control jacobian pattern')
end
%  === algebraic
truthData = [0 1 1; 1 0 1];
if ~isequal(truthData,algFuncProps.GetStateJacPattern())
    disp('Error in algebraic func state jacobian pattern')
end
truthData = [0 1; 1 0];
if ~isequal(truthData,algFuncProps.GetControlJacPattern())
    disp('Error in algebraic func control jacobian pattern')
end
truthData = [1;0];
if ~isequal(truthData,algFuncProps.GetTimeJacPattern())
    disp('Error in algebraic func control jacobian pattern')
end