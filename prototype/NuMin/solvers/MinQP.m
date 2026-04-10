function [x, q, lambda, Converged, W, iter] = MinQP(x0, G, d, A, b, eqInd, ineqInd,...
    W, Options, Phase)

%  minQP finds a solution to the quadratic programming problem
%  defined by:
%               min .5*x'*G*x + x'*d
%        subject to    Aeq*x =  beq
%                    Aineq*x >= bineq
%
%  QPSOLVE uses an active set method similar to algorithm 16.1 of Nocedal
%  and Wright.  A feasible initial guess is calculated using a "Phase I"
%  stage that accepts a user provided estimate for a feasible point.  The
%  Phase I stage solves a simplified optimization problem, that minimizes
%  the infinity norm of the constraints  violation using a method based on
%  section 7.9.2 of Ref. [2].   If the infinity norm of the constraint violation
%  is zero, then a feasible solution has been found.  If the variance is
%  non-zero, then there is not a feasible solution.  To solve the
%  simplified Phase I problem, QPSOLVE is called recursively with
%  modifications made to G, f, A, and b.
%
%  At each iteration, the KKT conditions are solved using the null
%  space method.  minQP supports a "Hot Start" strategy, which means
%  the user can provide a guess for the active constraint set.  This is
%  important when using QPSOLVE inside of an SQP optimizer.
%
%   References:
%   1) Nocedal, J., and Wright, S.,  "Numerical Optimization", 2nd Edition,
%   Springer, Ithica, NY., 2006.
%
%   2) Gill, P.E., Murray, W., and Wright, M.H., " Numerical Linear Algebra
%   and Optimization,"  Addison Wesley Publishing Company, Redwood City,
%   CA, 1991.
%
%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   09-08-08  S. Hughes:  Created the initial version.

%  Variable Name  Dim.  Units  Description
%  ------------------------------------------------------------------------
%  x0            n x 1   Any   The vector of optimization variables
%                              We define the lenght of x0 to be n
%  G             n x n   Any   The Hessian matrix of the cost function
%  d             n x 1   Any   The linear coefficiens in the cost function
%  A             m x n   Any   The LHS  of the linear contraints matrix
%  b             m x 1   Any   The RHS  of the linear contraints
%  eqInd        mE x 1   N/A   Set of integers representing which
%                              constraints in A*x = b are equality
%  ineqInd      mI x 1   N/A   Set of integers representing which
%                              constraints in A*x = b are inequality
%                              NOTE:  eqInd and ineqInd must contain unique
%                              values.  An index cannot appear in both or
%                              in the same array twice.
%  W            mW x 1   N/A   Intial guess for set of active inequality constraints
%  Options        N/A    N/A   Structure containing options and settings
%  Phase         1 x 1   N/A   Flag that indicates in running phase 1 or 2.
%  lambda        m x 1   Any   Vector of Lagrange multipliers
%  Converged     1 x 1   N/A   Flag indicating the exit criteria

% ----- Calculate dimensions of various quantities
DebugData = 0;
numDecisionVars  = size(x0,1);
numEqCons = size(eqInd,1);
numIneqCon = size(ineqInd,1);
m  = size(A,1);
mW = size(W,1);
numActiveConstraints = mW + numEqCons;

% ----- Check that the user inputs are consistent
if Phase == 2
    flag = CheckInputs(x0, G, d, A, b, eqInd, ineqInd, W, Options, ...
        numDecisionVars, numEqCons, numIneqCon, m, mW, numActiveConstraints);
    if flag == 0
        x = [];
        q = [];
        lambda = [];
        Converged = -1;
        disp('Optimization did not succeed.  There are errors in the problem statement.')
        return
    end
end

% ----- Scale the constraints and cost
if Phase == 2
    normA = zeros(m,1);
    for i = 1:m
        fac = norm(A(i,:));
        if fac > eps
            A(i,:) = A(i,:)/fac;
            b(i,1) = b(i,1)/fac;
            normA(i,1) = fac;
        else
            normA(i,1) = 1;
        end
    end
    fac = norm(G);
    if fac > eps
        G = G/fac;
        d = d/fac;
    end
