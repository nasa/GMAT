function [F,GradF] = CallGMATfminconSolver(X)


%  function [F,GradF] = CallGMATfminconSolver(X) 
%
%  Description:  This function performs two primary operations.  The first is
%  that is interacts with the GMAT interface and sends the vector X to GMAT,
%  for GMAT to use in an fmincon solver seqeuence.  The second function is to
%  declare the contraint information as global.  The global declaration avoids
%  another call to GMAT to get the constraints data, when fmincon calls 
%  EvaluateGMATConstraints.m.
%
%  Variable I/O
% -------------------------------------------------------------------------
%  Variable Name       I/0     Type         Dimens.        Description/Comments
%
%  X                    I     array         n x 1           Column vector of 
%                                                           Independent variables
%
%  F                    O     array         1 x 1           Cost function value
%
%  GradF                O     array         n x 1  or []    Gradient of the cost function
%
%  NonLinearEqCon       O   global array    neq x 1 or []   Column vector containing 
%                                                           nonlinear equality constraint
%                                                           values.
%
%  JacNonLinearEqCon    O   global array    n x neq or []   Jacobian of the nonlinear
%                                                           equality constraints
%  
%  NonLinearIneqCon     O   global array    nineq x1 or []  Column vector containing 
%                                                           nonlinear inequality constraint
%                                                           values.
%
%  JacNonLinearIneqCon  O   global array    n x ineq or []  Jacobian of the nonlinear
%                                                           inequality constraints
%
%  Notes:  n is the number of independent variables in X
%          neq is the number of nonlinear equality constraints
%          nineq is the number of nonlinear inequality constraints
% -------------------------------------------------------------------------
%
% Copyright (c) 2002-2025 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Rights Reserved.
%
%  Modification History
%
%  29 Aug 06  - S. Hughes, Created File

% --- Declare global variables
global NonLinearEqCon JacNonLinearEqCon NonLinearIneqCon JacNonLinearIneqCon
global callbackResults

xString = sprintf('%.16f ', X)
CallGMAT('CallbackData',xString) % does there need to be waiting here?
CallGMAT('Callback','')
WaitForCallback
WaitForCallbackResults() % callbackResults should contain all values needed
eval(callbackResults)
