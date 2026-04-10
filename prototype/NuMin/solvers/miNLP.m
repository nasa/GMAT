function [x,f,Converged,OutPut] = miNLP(costFunc,x0, A, b, Aeq, beq, lb, ...
    ub, nlconstFunc, Options, varargin)

%  miNLP finds a constrained minimum of a function of several variables.
%  miNLP attempts to solve problems of the form:
%       min F(X)  subject to:  A*X  >= b,  Aeq*X  = beq  (linear constraints)
%        X                     c(X) >== 0, ceq(X) = 0   (nonlinear constraints)
%                                lb <= X <= ub            (bounds)
%
%  miNLP uses an SQP method similar to algorithm XXX of Nocedal
%  and Wright.   A line search is used and the search direction is based
%  on the solution of a quadratic programming subproblem solved using
%  minQP.  minQP uses an active set method and warm start so solve a
%  quadratic approximiation to the NLP.  minNLP uses the BFGS to
%  the Hessian matrix.  This version is only suitable to small and medium
%  scale NLP problems. Future versions will handle large problems.
%
%
%  Variable Name  Dim.  Units  Description
%  ------------------------------------------------------------------------
%  costFunc       N/A    N/A   String containing the name of the objective
%                              function.  costFunc returns the objective
%                              function value, and possibly the derivative.
%  x0            n x 1   Any   The initial guess for optimization variables
%                              We define the length of x0 to be n.
%  A            mLI x n  Any   The LHS of thelinear inequality contraints matrix
%  b            mLI x 1  Any   The RHS of the linear inequality contraints
%  Aeq          mLE x n  Any   The LHS of the linear equality contraints matrix
%  beq          mLE x 1  Any   The RHS of the linear equality contraints
%  lb             n x 1  Any   Lower bounds on optimization variables
%  ub             n x 1  Any   Upper bounds on optimization variables
%  nlconstFunc    N/A    N/A   String containing the name of the nonlinear
%                              constraint function.  nlconstFunc returns the objective
%                              nonlinear equality and equality constraints
%                              and possibly their derivative.
%  Options        N/A    N/A   Structure containing options and settings
%  varargin       N/A    N/A   Cell array to catch extra user inputs
%
%
%   References:
%   1) Nocedal, J., and Wright, S.,  "Numerical Optimization", 2nd Edition,
%   Springer, Ithica, NY., 2006.
%
%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.


%  --------------------------- Internal Variables -------------------------
%  Variable Name  Dim.  Units  Description
%  ------------------------------------------------------------------------
%  n              1x1    N/A   Number of optimization variables
%  mNLI           1x1    N/A   Number of nonlin. ineq. constraints
%  mNLE           1x1    N/A   Number of nonlin. eq. constraints
%  mLI            1x1    N/A   Number of lin. ineq. constraints
%  mLE            1x1    N/A   Number of lin. ineq. constraints
%  m              1x1    N/A   Total number of constraints
%  ce          mNLEx1    Any   Vector of nonlin. eq. constraints
%  ci          mNLIx1    Any   Vector of nonlin. ineq. constraints
%  Je          nxmNLE    Any   Jacobian of nonlin. eq. constraints
%  Ji          nxmNLI    Any   Jacobian of nonlin. ineq. constraints
%  F              1x1    Any   Cost value
%  gradF          nx1    Any   Gradian of the cost function
%  Lagragian      1x1    N/A   Lagrangian
%  lagMult        mx1    N/A   Vector of lagrange multipliers ordered like:
%                                   Lamda = [ LambdaLeq; LambdaNLeq;
%                                             LambdaLineq; LambdaNLineq];
%                                   where the multipliers for inactive
%                                   inequality constraints are zero.
%  conVec           mx1 Any    Vector concatenization of all constraints
%  mu            1x1    N/A    Penalty parameter for constraints in the
%                              merit function.  mu is always positive.
%  To do:
%  1)  Fix minQP to remove linearly independent constraints
%  2)  Fix minQP to identify problems with incompatible constraints.
%  3)  Fix miNLP to switch to elastic mode when QP subproblem fails.
%  4)  Add line search and merit function from SNOPT.
%  5)  Add Hessian update from SNOPT.

