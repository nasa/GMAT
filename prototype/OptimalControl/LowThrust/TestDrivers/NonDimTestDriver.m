
%%  Example 1 Problem Setup:
%  A linear problem with 5 variables and 9 constraints

%  Define the decision vector
decVec           = [-9 -7 3 9  10 7]';
numVars          = length(decVec);
decVecUpperBound = ones(numVars,1)*10;
decVecLowerBound = ones(numVars,1)*-10;

%  Define the cost function 
costFunc     = -9865.76145;

%  Define constraint functions A*decVec = b, where A is 
A = [1 0 2 0 0 0;
    2 1 0 0 0 0;
    0 3 5 4 0 0;
    0 0 0 1 2 0;
    0 1 0 0 0 0;
    0 0 0 1 1 0;
    0 0 0 0 0 1;
    0 0 1 0 0 1;
    1 0 0 0 0 1];

%  Compute sparsity pattern
[jacRowIdx,jacColIdx,s] = find(A);
sparseJac               = sparse(jacRowIdx,jacColIdx,s);
numCons                 = size(A,1);
conVec                  = A*decVec;

%%  Example 2 problem exectution.  Non-dim then redim.

%  Instantiate a non-dimensionalizer object and initialize it
scaler = NonDimensionalizer;
scaler.Initialize(numVars,numCons,jacRowIdx,jacColIdx);
scaler.SetDecVecScaling_Bounds(decVecUpperBound,decVecLowerBound);
% for now scale constraints randomly 
scaler.SetConstraintScaling_UserDefined(rand(numCons,1));
% Choose a random cost scaling
scaler.SetCostScaling_UserDefined(1e-4)

%  Test scaling of decision vector
scaledDecVec = scaler.ScaleDecisionVector(decVec);
decVecTest = scaler.UnScaleDecisionVector(scaledDecVec) - decVec;
disp('The scaled decision vector is')
disp(scaledDecVec)

%  Test scaling of constraint vector
scaledConVec = scaler.ScaleConstraintVector(conVec);
conVecTest = scaler.UnScaleConstraintVector(scaledConVec) - conVec;
disp('The scaled constraint vector is')
disp(scaledConVec)

%  Test scaling of cost function
scaledCostFunc = scaler.ScaleCostFunction(costFunc);
costTest = scaler.UnScaleCostFunction(scaledCostFunc) - costFunc;
disp('The scaled cost function is')
disp(scaledCostFunc)

%  Test scaling of Jacobian
scaledJac = scaler.ScaleJacobian(sparseJac);
jacTest = scaler.UnScaleJacobian(scaledJac) - sparseJac; 
disp('The scaled Jacobian is')
disp(scaledJac)

%%  Example test results.  See if we got the right answers
if max(decVecTest) >= 1e-14
    disp('Error in scaling or unscaling of Decision Vector')
else
    disp('Decision vector scaling passed example 1 test')
end
if max(conVecTest) ~= 0
    disp('Error in scaling or unscaling of Constraint Vector')
else
    disp('Constraint vector scaling passed example 1 test')
end
if max(costTest) ~= 0
    disp('Error in scaling or unscaling of cost function')
else
    disp('Cost function scaling passed example 1 test')
end
if max(jacTest) ~= 0
    disp('Error in scaling or unscaling of Jacobian')
else
    disp('Jacobian scaling passed example 1 test')
end