else
    fac   = 1;
    normA = ones(m,1);
end

% ----- If we're in Phase 2 check to see if the initial guess is feasible.
%       If the initial guess is not feasible, modify the intial guess to
%       find a feasible point, using the standard Phase I method.
if numEqCons + numIneqCon > 0 && Phase == 2
    
    %  Determine if any of the constraints are violated for initial guess.
    conViolation = A*x0 - b;
    feasible     = 1;
    if numEqCons > 0
        if max(abs(conViolation(eqInd,1))) > 10*eps
            feasible = 0;
        end
    end
    if numIneqCon > 0
        if min(conViolation(ineqInd,1)) < -10*eps
            feasible = 0;
        end
    end
    
    %  If the intial guess is infeasible, perform the Phase I optimization.
    if feasible == 0
        
        %  Create initial guess for active set and solve the Phase I
        %  problem.  We reformulate the problem by adding another variable
        %  that is the a measure of the constraint violation.  The problem
        %  sets the Hessian equal to zero, and the gradient is defined
        %  so that the only contribution to the cost function, is the
        %  constraint violations.  When minimized, the cost function should
        %  be zero, or there is not a feasible solution.
        [x0_I, A_I, G_I, d_I, b_I]   = SetUpPhaseI(x0,A,b,eqInd,ineqInd,W);
        oldOptions = Options;
        Options.Display = 1;
        [x, q, lambda, ~, W] = MinQP(x0_I, G_I, d_I, A_I, b_I,...
            eqInd, [ineqInd;m+1],W, Options, 1);
        Options = oldOptions;
        %  Extract data from the Phase I solution.  First check to see
        %  if a feasible solution was found.
        x0    = x(1:numDecisionVars,1);
        error = x(numDecisionVars+1,1);
        if error >= 1e-4
            disp('Error in minQP:  Problem appears infeasible')
            x    = x0;
            lambda = lambda(1:m,1);
            Converged = -2;
            q    = 0.5*x'*G*x + x'*d;
            return
        end
        [~,ind] = find(W == numIneqCon + 1);
        if ~isempty(ind)
            W(ind) = []; %#ok<NASGU>
        end
        ConViolation = A(ineqInd,:)*x0 - b(ineqInd,:);
        Wind =  abs( ConViolation ) <= 1e-14 ;
        W = ineqInd(Wind);
        mW = size(W,1);
        numActiveConstraints = numEqCons + mW;
        
    end
end



% ----- Preparations to begin iteration.  Initialize counters and arrays
%       determine the non-active set of inequality constraints.
q         = (.5*x0'*G*x0 + x0'*d)*fac;
iter      = 0;
Converged = 0;
alpha     = 0;
x         = x0;
lambda    = [];
Winact    = ineqInd;
for i = 1:size(W,1)
    [remove]       = find(Winact == W(i,1));
    Winact(remove) = [];