%--------------------------------------------------------------------------
%---------------------------  Initializations -----------------------------
%--------------------------------------------------------------------------

%----- Initialize counters
numfEval = 0;                         %  The number of function evaluations
n        = length(x0);                  %  Number of optimization variables

%----- If there are bound contraints, convert them to linear inequalities
%----- Also, if the initial guess breaks a bound constraint, set to bound
numBound = sum([(lb > -inf);(ub < inf)]);
Ab       = zeros(numBound,n);
bb       = zeros(numBound,1);
cb = 0;
if ~isempty(lb)
    for i = 1:n;
        %----- Set guess to bound if guess is lower than bound
        if x0(i) < lb(i);
            x0(i) = lb(i);
        end
        %----- Construct the lower bound constraint matrix
        if lb(i) > -inf
            cb = cb + 1;
            Ab(cb,i) = 1;
            bb(cb,1) = lb(i);
        end
    end
end
if ~isempty(ub)
    for i = 1:n;
        %----- Set guess to bound if guess is greater than bound
        if x0(i) > ub(i);
            x0(i) = ub(i);
        end
        %----- Construct the upper bound constraint matrix
        if ub(i) < inf
            cb = cb + 1;
            Ab(cb,i) = -1;
            bb(cb,1) = -ub(i);
        end
    end
end
A = [A;Ab];
b = [b;bb];

%----- Evaluate the function and gradient at the initial guess
[f,gradF,numGEval] = GetDerivatives(costFunc,x0,Options,varargin{:});
numfEval = numfEval + numGEval;
if ~isempty(nlconstFunc)
    [ci,ce,Ji,Je] = GetConDerivatives(nlconstFunc,x0,Options,varargin{:});
else
    ci= []; ce = []; Ji =[]; Je = [];
end

%----- Determine dimensionality of the problem and
mNLI = size(ci,1);              %  Number of nonlin. ineq. constraints
mNLE = size(ce,1);              %  Number of nonlin. eq. constraints
mLI  = size(A,1);               %  Number of lin. ineq. constraints
mLE  = size(Aeq,1);             %  Number of lin. ineq. constraints
mE   = mLE + mNLE;              %  Total number of equality constraints
mI   = mLI + mNLI;              %  Total number of inequality constraints
m    = mE + mI;                 %  Total number of constraints

%  Write data describing the problem and solution approach
%disp([ ' Objective Function: ' costFunc ]);
%disp([ ' Nonlinear Constraint Function: ' nlconstFunc ]);
disp([ ' Number of Variables: ' num2str(n,6)]);
disp([ ' Number of Lin. Ineq. Constraints: ' num2str(mLI,6)]);
disp([ ' Number of Lin. Eq. Constraints: ' num2str(mLE,6)]);
disp([ ' Number of NonLin. Ineq. Constraints: ' num2str(mNLI,6)]);
disp([ ' Number of NonLin. Eq. Constraints: ' num2str(mNLE,6)]);
disp([ ' Descent Dirction: ' Options.hessUpdateMethod]);
disp([ ' Merit Function  : ' Options.meritFunction ]);

%--------------------------------------------------------------------------
%---------------  Write Header and data on initial guess ------------------
%--------------------------------------------------------------------------

header = ...
    sprintf(['\n                             max con.    Line search  Directional  First-order \n',...
    ' Iter F-count        f(x)   violation   steplength   derivative   optimality     Procedure ']);
formatstrFirstIter = '%5.0f  %5.0f %12.6g %12.4g                                         %s';
formatstr          = '%5.0f  %5.0f %12.6g %12.4g %12.3g %12.3g %12.3g %s  %s';
[c,J]           = ConCatConstraints(x0,ci,ce,Ji,Je,A,b,Aeq,beq,mLI,mLE);
maxConViolation = max(CalcConViolations(c,mE,mI,m));
iterdata        = sprintf(formatstrFirstIter,0,numfEval,f,maxConViolation);
disp(header);
disp(iterdata);

