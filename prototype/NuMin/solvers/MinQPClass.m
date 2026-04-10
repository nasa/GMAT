classdef MinQPClass < handle
    %  MinQP finds a solution to the quadratic programming problem
    %  defined by:
    %               min 0.5*x'*G*x + x'*d
    %        subject to  b_lower <= A*x < = b_upper
    %
    %  MinQP uses an active set method similar to algorithm 16.1 of Nocedal
    %  and Wright.  A feasible initial guess is calculated using a "Phase I"
    %  stage that accepts a user provided estimate for a feasible point.  The
    %  Phase I stage solves a simplified optimization problem, that minimizes
    %  the infinity norm of the constraint violations using a method based on
    %  section 7.9.2 of Ref. [2]. If the infinity norm of the constraint violation
    %  is zero, then a feasible solution has been found.  If the variance is
    %  non-zero, then there is not a feasible solution.  To solve the
    %  simplified Phase I problem, MinQP is called recursively with
    %  modifications made to G, f, A, and b.
    %
    %  At each iteration, the KKT conditions are solved using the null
    %  space method.  MinQP supports a "Hot Start" strategy, which means
    %  the user can provide a guess for the active constraint set.  This is
    %  important when using QPSOLVE inside of an SQP optimizer.
    %
    %   exitFlag: 1 Converged
    %             0 Invalid QP problem. Mistake in the problem definition
    %            -1 The QP problem is not feasible
    %            -2 Max iterations reached before convergence
    %            -3 Singular QP, no free variables
    %            -4 Failed null space solution
    %            -5 Failed to solve for Lagrange multipliers
    %
    %   References:
    %   1) Nocedal, J., and Wright, S.,  "Numerical Optimization", 2nd Edition,
    %   Springer, Ithica, NY., 2006.
    %
    %   2) Gill, P.E., Murray, W., and Wright, M.H., " Numerical Linear Algebra
    %   and Optimization,"  Addison Wesley Publishing Company, Redwood City,
    %   CA, 1991.
    %
    %   Copyright 2016, United States Government as represented by the
    %   Administrator of The National Aeronautics and Space Administration.
    %   All Rights Reserved.
    %
    %   This software is licensed under Apache License 2.0.
    %
    %   Author. S. Hughes, steven.p.hughes at nasa.gov
    %
    %   Modification History
    %   -----------------------------------------------------------------------
    %   Sept-08     S. Hughes:  Created the initial version.
    %   Mar-04-17   S. Hughes:  Converted to OO MATLAB
    %   Jun-20-17   S. Hughes:  Modified to handle upper and lower bounds
    %                           on constraints.  Renamed variables for
    %                           readablity.
    
    properties (Access = 'public')
        % Options below this line
        WriteOutput = false;
    end
    properties (Access = 'private')
        % Real Vector. Guess for optimization variables. numDecisionVars x 1
        initialGuess
        % Real Vector. The decision vector (vector of optization variables) numDecisionVars x 1
        decVec
        % Matrix. Hessian matrix (numDecisionVars x numDecisionVars)
        hessianMat
        % Real Vector. Gradient vector. numDecisionVars x 1
        gradVec
        % Matrix. Linear constraints A matrix A*x = b. numCons x numDecisionVars
        A
        % Real Vector. Lower bound on linear constraints A*x. numCons x 1
        conLowerBounds
        % Real Vector. Upper bound on linear constraints A*x. numCons x 1
        conUpperBounds
        % Integer vector. Indeces of the equality constraints. numEqCons x 1
        eqIdxs
        % Integer vector. Indeces of the inequality constraints. Dimension varies.
        ineqIdxs
        % Integer. The problem phase (1 for infeasbible startup, 2 for feasible run)
        Phase
        % Integer. Number of decision variables
        numDecisionVars
        % Integer. Number of equality constraints
        numEqCons
        % Integer. Number of inequality constraints
        numIneqCons
        % Integer. The total number of constraints (equality + inequality)
        numCons
        % Integer. Number of constraints in the working set
        numWorkingIneqCons
        % Integer. Number of active constraints (num equality + num working)
        numActiveCons
        % Bool. Flag to write debug data
        writeDebug = false;
        % Integer vector. Indeces of working set.  stdvec: Dimension varies.
        activeIneqSet
        % Inactive inequality constraint indeces. stdvec: Dimension varies.
        inactiveIneqSet
        % Real Vector. Product of A and decVec. numCons x1
        conValues
        % Integer array. Vector of 1s and 2s where 1 means inequality con and 2 means equality
        conType
        % Real. Tolerance on constraint satisfaction
        conTolerance = 1e-12
        % Bool. True if Hessian has all zero entries, false otherwise.
        isHessianZero
        % Integer. Number of QP iterations
        numIterations
        % Real.  The  value of the cost function
        costValue
    end
    
    methods (Access = 'public')
        
        function  obj = MinQPClass(initialGuess, G, d, A, conLowerBounds, conUpperBounds, W, Phase)
            
            % Set member data
            obj.A = A;
            obj.numCons  = size(obj.A,1);
            obj.SetDecisionVector(initialGuess);
            obj.hessianMat = G;
            obj.gradVec = d;
            obj.conLowerBounds = conLowerBounds;
            obj.conUpperBounds = conUpperBounds;
            obj.SetConstraintTypes();
            obj.activeIneqSet = W;
            obj.Phase = Phase;
            
            % Calculate dimensions of various quantities
            obj.numDecisionVars  = size(obj.decVec,1);
            obj.numEqCons = length(obj.eqIdxs);
            obj.numIneqCons = length(obj.ineqIdxs);
            obj.numCons  = size(obj.A,1);
            obj.numWorkingIneqCons = length(obj.activeIneqSet);
            obj.numActiveCons = obj.numWorkingIneqCons + obj.numEqCons;
            obj.TestForNonZeroHessian();
            obj.ValidateProblemConfig();
            
        end
        
        function SetWriteOutput(obj,flag)
            % Sets flag to optionally write output during iteration
            obj.WriteOutput = flag;
        end
        
        function workingSet = GetActiveSet(obj)
            % returns the working set vector
            workingSet = obj.activeIneqSet;
        end
        
        function [decVec, costValue, lagMult, exitFlag, numIterations, activeConIdxs] = Optimize(obj)
            % Solves the QP Optimization problem.
            
            % ---- Configure MATLAB to treat singular matrices as errors
