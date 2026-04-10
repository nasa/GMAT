function [F,GradF] = EvaluateGMATObjective(X)

%  function [F,GradF] = EvaluateGMATObjective(X)
%
%  Description:  This function takes the nondimensionalized vector of
%  independent variables, X, and sends it to GMAT for evaluation of the
%  cost, constraints, and derivatives.  If derivatives are not calculated 
%  in GMAT, then an empty matrix is returned.
%
%  Variable I/O
% -------------------------------------------------------------------------
%  Variable Name         I/0     Type         Dimens.        Description/
%                                                            Comments
%
%  X                      I     array         n x 1          Column vector 
%                                                            of Independent
%                                                            variables
%
%  F                      O     array         1 x 1          Cost function 
%                                                            value
%
%  GradF                  O     array         n x 1  or []   Gradient of 
%                                                            the cost f'n
%
%  NonLinearEqCon         O   global array    neq x 1 or []  Column vector 
%                                                            containing
%                                                            nonlinear 
%                                                            equality 
%                                                            constraint
%                                                            values.
%
%  JacNonLinearEqCon      O   global array    n x neq or []  Jacobian of the 
%                                                            nonlinear
%                                                            equality 
%                                                            constraints
%
%  NonLinearIneqCon       O  global array     nineq x1 or [] Column vector 
%                                                            containing
%                                                            nonlinear 
%                                                            inequality 
%                                                            constraint
%                                                            values.
%
%  JacNonLinearIneqCon    O  global array   n x ineq or []   Jacobian of the 
%                                                            nonlinear
%                                                            inequality 
%                                                            constraints
%
%  Notes:  n is the number of independent variables in X
%          neq is the number of nonlinear equality constraints
%          nineq is the number of nonlinear inequality constraints
% -------------------------------------------------------------------------
%
%  External References: CallGMATfminconSolver
%
%  Modification History
%
%  06/13/06, S. Hughes, Created


% --- Declare global variables
global NonLinearIneqCon, JacNonLinearIneqCon, NonLinearEqCon, ...
       JacNonLinearEqCon

% --- Call GMAT and get values for cost, constraints, and derivatives
[F, GradF, NonLinearEqCon, JacNonLinearEqCon, NonLinearIneqCon, ...
    JacNonLinearIneqCon] = CallGMATfminconSolver(X);