%--------------------------------------------------------------------------
%---------------------------  Initializations  ----------------------------
%--------------------------------------------------------------------------

%----- Initializations
lambda    = zeros(m,1);
method    = '';
Converged = 0;
iter      = 0;                    %  The number of optimization iterations
W         = eye(n);               %  Guess for Hessian of the Lagragian
x         = x0;
if mLE + mNLE > 0
    eqInd     = (1:mLE+mNLE)';
else
    eqInd   = [];
end
if mLI + mNLI > 0
    ineqInd   = (mLE+mNLE+1:m)';
else
    ineqInd   = [];
end

%----- Line search parameters
rho   = 0.7;
sigma = 1.0;
tau   = 0.5;
eta   = 0.4;
mu    = 1;

%--------------------------------------------------------------------------
%----------------------  Perform the Algorithm  ---------------------------
%--------------------------------------------------------------------------

%----- Perform the iteration
while ~Converged && iter <= Options.MaxIter && ...
        numfEval <= Options.MaxFunEvals
    
    %----- Increment the counter
    iter = iter + 1;
    
    % ----- Solve the Quadraditic Programming Subproblem
    %  Define the QP subproblem according to N&W 2nd Ed., Eqs.18.11
    %                min:   f + gradF'*p + 0.5*p'*W*p  (over p)
    %         subject to:   gradce'*p + ce =  0  (i in Equality Set)
    %                       gradci'*p + ci >= 0  (i in Inequality Set)
    %
    %  In terms of what minQP needs, we rewrite this as
    %                min:    0.5*p'*W*p + gradF'*p  (over p)
    %         subject to:   gradce'*p  = -ce  (i in Equality Set)
    %                       gradci'*p >= -ci  (i in Inequality Set)
    
    %----- Setup the QP subproblem and call minQP
    mode = 1; %  use the old procedural interface to miNLP
    [px, q, plam, flag] = miNLP_searchdir(x,f,gradF,ce,ci,Je,Ji,A, ...
        b,Aeq,beq,eqInd,ineqInd,Options,mLE,mLI,m,W,lambda,W,mode);
    
    if flag ~=1
        keyboard
    end
    %----------------------------------------------------------------------
    %-------- Perform the line search to determine the step length --------
    %----------------------------------------------------------------------
    
    %----- Calculate the constraint penalty parameter, mu, to result in a
    %      descent direction for the merit function.  We use the L1 merit
    %      function, and calculate mu using N&W 2nd Ed. Eq 18.36.
    cviol = CalcConViolations(c,mE,mI,m);
    normc = norm(cviol,1);
    if normc >= 1
        muRHS = (gradF'*px + (sigma/2)*px'*W*px)/((1 - rho)*normc);
        if mu <= muRHS
            mu = muRHS*1.1;
        elseif mu >= 2 && mu/2 > muRHS
            mu = mu/2;
        end
    end
    
    alpha     = 1;
    dirDeriv  = gradF'*px - mu*normc;   %  Eq. 18.29
    meritF    = CalcMeritFunction(f, cviol, mu, Options);
    foundStep = 0;
    fold      = f;  xold      = x;  Jold      = J;
    gradFold  = gradF;
    srchCount = 0;
    xk        = x;
    while ~foundStep && srchCount < 10
        
        %-----  Increment counter and value of x
        srchCount   = srchCount + 1;
        x           = xk + alpha*px;
        
        %-----  Evaluate objective,constraints, and merit
        %       at at x = x + alpha*p
        [f,gradF,numGEval]     = GetDerivatives(...
            costFunc,x,Options,varargin{:});
        numfEval               = numfEval + numGEval;
        if ~isempty(nlconstFunc)
            [ci,ce,Ji,Je] = GetConDerivatives(...
                nlconstFunc,x,Options,varargin{:});
        else
            ci= []; ce = []; Ji =[]; Je = [];
        end
        [c,J]       = ConCatConstraints(x,ci,ce,Ji,Je,A,b,Aeq,beq,mLI,mLE);
        cviol       = CalcConViolations(c,mE,mI,m);
        meritFalpha = CalcMeritFunction(f, cviol, mu, Options);
        
        %-----  Check sufficient decrease condition
        if (meritFalpha > meritF + eta*alpha*dirDeriv && abs(dirDeriv) > 1.e-8)
            alpha = alpha*tau;
        else
            foundStep = 1;
        end
        
    end
    
    %----------------------------------------------------------------------
    %----- Evaluate Important Values Such as F, ce, ci at the new step-----
    %----------------------------------------------------------------------
    
    s         = alpha*px;
    lambda    = lambda + alpha*plam;     % N&W Eq.
    if ~isempty(lambda)
        gradLagrangianXK = gradFold - Jold*lambda;
        gradLagrangian   = gradF - J*lambda;
    else
        gradLagrangianXK = gradFold;
        gradLagrangian   = gradF;
    end
    y                = gradLagrangian - gradLagrangianXK;
    
    %----------------------------------------------------------------------
    %----- Output data, Check convergence, and prepare for next iteration
    %----------------------------------------------------------------------
    
    %-----  Ouput Data for New Iterate if Requested
    maxConViolation = max(CalcConViolations(c,mE,mI,m));
    if isempty(maxConViolation);
        maxConViolation = 0;
    end
    iterdata = sprintf(formatstr,iter,numfEval,f,maxConViolation,alpha,...
        dirDeriv,norm(gradLagrangian),method);
    disp(iterdata);
    
    %-----  Check for Convergence
    [Converged] = CheckConvergence(gradLagrangian, fold, f, x, xold,...
        alpha, maxConViolation, Options);
    
    %-----  Update the Hessian of the Lagrangian
    if Converged == 0
        
        if strcmp(Options.hessUpdateMethod,'DampedBFGS')
            
            %----- The Damped BFGS Update formula. Ref. 1, Procedure 18.2.
            %      The values of 0.2 and 0.8 in the Ref. 1 are changed to
            %      0.1 and 0.9 based on empirical evidence:  they seem to
            %      work better for the test problem set.
            projHess = s'*W*s;
            if s'*y >= 0.1*projHess
                theta = 1;
                method = '   BFGS Update';
            else
                theta = ( 0.9*projHess) / (projHess - s'*y);
                method = '   Damped BFGS Update';
                
            end
            r   = theta*y + (1 - theta)*W*s;            %  Ref 1. Eq. 18.14
            W   = W - W*(s*s')*W/projHess + (r*r')/(s'*r);  %  Ref 1. Eq. 18.16
            
        elseif strcmp(Options.hessUpdateMethod,'SelfScaledBFGS')
            
            %----- The self-scaled BFGS update.  See section 4.3.3 of
            %      Eldersveld.
            projHess = s'*W*s;
            if s'*y >= projHess;
                gamma = 1;
                method = '   BFGS Update';
                W   = gamma*W - gamma*W*(s*s')*W/projHess + y*y'/(s'*y);
            elseif 0 < s'*y &&  s'*y <= projHess
                gamma = y'*s/(projHess);
                method = '   Self Scaled BFGS ';
                W   = gamma*W - gamma*W*(s*s')*W/projHess + y*y'/(s'*y);
            else
                method = '   No Update ';
            end
        end
        W   = (W' + W)*0.5;
    end
    
end

%  If did not converge, determine why and write to screen
if Converged == 0
    
    if numfEval == Options.MaxFunEvals
        field = sprintf(['\n Optimization Failed \n' ...
            ' Solution was not found within maximum number \n of ' ...
            'allowed function evaluations \n']);
        disp(field)
    else
        field = sprintf(['\n Optimization Failed \n' ...
            ' Solution was not found within maximum number \n ' ...
            'of allowed iterations \n']);
        disp(field)
    end
    
end

OutPut.iter   = iter;
OutPut.fevals = numfEval;
disp(' ');

%==========================================================================
function  phi = CalcMeritFunction(f, cviol, mu, Options)

if strcmp(Options.meritFunction,'Powell')
    phi = f + mu'*cviol;
elseif strcmp(Options.meritFunction,'NocWright')
    phi = f + mu*sum(cviol);
end

%==========================================================================
function  [c,J] = ConCatConstraints(x,ci,ce,Ji,Je,A,b,Aeq,beq,mLI,mLE)

%    This utility function concatenates all of the constraint vectors into
%    one large vector.  The format is ALWAYS in the following order, which
%    is used consistently throughout MiNLP:
%
%       c = [ Aeq*x - b; ce; A*x - b; ci];  J = [Aeq',ce,A',ci];

%-----  Initialize matrices
c = [];
J = [];

%-----  If there are linear equality constraints, calculate values
if mLE > 0
    c = [c; Aeq*x - beq];
    J = [J, Aeq'];
end

%----- Add in the nonlinear equality constraints
c = [c;ce];
J = [J,Je];

%-----  If there are linear inequality constraints, calculate values
if mLI > 0
    c = [c; A*x - b];
    J = [J, A'];
end

%-----  Add in the nonlinear inequality constraints
c = [c; ci];
J = [J, Ji];


%==========================================================================
function  [cviol] = CalcConViolations(c,mE,mI,m)

%    This utility calculates the constraint violations for all constraints.
%    If there is not a violation for a specific constraint, the violation
%    is set to zero.  For equality constraints, the violation is the
%    absolute value of the constraint value.  For inequality constraints,
%    the constraint violation is nonzero only if the constraint value is
%    negative.  ( If an inequality constraint is positive not violated)

%-----  Calculate the equality constraint violations.
eqViol = abs(c(1:mE));

%-----  Determine which inequality constraints are negative and set the
%       constraint violations.
if mI > 0
    ineqViol = max([-c(mE+1:m)';zeros(1,mI)])';
else
    ineqViol = [];
end
cviol = [eqViol;ineqViol];

%==========================================================================
function Converged = CheckConvergence(gradLagrangian, f, fnew, x, xnew,...
    alpha, maxConViolation, Options)

if ~isempty(maxConViolation)
    if maxConViolation > Options.TolCon
        Converged = 0;
        return
    end
end

if (norm(gradLagrangian) < Options.TolGrad)
    
    Converged = 1;
    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Magnitude of gradient of Lagrangian less that Options.tol \n']);
    disp(field)
    
elseif ( abs(fnew - f) < Options.TolF )
    
    Converged = 2;
    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Absolute value of function improvement \n is less than tolerance']);
    disp(field)
    
elseif (norm(x - xnew) < Options.TolX)
    
    Converged = 3;
    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Change in x is less than tolerance \n']);
    disp(field)
    
elseif alpha < Options.TolStep
    
    Converged = 4;
    field = sprintf(['\n Optimization Terminated Successfully \n' ...
        ' Step length is less than tolerance \n']);
    disp(field)
    
else
    
    Converged = 0;
    
end

%  B
%         elseif strcmp(Options.DescentMethod,'ModifiedBFGS')
%
%             %  This method is documented in Matlab's spec for constrained
%             %  optimization under the section "Updating the Hessian
%             %  Matrix."
%             AN = J';
%
%             if y'*s < alpha^2*1e-3
%                 while t'*s < -1e-5
%                     [yMax,yInd] = min(y.*s);
%                     y(yInd) = y(yInd)/2;
%                 end
%                 if y'*s < (eps*norm(W,'fro'))
%                     method = ' Modified twice';
%                     fac = AN'*c - OLDAN'*OLDC;
%                     fac = fac.*(s.*fac>0).*(y.*s<=eps);
%                     WT = 1e-2;
%                     if max(abs(fac))==0
%                         fac = 1e-5*sign(s);
%                     end
%                     while y'*s < (eps*norm(HESS,'fro')) && WT < 1/eps
%                         y = y + WT*fac;
%                         WT = WT*2;
%                     end
%                 else
%                     how = ' Hessian modified';
%                 end
%             end
%
%             if s'*y > 1e-10
%                 W = W +(y*y')/(y'*s)-((W*s)*(s'*W'))/(s'*W*s);
%             else
%                 method = '   No Update';
%             end



