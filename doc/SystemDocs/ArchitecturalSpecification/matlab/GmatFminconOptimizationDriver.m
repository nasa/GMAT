function [X] = GmatFminconOptimizationDriver(X0, Opt, Lower, Upper)

%  function GmatFminconOptimizationDriver(X0, Opt, Lower, Upper)
%
%  Description:  This function is called from GMAT to drive the fmincon 
%  optimizer.
%
%  Variable I/O
% -------------------------------------------------------------------------
%  Variable Name       I/0     Type        Dimens.     Description/Comments
%
%  X0                  I       array       nx1         Column vector of 
%                                                      initial values for 
%                                                      independent 
%                                                      variables
%
%  Opt                 I       string                  Name of GMAT 
%                                                      FminconOptimizer
%                                                      object.  This is the
%                                                      the options structure used
%                                                      by fmincon.
%
%  Lower               I       array       nx1         Lower bound on the 
%                                                      values of X
%
%  Upper               I       array       nx1         Upper bound on the 
%                                                      values of X
%
%  X                   O       array       nx1         Column vector of 
%                                                      final values for 
%                                                      independent
%                                                      variables
%
%  Notes:  n is the number of independent variables in X
%          neq is the number of nonlinear equality constraints
%          nineq is the number of nonlinear inequality constraints
% -------------------------------------------------------------------------
%
%  External References: fmincon, EvaluateGMATObjective, 
%                       EvaluateGMATConstraints, CallGMATfminconSolver
%
%  Modification History
%
%  06/15/06, D. Conway, Created


% --- Declare global variables
global NonLinearEqCon NLEqConstraintJacobian NonLinearIneqCon ...
    NLIneqConstraintJacobian

X = fmincon(@EvaluateGMATObjective, X0, [], [], [], [], Lower, Upper, ...
    @EvaluateGMATConstraints, Opt)

% Apply the converged variables
CallGMATfminconSolver(X, 'Converged')
