function [NonLinearIneqCon, JacNonLinearIneqCon, NonLinearEqCon, ...
                                     JacNonLinearEqCon] = EvaluateGMATConstraints(X)

%  function [F,GradF] = EvaluateGMATConstraints(X)
%
%  Description:  This function returns the values of the contraints and
%  Jacobians.  Empty matrices are returned when either a constraint type
%  does not exist, or a Jacobian is not provided.
%
%  Variable I/O
% -------------------------------------------------------------------------
%  Variable Name       I/0     Type         Dimens.        Description/
%                                                          Comments
%
%  X                    I     array         n x 1          Column vector of
%                                                          Independent 
%                                                          variables
%
%  NonLinearEqCon       O   global array    neq x 1 or []  Column vector 
%                                                          containing
%                                                          nonlinear 
%                                                          equality 
%                                                          constraint
%                                                          values.
%
%  JacNonLinearEqCon    O   global array    n x neq or []  Jacobian of the 
%                                                          nonlinear
%                                                          equality 
%                                                          constraints
%
%  NonLinearIneqCon     O  global array     nineq x1 or [] Column vector 
%                                                          containing
%                                                          nonlinear 
%                                                          inequality 
%                                                          constraint
%                                                          values.
%
%  JacNonLinearIneqCon  O  global array   n x ineq or []   Jacobian of the 
%                                                          nonlinear
%                                                          inequality 
%                                                          constraints
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

global NonLinearIneqCon, JacNonLinearIneqCon, NonLinearEqCon, ...
       JacNonLinearEqCon