end
[Q,~] = qr(A([eqInd;W],:)');
if DebugData
    display('==== Working Sets After Call to Phase 1 ====')
    display('Working Set')
    display(W)
    display('Inactive Set')
    display(Winact)
end
% ----- Write output headers for data, and write data for initial guess
formatstr = ' %5.0f    %11.6g    %13.6g  %13.10g  %13.6g  %s';
if Options.Display == 1
    header = sprintf(['\n                                              Max         Min\n',...
        ' Iteration        f(x)         Step-size         Lambda     Violation       Action']);
    disp(header);
    iterdata = sprintf(formatstr,iter,q,0);
    disp(iterdata);
end
MaxIter = 10*max(numDecisionVars,m-numEqCons);


while Converged == 0
    
    % -----  Loop updates
    iter   = iter + 1;
    q      = (0.5*x'*G*x + x'*d)*fac;       % The quadratic cost fuction
    g      = G*x + d;                       % First derivative of q w/r/t x
    lambda = [];                            % Initialize lambda to empty
    
    % ----- Calculate the step size
    %       If the matrix of active constraints is not empty then use the
    %       null space method to calculate the step size.  Otherwise, use
    %       the a Newton step.
    %  Solve the following system:
    %        [  G  -A^T ][p     ] = [g]
    %        [  A   0   ][lambda] = [h]
    %  where
    %        h = A*x - b  (for active set)
    %        g = G*x + d
    
    if numActiveConstraints < numDecisionVars && numActiveConstraints > 0
        
        %-----  Calculate the step using the null space method
        Y     = Q(:,1:numActiveConstraints);                   % an n x m  ([Y|Z] is non singular)
        Z     = Q(:,numActiveConstraints+1:numDecisionVars);   % n x ( n - m ) Null Space Matrix
        h     = A([eqInd;W],:)*x - b([eqInd;W],:);
        
        if Phase == 2
            [L,U] = lu(A([eqInd;W],:)*Y);
            py    = -U\(L\ h);                         %N&W 2nd Ed.,  Eq. 16.18
            [L,U] = lu(Z'*G*Z);
            pz    = -U\(L\ (Z'*G*Y*py + Z'*g));    %N&W 2nd Ed.,  Eq. 16.19
            p     =  Y*py + Z*pz;
            stepType = 'Null Space Phase 2';
        else
            p     = -Z*Z'*g;
            stepType = 'Null Space Phase 1';
        end
        
    elseif numActiveConstraints == numDecisionVars
        %-----  Null space is empty because there are no free variables.
        p = A([eqInd;W],:)\(b([eqInd;W],:)) - x;
        stepType = 'No Free Variables';
    else
        %-----  There are no constraints so use the Newton step
        if norm(G) ~= 0
            [R] = chol(G);
            p   = -(R \( R'\(g)));
            stepType = 'Newton';
        else
            %-----  No constraints,  Hessian is zero, so use steepest descent
            p   = -g;
            stepType = 'Steepest Descent';
        end
        
    end
    if DebugData
        display('==== Computed Step Size and Method ====')
        display(['Step Type : ' stepType])
        display(p )
    end
    % ----- Check to see if we found a solution
    if norm(p) <= 1e-7 || iter >= MaxIter
        
        %  Calculate the Lagrange multipliers of the active inequality
        %  constraints.  If the matrix is empty then set lambda to emtpy.
        [Q,R] = qr(A([eqInd;W],:)');
        lambda = zeros(m,1);
        lambdaW = [];
        if numActiveConstraints > 0;
            % Calculate the Lagrange mulipliers and undo the scaling.
            lambdaAct = R\(Q'*g*fac)./normA([eqInd;W]);
            lambda([eqInd;W],1) = lambdaAct;
            lambdaW = lambdaAct(numEqCons+1:numEqCons+mW);
        end
        
        if DebugData
            display('==== Variables After Updating Lagrange Multipliers ====')
            display('Working Set')
            display(W)
            display('Inactive Set')
            display(Winact)
            display('Working Set Lagrange Multipliers');
            display(lambdaW)
        end
        
        %  Check to see if there are any negative multipliers
        if mW > 0
            [minLambda,j] = min(lambdaW);
        else
            minLambda = 0;
        end
        if isempty(lambda) || minLambda >= -1e-10 || iter >= MaxIter;
            
            %  We found the solution
            if isempty(lambda) || minLambda >= -1e-10
                Converged = 1;
                action = '      Stop';
            else
                Converged = 0;
                action = '  Max. Iterations';
            end
            W      = [eqInd;W]; %#ok<AGROW>
            if Options.Display == 1
                iterdata = sprintf(formatstr,iter,q,alpha,minLambda, [],action);
                disp(iterdata);
            end
            return
            
        else
            
            %  Remove the constraint with the most negative multiplier
            %  [Q,R]  = qrdelete(Q,R,mE + j);
            Winact = [Winact;W(j)]; %#ok<AGROW>
            action = ['      Remove Constraint ' num2str(W(j))];
            W(j)   = [];
            mW     = mW - 1;
            numActiveConstraints     = numActiveConstraints - 1;
            [Q,~] = qr(A([eqInd;W],:)');
            
            if DebugData
                display('==== Variables After Removing constraint with most negative multiplier ====')
                display('Working Set')
                display(W)
                display('Inactive Set')
                display(Winact)
                display('Working Set Lagrange Multipliers');
                display(lambdaW)
            end
            
        end
        
        % ----- Calculate the step length
    else
        
        %  Find distance to inactive constraints in the search direction.
        %  Begin by finding the change in the inactive inequality constraints.
        %  If the change is zero (to some tolerance) or positive, then
        %  there is no way the constraint can become active with the proposed
        %  step so we don't need to consider it further.
        dist     = [];
        if ~isempty(Winact)
            Denom    = A(Winact,:)*p;
            [W2,~] = find( Denom <= -eps );
            dist     = [(b( Winact(W2) , 1) - A(Winact(W2),:)*x)./ Denom(W2,1)  Winact(W2) ];
        end
        
        %  Calculate step and if a new constraint is active, add it to
        %  the working set, W
        mindist = [];
        if ~isempty(dist)
            [mindist,j] = min(dist(:,1));
        end
        if Phase == 2
            if mindist <= 1 + eps
                alpha = mindist;
                hitCon = 1;
            else
                alpha = 1;
                hitCon = 0;
            end
        else
            alpha  = mindist;
            hitCon = 1;
        end
        if DebugData
            display('Distance To Constraints')
            display(dist(:,1))
            display('==== Computed step size an minimum distance ====')
            display('minimum distance to contraint')
            display(mindist)
            display('Selected Step Size')
            display(alpha)
        end
        
        x     = x + alpha*p;
        if hitCon
            ind = find(Winact == dist(j,2));
            Winact(ind) = []; %#ok<FNDSB>
            W     = [W;dist(j,2)]; %#ok<AGROW>
            numActiveConstraints = numActiveConstraints + 1;
            mW = mW + 1;
            [Q,~] = qr(A([eqInd;W],:)');
            action      = ['      Step and Add Constraint ' num2str(dist(j,2))];
        else
            action      = '      Full Step';
        end
        
    end
    
    %  Output iterate data
    q = (0.5*x'*G'*x + d'*x)*fac;
    if ~isempty(lambda)
        minLambda = min(lambda);
    else
        minLambda = 0;
    end
    
    if ~isempty(A([eqInd;W],:));
        maxActiveViolation = min(A([eqInd;W],:)*x - b([eqInd;W],1));
    else
        maxActiveViolation = [];
    end
    if Options.Display == 1
        iterdata = sprintf(formatstr,iter,q,alpha,minLambda,maxActiveViolation,[ action ' ' stepType]);
        disp(iterdata);
    end
    
end

%==========================================================================
%==========================================================================
%==========================================================================
function [x0_I, A_I, G, d, b_I] = SetUpPhaseI(x0,A,b,eqInd,ineqInd,W) %#ok<INUSL>

%  This helper function sets up the Phase I problem which attempts to find
%  a feasible guess to the QP problem by formulating an alternative QP
%  problem that is equivalent to minimizing the infinity norm of the
%  contraint violations.

%----- The number of variables and constraints in the original problem
n  = size(x0,1);                %  Number for variables
m  = size(A,1);                 %  Number of constraints
mE = size(eqInd,1);
mW = size(W,1);
mA = mE + mW;

%----- Determine if the working set is linearly independent.  If it is not,
%      then remove constraints to make it linearly independent.


%----- Calculate a step that satisfies the initial working set, but is as
%      close to the user's guess as possible.
%            solve        Min dx
%            subject to   A(x0 + dx) = b
%
%       The solution is dx = A^-1*( b - A*x0);
if n >= mA
    dx = pinv(A([eqInd;W],:))*(b([eqInd;W],1) - A([eqInd;W],:)*x0);
    x0 = x0 + dx;
end

%----- Set up the cost function, constraints, and initial guess
%      See pgs. 313 and 314 of Ref. [2] for more details.

%  Calculate the maximum constraint violation.
Viol = A*x0-b;
if m > mE
    maxViol= norm([-Viol(mE+1:m);0],'inf');
else
    maxViol = 0;
end

%  Set up the cost function quantities
G    = zeros(n+1,n+1);         %  Linear problem so G is zero
d    = [zeros(n,1);1];         %  Ref. [2], Eqs. 7.9.5

%  Set up constraint functions
x0_I = [x0;maxViol];         %  Ref. [2], Eqs. 7.9.5
b_I  = [b;0];                %  Ref. [2], Eqs. 7.9.6
if mE > 0
    %  Ref. [2], Eq. 7.9.6 modified to handle equality constraints
    A_I  = [[A;zeros(1,n)],[zeros(mE,1);ones(m+1-mE,1)]];
else
    %  Ref. [2], Eq. 7.9.6
    A_I  = [[A;zeros(1,n)],ones(m+1,1)];
end


%==========================================================================
%==========================================================================
%==========================================================================
function flag = CheckInputs(x0, G, d, A, b, eqInd, ineqInd, W, Options, ...
    n, mE, mI, m, mW, mA)  %#ok<INUSD,INUSL>

%---------------------------------
%  Check constraint data
%---------------------------------
flag = 0;
%  Are A matrix and x vector consistent?
if ~isempty(A)
    if size(A,2) ~= n
        disp('The number of columns in A is not the same as the number of rows in X')
        return
    end
end
%  Are the X0, W, ineqInd, and eqInd column vectors?
if ~isempty(x0)
    if size(x0,2) ~= 1
        disp('The initial guess is not a column vector')
        return
    end
end
if ~isempty(W)
    if size(W,2) ~= 1
        disp('The working set is not a column vector')
        return
    end
end
if ~isempty(eqInd)
    if size(eqInd,2) ~= 1
        disp('The equality set is not a column vector')
        return
    end
end
if ~isempty(ineqInd)
    if size(ineqInd,2) ~=1
        disp('The inequality set is not a column vector')
        return
    end
end
%  Are sets of inequality and equality constraint sets consistent with the A matrix?
if mE + mI > m
    disp(['The number constraints in the equality and inequality sets exceeds ' ...
        'the number of constraints in the A matrix.'])
    return
end
%  Are A matrix and b vector consistent?
if ~isempty(b)
    if m ~= size(b,1)
        disp('The number of rows in A is not the same as the number of rows in b.')
        return
    end
end

%  Are the elements of W0 contained only in the inequality set?
for i = 1:mW
    check = find(W(i) == eqInd);
    if ~isempty(check)
        disp('An index in the working set is also in the equality set.')
        return
    end
end
%  Are the elements of W0 all contained in the inequality set?
for i = 1:mW
    check = find(W(i) == ineqInd);
    if isempty(check)
        disp('An index in the working set is also in the equality set.')
        return
    end
end
%  Are the elements of ineqInd and eqInd unique?
for i = 1:mE
    check = find(eqInd(i) == ineqInd);
    if ~isempty(check)
        disp('An constraint index occurs in both the equality and inequality sets.')
        return
    end
end

%---------------------------------
%  Check function data
%---------------------------------
%  Are the dimensions of G consistent with X?
if size(G,1) ~= n || size(G,2) ~= n
    disp('The dimensions of G are not consistent with the dimension of X')
end
%  Are the dimensions of d ok?
if size(d,1) ~= n || size(d,2) ~= 1
    disp('The dimensions of d are not correct')
    return
end
%eigG = eig(G);
% if abs(max(imag(eigG))) ~= 0
%     disp('The G has imaginary eigenvalues')
%     return
% end
%if min(real(eigG)) <= 0
%    disp('The G matrix is not positive definite')
%    return
%end
flag = 1;