%             warning('error','MATLAB:singularMatrix')
%             warning('off', 'MATLAB:nearlySingularMatrix');
%             warning('off', 'MATLAB:singularMatrix');
%             warning('off', 'MATLAB:singularMatrix');
 %           obj.WriteOutput = true;
            % ----- Check that the user inputs are consistent
            if obj.Phase == 2
                flag = obj.ValidateProblemConfig();
                if flag == 0
                    exitFlag = 0;
                    lagMult = [];
                    [decVec, costValue, numIterations, activeConIdxs] = ...
                        PrepareOutput(obj,exitFlag);
                    disp('Optimization did not succeed. There are errors in the problem statement.')
                    return
                end
            end
            
            % ----- Scale the constraints and cost
            if obj.Phase == 2
                normA = zeros(obj.numCons,1);
                for i = 1:obj.numCons
                    fac = norm(obj.A(i,:));
                    if fac > eps
                        obj.A(i,:) = obj.A(i,:)/fac;
                        obj.conLowerBounds(i,1) = obj.conLowerBounds(i,1)/fac;
                        obj.conUpperBounds(i,1) = obj.conUpperBounds(i,1)/fac;
                        normA(i,1) = fac;
                    else
                        normA(i,1) = 1;
                    end
                end
                fac = ComputeHessianScaleFactor(obj);
                if fac > eps
                    obj.hessianMat = obj.hessianMat/fac;
                    obj.gradVec = obj.gradVec/fac;
                end
            else
                fac = 1;
                normA = ones(obj.numCons,1);
            end
            
            % Remove inactive inequality constraints from the working set guess
            if obj.Phase == 2 && obj.numWorkingIneqCons > 0
                conViolation = obj.ComputeAllConViolations();
                removeIds = [];
                numToRemove = 0;
                for loopIdx = 1:length(obj.activeIneqSet)
                    conIdx = obj.activeIneqSet(loopIdx);
                    if find(obj.ineqIdxs == conIdx) && abs(conViolation(conIdx)) > 10*eps
                        numToRemove = numToRemove+1;
                        removeIds(numToRemove) = loopIdx; %#ok<AGROW>
                    end
                end
                obj.activeIneqSet(removeIds) = [];
                obj.numWorkingIneqCons = length(obj.activeIneqSet);
                obj.numActiveCons = obj.numWorkingIneqCons + obj.numEqCons;
            end
            
            % ----- If we're in Phase 2 check to see if the initial guess is feasible.
            %       If the initial guess is not feasible, modify the intial guess to
            %       find a feasible point, using the standard Phase I method.
            if obj.numEqCons + obj.numIneqCons > 0 && obj.Phase == 2
                
                %  Determine if any of the constraints are violated for initial guess.
                conViolation = obj.ComputeAllConViolations();
                feasible     = 1;
                if obj.numEqCons > 0
                    if max(abs(conViolation(obj.eqIdxs,1))) > obj.conTolerance
                        feasible = 0;
                    end
                end
                if obj.numIneqCons > 0
                    if min(conViolation(obj.ineqIdxs,1)) < - obj.conTolerance
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
                    [initialGuess_I, A_I, G_I, d_I, b_L, b_U]   = ...
                        obj.SetUpPhaseI(obj.decVec);
                    import MinQPClass
                    phase1QP = MinQPClass(initialGuess_I, G_I, d_I, A_I, b_L,...
                        b_U,obj.activeIneqSet, 1);
                    %phase1QP.WriteOutput = false;
                    if obj.writeDebug
                        phase1QP.SetWriteOutput(true);
                        fprintf('Entering QP Phase 1 Feasibilization Attempt')
                    end
                    % TODO: Clean up set get for W
                    [guessDecVec, ~, lagMult,flag] = phase1QP.Optimize();
                    
                    if obj.writeDebug
                        if flag ~= 1
                            fprintf('QP Phase 1 Feasibilization FAILED \n')
                        else
                            fprintf('QP Phase 1 Feasibilization SUCCEDED \n')
                        end
                    end
                    obj.activeIneqSet = phase1QP.activeIneqSet;
                    
                    %  Extract data from the Phase I solution.  First check to see
                    %  if a feasible solution was found.
                    obj.SetDecisionVector(guessDecVec(1:obj.numDecisionVars,1));
                    error = guessDecVec(obj.numDecisionVars+1,1);
                    errorLowerBound = obj.A*guessDecVec(1:obj.numDecisionVars) - obj.conLowerBounds;
                    errorUpperBound = obj.A*guessDecVec(1:obj.numDecisionVars) - obj.conUpperBounds;
                    errorLowerBoundIneq = errorLowerBound(obj.ineqIdxs);
                    errorUpperBoundIneq = errorUpperBound(obj.ineqIdxs);
                    obj.activeIneqSet = [];
                    obj.inactiveIneqSet = [];
                    for i = 1:length(errorLowerBoundIneq)
                        if abs(errorLowerBoundIneq(i)) < obj.conTolerance || ...
                                abs(errorUpperBoundIneq(i)) < obj.conTolerance
                            obj.activeIneqSet = [obj.activeIneqSet ; i];
                        else
                            obj.inactiveIneqSet = [obj.inactiveIneqSet ; i];
                        end
                    end
                    obj.activeIneqSet = obj.ineqIdxs(obj.activeIneqSet);
                    if error >= 1e-4
                        if obj.WriteOutput == 1
                            disp('Error in minQP:  Problem appears infeasible')
                        end
                        lagMult = lagMult(1:obj.numCons,1);
                        exitFlag = -1;
                        [decVec, costValue, numIterations, activeConIdxs] = ...
                            PrepareOutput(obj,exitFlag);
                        return
                    end
                    [~,ind] = find(obj.activeIneqSet == obj.numIneqCons + 1);
                    if ~isempty(ind)
                        obj.activeIneqSet(ind) = [];
                    end
                    obj.numWorkingIneqCons = length(obj.activeIneqSet);
                    obj.numActiveCons = obj.numEqCons + obj.numWorkingIneqCons;
                end
            end
            
            % ----- Preparations to begin iteration.  Initialize counters and arrays
            %       determine the non-active set of inequality constraints.
            costValue         = (.5*obj.decVec'*obj.hessianMat*obj.decVec + obj.decVec'*obj.gradVec)*fac;
            obj.numIterations      = 0;
            isConverged = 0;
            alpha     = 0;
            lagMult    = [];
            obj.inactiveIneqSet    = obj.ineqIdxs;
            for i = 1:length(obj.activeIneqSet)
                [remove]       = find(obj.inactiveIneqSet == obj.activeIneqSet(i,1));
                obj.inactiveIneqSet(remove) = [];
            end
            
            [Q,~] = qr(obj.A([obj.eqIdxs;obj.activeIneqSet],:)');
            if obj.writeDebug
                fprintf('==== Working Sets After Call to Phase 1 ====\n')
                fprintf('Working Set\n')
                display(obj.activeIneqSet)
                fprintf('Inactive Set\n')
                display(obj.inactiveIneqSet)
            end
            if obj.writeDebug && obj.Phase == 2
                fprintf('\n')
                fprintf('Entering QP Phase 2 Optimization Attempt \n')
            end
            
            % ----- Write output headers for data, and write data for initial guess
            formatstr = ' %5.0f    %11.6g    %13.6g  %13.10g  %13.6g  %s';
            if obj.WriteOutput == 1
                fprintf(['\n                                                  Max         Min\n',...
                    ' Iteration        f(x)         Step-size         Lambda     Violation       Action \n']);
                fprintf([formatstr '\n'],obj.numIterations,obj.costValue,0);
                if obj.numIterations == 0
                    fprintf('\n')
                end
            end
            MaxIter = 10*max(obj.numDecisionVars,obj.numCons-obj.numEqCons);
            
            while isConverged == 0
                
                % Loop updates
                obj.numIterations   = obj.numIterations + 1;
                % The quadratic cost fuction
                obj.costValue      = (0.5*obj.decVec'*obj.hessianMat*obj.decVec + obj.decVec'*obj.gradVec)*fac;
                % First derivative of q w/r/t x
                g      = obj.hessianMat*obj.decVec + obj.gradVec;
                % Initialize lagMult to empty
                lagMult = [];
                
                % ----- Calculate the step size
                %       If the matrix of active constraints is not empty then use the
                %       null space method to calculate the step size.  Otherwise, use
                %       the a Newton step.
                %  Solve the following system:
                %        [  G  -A^T ][p     ] = [g]
                %        [  A   0   ][lagMult] = [h]
                %  where
                %        h = A*x - b  (for active set)
                %        g = G*x + d
                
                if obj.numActiveCons < obj.numDecisionVars && obj.numActiveCons > 0
                    
                    %-----  Calculate the step using the null space method
                    Y     = Q(:,1:obj.numActiveCons);  % an n x m  ([Y|Z] is non singular)
                    Z     = Q(:,obj.numActiveCons+1:obj.numDecisionVars);  % n x ( n - m ) Null Space Matrix
                    h     = obj.ComputeActiveConViolations();
                    
                    if obj.Phase == 2
                        stepType = 'Null Space Phase 2';
                        try
                            [L,U] = lu(obj.A([obj.eqIdxs;obj.activeIneqSet],:)*Y);
                            py    = -U\(L\ h); %N&W 2nd Ed.,  Eq. 16.18
                            [L,U] = lu(Z'*obj.hessianMat*Z);
                            pz    = -U\(L\ (Z'*obj.hessianMat*Y*py + Z'*g)); %N&W 2nd Ed.,  Eq. 16.19
                            p     =  Y*py + Z*pz;
                            if isnan(pz)
                                exitFlag = -4;
                                [decVec, costValue, numIterations, activeConIdxs] = ...
                                    PrepareOutput(obj,exitFlag);
                                return
                            end
                        catch
                            exitFlag = -4;
                            [decVec, costValue, numIterations, activeConIdxs] = ...
                                PrepareOutput(obj,exitFlag);
                            return
                        end
                        
                    else
                        p     = -Z*Z'*g;
                        stepType = 'Null Space Phase 1';
                    end
                    
                elseif obj.numActiveCons == obj.numDecisionVars
                    %-----  Null space is empty because there are no free variables.
                    stepType = 'No Free Variables';
                    [Anear,bnear] = ComputeNearbyFeasibleLinearSystem(obj,obj.decVec);
                    try
                        p = Anear\bnear - obj.decVec;
                    catch
                        exitFlag = -3;
                        [decVec, costValue, numIterations, activeConIdxs] = ...
                            PrepareOutput(obj,exitFlag);
                        return
                    end
                else
                    %-----  No constraints, Hessian is NOT zero, so use the Newton step
                    if ~obj.isHessianZero
                        [R] = chol(obj.hessianMat);
                        p   = -(R \( R'\(g)));
                        stepType = 'Newton';
                    else
                        %-----  No constraints,  Hessian is zero, so use steepest descent
                        p   = -g;
                        stepType = 'Steepest Descent';
                    end
                    
                end
                if obj.writeDebug
                    fprintf('==== Computed Step Size and Method ==== \n')
                    fprintf(['Step Type : ' stepType '\n'])
                    display(p )
                end
                % ----- Check to see if we found a solution
                if norm(p) <= 1e-7 || obj.numIterations >= MaxIter
                    
                    %  Calculate the Lagrange multipliers of the active inequality
                    %  constraints.  If the matrix is empty then set lagMult to emtpy.
                    [Q,R] = qr(obj.A([obj.eqIdxs;obj.activeIneqSet],:)');
                    lagMult = zeros(obj.numCons,1);
                    lagMultW = [];
                    if obj.numActiveCons > 0
                        % Calculate the Lagrange mulipliers and undo the scaling.
                        try
                            lagMultAct = R\(Q'*g*fac)./normA([obj.eqIdxs;obj.activeIneqSet]);
                        catch
                            exitFlag = -5;
                            [decVec, costValue, numIterations, activeConIdxs] = ...
                                PrepareOutput(obj,exitFlag);
                            return
                        end
                        
                        lagMult([obj.eqIdxs;obj.activeIneqSet],1) = lagMultAct;
                        
                        lagMultW = lagMultAct(obj.numEqCons+1:obj.numEqCons+obj.numWorkingIneqCons);
                        error = obj.A(obj.activeIneqSet,:)*obj.decVec - ...
                            obj.conUpperBounds(obj.activeIneqSet);
                        % if active at upper bound, reverse the sign of
                        % lagrange multipler
                        for i = 1:length(error)
                            if obj.conType(i) == 1 && error(i) >= -10*eps
                                lagMultW(i) = - lagMultW(i);
                            end
                        end
                    end
                    if obj.writeDebug
                        fprintf('==== Variables After Updating Lagrange Multipliers ====\n')
                        fprintf('Working Set')
                        display(obj.activeIneqSet)
                        fprintf('Inactive Set\n')
                        display(obj.inactiveIneqSet)
                        fprintf('Working Set Lagrange Multipliers\n');
                        display(lagMultW)
                    end
                    
                    %  Check to see if there are any negative multipliers
                    if obj.numWorkingIneqCons > 0
                        [minLambda,j] = min(lagMultW);
                    else
                        minLambda = 0;
                    end
                    if isempty(lagMult) || minLambda >= -1e-10 || obj.numIterations >= MaxIter
                        
                        %  We found the solution
                        if isempty(lagMult) || minLambda >= -1e-10
                            exitFlag = 1;
                            action = '      Stop';
                        else
                            exitFlag = -2;
                            action = '  Max. Iterations';
                        end
                        [decVec, costValue, numIterations, activeConIdxs] = ...
                            PrepareOutput(obj,exitFlag);
                        if obj.WriteOutput == 1
                            fprintf([formatstr '\n'],obj.numIterations,costValue,alpha,minLambda, [],action);
                        end
                        return
                        
                    else
                        
                        %  Remove the constraint with the most negative multiplier
                        %  [Q,R]  = qrdelete(Q,R,mE + j);
                        obj.inactiveIneqSet = [obj.inactiveIneqSet;obj.activeIneqSet(j)];
                        action = ['      Remove Constraint ' num2str(obj.activeIneqSet(j))];
                        obj.activeIneqSet(j)   = [];
                        obj.numWorkingIneqCons     = obj.numWorkingIneqCons - 1;
                        obj.numActiveCons     = obj.numActiveCons - 1;
                        [Q,~] = qr(obj.A([obj.eqIdxs;obj.activeIneqSet],:)');
                        
                        if obj.writeDebug
                            fprintf('==== Variables After Removing constraint with most negative multiplier ====\n')
                            fprintf('Working Set\n')
                            display(obj.activeIneqSet)
                            fprintf('Inactive Set\n')
                            display(obj.inactiveIneqSet)
                            fprintf('Working Set Lagrange Multipliers\n');
                            display(lagMultW)
                        end
                        
                    end
                    
                    % ----- Calculate the step length
                else
                    
                    %  Find distance to inactive constraints in the search direction.
                    %  Begin by finding the change in the inactive inequality constraints.
                    %  If the change is zero (to some tolerance) or positive, then
                    %  there is no way the constraint can become active with the proposed
                    %  step so we don't need to consider it further.
                    distanceToInactiveCons     = [];
                    if ~isempty(obj.inactiveIneqSet)
                        [distanceToInactiveCons,inactiveConIdxs] = ...
                            obj.ComputeDistanceToInactiveCons(p);
                    end
                    
                    %  Calculate step and if a new constraint is active, add it to
                    %  the working set, W
                    mindist = [];
                    if ~isempty(distanceToInactiveCons)
                        [mindist,j] = min(distanceToInactiveCons);
                    end
                    
                    % Pick the step size
                    if obj.Phase == 2
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
                    
                    if obj.writeDebug
                        fprintf('Distance To Constraints\n')
                        display(distanceToInactiveCons)
                        fprintf('==== Computed step size an minimum distance ====\n')
                        fprintf('minimum distance to contraint\n')
                        display(mindist)
                        fprintf('Selected Step Size\n')
                        disp(alpha)
                    end
                    
                    obj.SetDecisionVector(obj.decVec + alpha*p);
                    
                    % If we hit a constraint, add it to the active set vector
                    if hitCon
                        ind = find(obj.inactiveIneqSet == inactiveConIdxs(j));
                        obj.inactiveIneqSet(ind) = []; %#ok<FNDSB>
                        obj.activeIneqSet     = [obj.activeIneqSet;inactiveConIdxs(j)];
                        obj.numActiveCons = obj.numActiveCons + 1;
                        obj.numWorkingIneqCons = obj.numWorkingIneqCons + 1;
                        [Q,~] = qr(obj.A([obj.eqIdxs;obj.activeIneqSet],:)');
                        action      = ['      Step ' num2str(alpha,3) ' and Add Constraint '  ...
                            num2str(inactiveConIdxs(j))];
                    else
                        action      = '      Full Step';
                    end
                    
                end
                
                %  Output iterate data
                costValue = (0.5*obj.decVec'*obj.hessianMat'*obj.decVec + obj.gradVec'*obj.decVec)*fac;
                if ~isempty(lagMult)
                    minLambda = min(lagMult);
                else
                    minLambda = 0;
                end
                
                if ~isempty(obj.A([obj.eqIdxs;obj.activeIneqSet],:))
                    maxActiveViolation = max(abs(obj.ComputeActiveConViolations()));
                else
                    maxActiveViolation = [];
                end
                if obj.WriteOutput == 1
                    fprintf([formatstr '\n'],obj.numIterations,obj.costValue,alpha,minLambda, ...
                        maxActiveViolation,[ action ' ' stepType]);
                end
                
            end
        end
        
    end
    
    methods (Access = 'private')
        
        function [decVec, costValue, numIterations, activeEqs] = ...
                PrepareOutput(obj,exitFlag)
            
            numIterations = obj.numIterations;
            if exitFlag == 1
                costValue = obj.costValue;
                activeEqs = [obj.eqIdxs;obj.activeIneqSet];
                decVec = obj.decVec;
            elseif exitFlag == 0
                decVec = [];
                costValue = [];
                activeEqs = [];
            elseif exitFlag == -1 || exitFlag == -2 || exitFlag == -3 || exitFlag == -4 || exitFlag == -5
                costValue = obj.costValue;
                activeEqs = [];
                decVec = obj.decVec;
            end
            
        end
        
        function TestForNonZeroHessian(obj)
            % Tests for nonzero hessian and sets isHessianZero accordingly
            obj.isHessianZero = true;
            for rowIdx = 1:obj.numDecisionVars
                for colIdx = 1:obj.numDecisionVars
                    if obj.hessianMat(rowIdx,colIdx) ~= 0
                        obj.isHessianZero = false;
                        return
                    end
                end
            end
        end
        
        function scaleFac = ComputeHessianScaleFactor(obj)
            % Computes scale factor for QP based on Inf norm of the
            % Hessian. Returns 1 if Hessian contains all zeros.  Must be
            % called after TestForNonZeroHessian().
            if obj.isHessianZero
                scaleFac = 1;
                return
            end
            scaleFac = 0;
            for rowIdx = 1:obj.numDecisionVars
                rowMag = sum(abs(obj.hessianMat(rowIdx,:)));
                if rowMag > scaleFac
                    scaleFac = rowMag;
                end
            end
        end
        
        function  SetConstraintTypes(obj)
            % Sets arrays eqIdxs and ineqIdxs and conType, and numbers of
            % members of the sets.
            obj.eqIdxs = [];
            obj.ineqIdxs = [];
            obj.conType = [];
            numEqCon = 0;
            numIneqCon = 0;
            for conIdx = 1:obj.numCons
                if obj.conLowerBounds(conIdx) > obj.conUpperBounds(conIdx)
                    error('')
                elseif abs(obj.conLowerBounds(conIdx) - obj.conUpperBounds(conIdx)) > eps
                    numIneqCon = numIneqCon + 1;
                    obj.ineqIdxs(numIneqCon,1) = conIdx;
                    obj.conType(conIdx) = 1;
                else
                    numEqCon = numEqCon + 1;
                    obj.eqIdxs(numEqCon,1) = conIdx;
                    obj.conType(conIdx) = 2;
                end
            end
        end
        
        function SetDecisionVector(obj,decVec)
            % Sets decision vector and updates constraint values
            obj.decVec = decVec;
            if ~isempty(obj.A)
                obj.ComputeConstraints();
            end
        end
        
        function ComputeConstraints(obj)
            % Computes the constraint values A*x
            obj.conValues = obj.A*obj.decVec;
        end
        
        function conViolations = ComputeAllConViolations(obj)
            %  Computes constraint violations at the current value of x.
            %  Returns zero for element if conLowerBounds(i) <= A*x(i) <=conUpperBounds(i)
            %  Returns A*x(i) - conLowerBounds(i) for element  if A*x(i) < conLowerBounds(i)
            %  Returns -A*x(i) + conUpperBounds(i) for element  if A*x(i) > conUpperBounds(i)
            conViolations = zeros(obj.numCons,1);
            for conIdx = 1:obj.numCons
                if obj.conType(conIdx) == 2
                    conViolations(conIdx) = obj.conValues(conIdx) - obj.conLowerBounds(conIdx);
                elseif obj.conType(conIdx) == 1
                    if obj.conLowerBounds(conIdx) <= obj.conValues(conIdx) && ...
                            obj.conValues(conIdx) <= obj.conUpperBounds(conIdx)
                        conViolations(conIdx) = 0;
                    elseif obj.conLowerBounds(conIdx) > obj.conValues(conIdx)
                        conViolations(conIdx) = obj.conValues(conIdx) - obj.conLowerBounds(conIdx);
                    else
                        conViolations(conIdx) = -obj.conValues(conIdx) + obj.conUpperBounds(conIdx);
                    end
                end
            end
            
        end
        
        function conViolations = ComputeActiveConViolations(obj)
            % returns array of active constraint violations defined
            % as violations of equality and working set
            allConViolations = obj.ComputeAllConViolations;
            conViolations = allConViolations([obj.eqIdxs;obj.activeIneqSet]);
        end
        
        function conViolations = ComputeIneqConViolations(obj)
            % returns array of equality constraint violations
            allConViolations = obj.ComputeAllConViolations;
            conViolations = allConViolations([obj.ineqIdxs]);
        end
        
        function [distanceToInactiveCons,inactiveConIdxs] = ...
                ComputeDistanceToInactiveCons(obj,stepVector)
            % Computes the distance from the current decVec to the inactive
            % constraint bounds, scaled by the step size. The function
            % checks the direction of the step, and computes the direction
            % to the bound in the direction of the step.  Note the distance
            % is scaled by the step size to help in comparing the step size
            % to each constraint boundary with the step size to thd cost
            % function minimum in the search direction in the line search.
            AtimesStep  = obj.A(obj.inactiveIneqSet,:)*(stepVector);
            distanceToInactiveCons = [];
            inactiveConIdxs = [];
            for conCount = 1:size(obj.inactiveIneqSet)
                if abs(AtimesStep(conCount)) > obj.conTolerance
                    conIdx = obj.inactiveIneqSet(conCount);
                    % Change in constraint is in direction of lower bound
                    if AtimesStep(conCount) < 0 && obj.conLowerBounds(conIdx) > -Inf
                        dist = (obj.conLowerBounds(conIdx) - obj.A(conIdx,:)*obj.decVec)/...
                            (obj.A(conIdx,:)*stepVector);
                        distanceToInactiveCons = [distanceToInactiveCons;dist];
                        inactiveConIdxs = [inactiveConIdxs;conIdx];
                    elseif AtimesStep(conCount) > 0 && obj.conUpperBounds(conIdx) < Inf
                        %  Change in constraint is in direction of upper bound
                        dist = (obj.conUpperBounds(conIdx) - obj.A(conIdx,:)*obj.decVec)/...
                            (obj.A(conIdx,:)*stepVector);
                        distanceToInactiveCons = [distanceToInactiveCons;dist];
                        inactiveConIdxs = [inactiveConIdxs;conIdx];
                    end
                end
            end
            distanceToInactiveCons = abs(distanceToInactiveCons);
        end
        
        function [Anear,bnear] = ComputeNearbyFeasibleLinearSystem(obj,decVec)
            %  Given a decision vector, computes a "nearby" system of
            %  equations for solution to create a feasible guess for phase
            %  I.  This involves choosing b in Ax = b.  The full
            %  problem is an inequality, conLowerBounds <= Ax < conUpperBounds.  If
            %  Ax satisfies the inequaly, then bnear(i) = A(i,:)*x.
            %  If upper bound is violated,  A(i,:)*x > conUpperBounds,
            %                              then bnear(i) = conUpperBounds(i).
            % If lower bound is violated, A(i,:)*x < conLowerBounds,
            %                              then bnear(i) = conLowerBounds(i);
            
            % Dimension arrays
            numGuessCons = obj.numEqCons + obj.numWorkingIneqCons;
            Anear = zeros(numGuessCons,obj.numDecisionVars);
            bnear = zeros(numGuessCons,1);
            % Add in the equality constraints
            Anear(1:obj.numEqCons,:) = obj.A(obj.eqIdxs,:);
            bnear(1:obj.numEqCons,1) = obj.conLowerBounds(obj.eqIdxs,:);
            % Now handle inequalities.  If current x satisfies constraint
            % use the resulting b value, otherwise choose the violated
            % boundary (upper or lower).
            linSysRowCount = obj.numEqCons;
            for loopIdx = 1:obj.numWorkingIneqCons
                linSysRowCount = linSysRowCount + 1;
                conIdx = obj.activeIneqSet(loopIdx);
                Anear(linSysRowCount,:) = obj.A(conIdx,:);
                conValue = Anear(linSysRowCount,:)*decVec;
                if conValue < obj.conLowerBounds(conIdx)
                    bnear(linSysRowCount) = obj.conLowerBounds(conIdx);
                elseif conValue > obj.conUpperBounds(conIdx)
                    bnear(linSysRowCount) = obj.conUpperBounds(conIdx);
                else
                    bnear(linSysRowCount) = conValue;
                end
            end
        end
        
        function [initialGuess_I, A_I, G, d, b_L, b_U] = SetUpPhaseI(obj,initialGuess)
            
            %  This helper function sets up the Phase I problem which attempts to find
            %  a feasible guess to the QP problem by formulating an alternative QP
            %  problem that is equivalent to minimizing the infinity norm of the
            %  contraint violations.  This function sets up the cost function, constraints,
            %  and initial guess for the phase I feasibility issue based on the write up on
            %  pg. 473 of Ref. [1].
            
            %----- Calculate a step that satisfies the initial working set, but is as
            %      close to the user's guess as possible.
            %            solve        Min dx
            %            subject to   A(initialGuess + dx) = b
            %
            %       The solution is dx = A^-1*( b - A*initialGuess);
            [Anear,bnear] = obj.ComputeNearbyFeasibleLinearSystem(initialGuess);
            if obj.numDecisionVars >= obj.numActiveCons && obj.numActiveCons > 0
                dx = pinv(Anear)*(bnear - Anear*initialGuess);
                initialGuess = initialGuess + dx;
            end
            
            %  Calculate the maximum constraint violation.
            obj.SetDecisionVector(initialGuess);
            Viol = ComputeAllConViolations(obj);
            if obj.numCons > obj.numEqCons
                maxViol= norm([Viol(obj.numEqCons+1:obj.numCons);0],'inf');
            else
                maxViol = 0;
            end
            
            %  Set up the cost function quantities
            G = zeros(obj.numDecisionVars+1,obj.numDecisionVars+1); %  Linear so G is zero
            d = [zeros(obj.numDecisionVars,1);1];  %  Ref. [2], Eqs. 7.9.5
            
            %  Set up constraint functions
            initialGuess_I = [initialGuess;maxViol]; %  Ref. [2], Eqs. 7.9.5
            b_L = [obj.conLowerBounds;0]; %  Ref. [2], Eqs. 7.9.6
            b_U = [obj.conUpperBounds;inf];
            if obj.numEqCons > 0
                %  Ref. [2], Eq. 7.9.6 modified to handle equality constraints
                A_I = [[obj.A;zeros(1,obj.numDecisionVars)],...
                    [zeros(obj.numEqCons,1);ones(obj.numCons+1-obj.numEqCons,1)]];
            else
                %  Ref. [2], Eq. 7.9.6
                A_I = [[obj.A;zeros(1,obj.numDecisionVars)],ones(obj.numCons+1,1)];
            end
        end
        
        function flag = ValidateProblemConfig(obj)
            
            %---------------------------------
            %  Check constraint data
            %---------------------------------
            flag = 0;
            %  Are A matrix and x vector consistent?
            if ~isempty(obj.A)
                if size(obj.A,2) ~= obj.numDecisionVars
                    disp('The number of columns in A is not the same as the number of rows in X')
                    return
                end
            end
            %  Are the initialGuess, W, ineqIdxs, and eqIdxs column vectors?
            if ~isempty(obj.initialGuess)
                if size(obj.initialGuess,2) ~= 1
                    disp('The initial guess is not a column vector')
                    return
                end
            end
            if ~isempty(obj.activeIneqSet)
                if size(obj.activeIneqSet,2) ~= 1
                    disp('The working set is not a column vector')
                    return
                end
            end
            if ~isempty(obj.eqIdxs)
                if size(obj.eqIdxs,2) ~= 1
                    disp('The equality set is not a column vector')
                    return
                end
            end
            if ~isempty(obj.ineqIdxs)
                if size(obj.ineqIdxs,2) ~=1
                    disp('The inequality set is not a column vector')
                    return
                end
            end
            
            %  Are sets of inequality and equality constraint sets consistent with the A matrix?
            if obj.numEqCons + obj.numIneqCons > obj.numCons
                disp(['The number constraints in the equality and inequality sets exceeds ' ...
                    'the number of constraints in the A matrix.'])
                return
            end
            
            %  Are A matrix and b vector consistent?
            if ~isempty(obj.conLowerBounds)
                if obj.numCons ~= size(obj.conLowerBounds,1)
                    disp('The number of rows in A is not the same as the number of rows in b.')
                    return
                end
            end
            
            %  Check that constraint upper bounds are >= to Lower bounds
            if ~isempty(obj.conLowerBounds)
                for conIdx = 1:obj.numCons
                    if obj.conUpperBounds(conIdx) < obj.conLowerBounds(conIdx)
                        disp(['Upper Bound on constraint ' num2str(conIdx)...
                            ' is less than lower bound'])
                        return
                    end
                end
            end
            %  Are the elements of W0 contained only in the inequality set?
            for i = 1:obj.numWorkingIneqCons
                check = find(obj.activeIneqSet(i) == obj.eqIdxs);
                if ~isempty(check)
                    disp('An index in the working set is also in the equality set.')
                    return
                end
            end
            
            %  Are the elements of W0 all contained in the inequality set?
            for i = 1:obj.numWorkingIneqCons
                check = find(obj.activeIneqSet(i) == obj.ineqIdxs);
                if isempty(check)
                    disp('An index in the working set is also in the equality set.')
                    return
                end
            end
            
            %  Are the elements of ineqIdxs and eqIdxs unique?
            for i = 1:obj.numEqCons
                check = find(obj.eqIdxs(i) == obj.ineqIdxs);
                if ~isempty(check)
                    disp('An constraint index occurs in both the equality and inequality sets.')
                    return
                end
            end
            
            %---------------------------------
            %  Check function data
            %---------------------------------
            %  Are the dimensions of G consistent with X?
            if size(obj.hessianMat,1) ~= obj.numDecisionVars ||...
                    size(obj.hessianMat,2) ~= obj.numDecisionVars
                disp('The dimensions of G are not consistent with the dimension of X')
                return
            end
            %  Are the dimensions of d ok?
            if size(obj.gradVec,1) ~= obj.numDecisionVars || size(obj.gradVec,2) ~= 1
                disp('The dimensions of d are not correct')
                return
            end
            flag = 1;
            
        end
        
    end
end

