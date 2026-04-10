classdef MiNLPClass < handle
    
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
    %   Copyright 2017, United States Government as represented by the
    %   Administrator of The National Aeronautics and Space Administration.
    %   All Rights Reserved.
    %
    %   This software is licensed under the Apache License 2.0.
    %
    %   Modification History
    %   -----------------------------------------------------------------------
    %   Sept-08-08  S. Hughes:  Created the initial version.
    %   Mar-04-17   S. Hughes:  Converted to OO MATLAB
    
    
    %  --------------------------- Internal Variables -------------------------
    %  Variable Name  Dim.  Units  Description
    %  ------------------------------------------------------------------------
    %  n              1x1    N/A   Number of optimization variables
    %  obj.numNonLinIneqCon           1x1    N/A   Number of nonlin. ineq. constraints
    %  obj.numNonLinEqCon           1x1    N/A   Number of nonlin. eq. constraints
    %  mLI            1x1    N/A   Number of lin. ineq. constraints
    %  mLE            1x1    N/A   Number of lin. ineq. constraints
    %  m              1x1    N/A   Total number of constraints
    %  ce          obj.numNonLinEqConx1    Any   Vector of nonlin. eq. constraints
    %  ci          obj.numNonLinIneqConx1    Any   Vector of nonlin. ineq. constraints
    %  Je          nxobj.numNonLinEqCon    Any   Jacobian of nonlin. eq. constraints
    %  Ji          nxobj.numNonLinIneqCon    Any   Jacobian of nonlin. ineq. constraints
    %  F              1x1    Any   Cost value
    %  obj.costJac          nx1    Any   Gradian of the cost function
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
    %  1) Fix minQP to remove linearly dependent constraints
    %  2) Fix minQP to identify problems with incompatible constraints.
    %  3) Fix miNLP to switch to elastic mode when QP subproblem fails.
    %  4) Add line search and merit function from SNOPT.
    %  6) Verify Damped BFGS emperical settings.  Seems like code changes
    %  them and perhaps it shouldn't
    %  7) Finite differenced partials
    %  8) Scaling
    
    properties (Access = 'public')
        % Pointer to UserFunctionManager
        userFuncManager
        % Pointer to MiNLP options structure
        Options
        %         elasticWeight
        %         runMode
        %         elasticVarsV
        %         elasticVarsW
        %         % Real vector.  Upper bound on the step size on per variable basis
        %         stepSizeConstraintVector
        %                 % Bool.  True if user provided a stepSizeConstraintVector, false otherwise
        %         hasStepSizeConstaints
    end
    
    properties (Access = 'private')
        %
        debugSearchDir = false;
        % Integer. Total number of constraints
        totalNumCon
        % Integer. Number of user function evaluations
        numFuncEvals
        % Integer. Number of decision variables
        numDecisionVars
        % Real Vector. The decision vector
        decVec
        % Real Vector. Lower bounds on decision variables
        varLowerBounds
        % Real Vector. Upper bounds on decision variables
        varUpperBounds
        % Real Vector. Lower bounds on constraints
        conLowerBounds
        % Real Vector. Upper bounds on constraints
        conUpperBounds
        % Pointer to userProblem
        userProblem
        % Variable. The cost function value
        costFunction
        % Real array.  The user constraint functions
        conFunctions
        % Real array.  The cost function Jacobian
        costJac
        % Real array. The constraint function Jacobian
        conJac
        % Real array. Gradient of the Lagrangian
        gradLagrangian
        % Real array. Change in the Lagrangian after last step
        deltaGradLagrangian
        % Real array. The change in the decision variables after line
        stepTaken
        % Real matrix.  Hessian of the Lagrangian
        hessLagrangian
        % Real array. Lagrange multiplieres
        lagMultipliers
        % Std vector.  Equality constraint indeces for conFunctions vector
        eqInd
        % std::vector.  Inequality constraint indeces for conFunctions vector
        ineqInd
    end
    
    methods (Access = 'public')
        
        function obj = MiNLPClass(userProblem,Options)
            % Class constructor
            
            % Instantiate a user function manager
            import NLPFunctionGenerator
            obj.userFuncManager = NLPFunctionGenerator(userProblem);
            obj.userProblem = userProblem;
            
            % Call user problem and configure basic problem info
            [numVars,numCons] = obj.userProblem.GetNLPInfo();
            obj.numDecisionVars = numVars;
            obj.totalNumCon = numCons;
            
            % Call user problem and configure bounds info
            [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                obj.userProblem.GetBoundsInfo(numVars,numCons);
            
            obj.varLowerBounds = varLowerBounds;
            obj.varUpperBounds = varUpperBounds;
            obj.conLowerBounds = conLowerBounds;
            obj.conUpperBounds = conUpperBounds;
            
            % Call user problem and configure initial guess
            obj.PrepareInitialGuess()
            obj.Options = Options;
            
            % Initialize counters
            obj.numFuncEvals = 0;
            obj.numDecisionVars = length(obj.decVec);
            
        end
        
        function [decVec,costOut,Converged,OutPut] = Optimize(obj)
            % Optimize the problem
            
            % Evaluate all functions and derivatives
            [obj.costFunction,obj.costJac,numFEval,obj.conFunctions,obj.conJac] = ...
                obj.userFuncManager.EvaluateAllFuncJac(obj.numDecisionVars,obj.decVec,true);
            obj.numFuncEvals = obj.numFuncEvals + numFEval;
            
            % Write data on the config and problem specs to file and disply
            formatstr = '%5i %5i  %2.1e %4i % 4.2e % 4.2e % 8.7e %s';
            obj.ReportNLPConfiguration();
            obj.ReportProblemSpecification();
            obj.WriteIterationHeader();
            
            % Initializations
            method = '';
            Converged = 0;
            numNLPIterations = 0;
            obj.hessLagrangian = eye(obj.numDecisionVars); %  Guess for Hessian of the Lagragian
            obj.lagMultipliers = zeros(obj.totalNumCon,1);
            obj.SetConstraintTypes();
            
            % Line search parameters
            rho   = 0.7;
            sigma = 1.0;
            tau   = 0.5;
            eta   = 0.4;
            mu    = 1;
            iterSinceHeaderWrite = 0;
            
            % Perform the iteration
            while ~Converged && numNLPIterations <= obj.Options.MaxIter && ...
                    obj.numFuncEvals <= obj.Options.MaxFunEvals
                
                %----- Increment counters
                numNLPIterations = numNLPIterations + 1;
                iterSinceHeaderWrite = iterSinceHeaderWrite + 1;
                
                % Solve the QP subproblem to determine the search direction
                [px, ~, plam, ~,activeSet,qpIter] = obj.ComputeSearchDirection();
                
                %% Perform the line search to determine the step length --------
                
                % Calculate the constraint penalty parameter, mu, to result in a
                %      descent direction for the merit function.  We use the L1 merit
                %      function, and calculate mu using N&W 2nd Ed. Eq 18.36.
                cviol = obj.CalcConViolations();
                normc = norm(cviol,1);
                if normc >= 1
                    muRHS = (obj.costJac'*px + (sigma/2)*px'*obj.hessLagrangian*px)/((1 - rho)*normc);
                    if mu <= muRHS
                        mu = muRHS*1.1;
                    elseif mu >= 2 && mu/2 > muRHS
                        mu = mu/2;
                    end
                end
                
                alpha = 1;
                dirDeriv = obj.costJac'*px - mu*normc;   %  Eq. 18.29
                meritF = obj.CalcMeritFunction(obj.costFunction, cviol, mu);
                foundStep = 0;
                fold = obj.costFunction;  xold      = obj.decVec;  Jold      = obj.conJac;
                gradFold  = obj.costJac;
                srchCount = 0;
                xk        = obj.decVec;
                
                %if norm(px) < 10*eps
                %    alpha = norm(px);
                %else
                    while ~foundStep && srchCount < 10 
                        
                        % Increment counter and value of x
                        srchCount = srchCount + 1;
                        obj.stepTaken = alpha*px;
                        obj.decVec = xk + alpha*px;
                        
                        % Evaluate objective,constraints, and merit at at x = x + alpha*p
                        [obj.costFunction,numFEval,obj.conFunctions] = ...
                            obj.userFuncManager.EvaluateFuncsOnly(obj.numDecisionVars,obj.decVec,true);
                        obj.numFuncEvals = obj.numFuncEvals + numFEval;
                        cviol = obj.CalcConViolations();
                        meritFalpha = obj.CalcMeritFunction(obj.costFunction, cviol, mu);
                        
                        % Check sufficient decrease condition
                        if (meritFalpha > meritF + eta*alpha*dirDeriv && abs(dirDeriv) > 1.e-8)
                            alpha = alpha*tau;
                        else
                            foundStep = 1;
                        end
                        
                    end
                %end
                
                %% Evaluate Important Values Such as F, ce, ci at the new step
                [obj.costJac,obj.conJac] = obj.userFuncManager.EvaluateDerivsOnly(...
                    obj.numDecisionVars,obj.decVec,false);
                
                obj.lagMultipliers = obj.lagMultipliers + alpha*plam;     % N&W Eq.
                if ~isempty(obj.lagMultipliers)
                    gradLagrangianXK = gradFold - Jold'*obj.lagMultipliers;
                    obj.gradLagrangian   = obj.costJac - obj.conJac'*obj.lagMultipliers;
                else
                    gradLagrangianXK = gradFold;
                    obj.gradLagrangian = obj.costJac;
                end
                obj.deltaGradLagrangian = obj.gradLagrangian - gradLagrangianXK;
                
                %% Output data, Check convergence, and prepare for next iteration
                
                % Ouput Data for New Iterate if Requested
                maxConViolation = max(obj.CalcConViolations());
                if isempty(maxConViolation)
                    maxConViolation = 0;
                end
                
                if iterSinceHeaderWrite == 10
                    iterSinceHeaderWrite = 0;
                    obj.WriteIterationHeader()
                end
                iterdata = sprintf(formatstr,numNLPIterations,qpIter, alpha, ...
                    length(activeSet), obj.costFunction, maxConViolation,meritFalpha,method);
                disp(iterdata); %#ok<DSPS>
                
                % Check for Convergence
                [Converged] = obj.CheckConvergence(obj.gradLagrangian, fold,...
                    obj.costFunction, obj.decVec, xold,...
                    alpha, maxConViolation, obj.Options);
                
                % Update the Hessian of the Lagrangian
                if Converged == 0
                    method = obj.UpdateHessian();
                end
                
            end
            
            %  If did not converge, determine why and write to screen
            if Converged == 0
                if obj.numFuncEvals == obj.Options.MaxFunEvals
                    fprintf(['\n Optimization Failed \n' ...
                        ' Solution was not found within maximum number \n of ' ...
                        'allowed function evaluations \n']);
                else
                    fprintf(['\n Optimization Failed \n' ...
                        ' Solution was not found within maximum number \n ' ...
                        'of allowed iterations \n']);
                end
            end
            
            costOut = obj.costFunction;
            OutPut.iter   = numNLPIterations;
            OutPut.fevals = obj.numFuncEvals;
            decVec = obj.decVec;
            disp(' ');
            
        end
        
    end
    
    methods (Access = 'private')
        
        function [px, f, plam, flag,activeSet,qpIter] = ComputeSearchDirection(obj)
            
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
            
            % Setup the QP subproblem and call minQP
            if obj.userProblem.conMode == 1
                Aeq = obj.conJac(obj.userProblem.conType == 1,1);
                Je = obj.conJac(obj.userProblem.conType == 2,:);
                A = obj.conJac(obj.userProblem.conType == 3,:);
                Ji = obj.conJac(obj.userProblem.conType == 4,:);
                beq = obj.conLowerBounds(obj.userProblem.conType == 1);
                b = obj.conLowerBounds(obj.userProblem.conType == 3);
                ce =  obj.conFunctions(obj.userProblem.conType == 2);
                ci = obj.conFunctions(obj.userProblem.conType == 4);
                
                % Shift the point of linearization of the linear constraints,
                % to be about the current x value, so we need to update the rhs
                % "b".
                Aqp= [Aeq;Je;A;Ji];
                bqp = [];
                if size(beq,1) > 0
                    bqp = -(Aeq*obj.decVec - beq);
                end
                bqp = [bqp;-ce];
                if size(b,1) > 0
                    bqp = [bqp;-(A*obj.decVec-b)];
                end
                bqp = [bqp;-ci];
                qpOptions.Display = 0;
                
                if obj.debugSearchDir
                    qpOptions.Display = 1;
                end
                if obj.debugSearchDir
                    disp('=========  Input to from QP subproblem ==========')
                    obj.PrintSearchDirDebug(Aqp,bqp)
                end
                %                 if abs(obj.decVec(2) - 1.73205080756888) < 100*eps
                %                     keyboard
                %                 end
                [px, f, lambdaQP, flag,activeSet,qpIter] = MinQP(...
                    0*obj.decVec, obj.hessLagrangian, obj.costJac, Aqp, bqp, obj.eqInd,...
                    obj.ineqInd, [], qpOptions, 2);
                plam = lambdaQP - obj.lagMultipliers;
                
                if obj.debugSearchDir
                    disp('=========  Output from QP subproblem ==========')
                    disp('QP Solution')
                    disp(px)
                    disp('QP function value')
                    disp(f)
                    disp('QP LagMult')
                    disp(plam)
                end
            else
                Aqp = obj.conJac;
                bqp_low = obj.conLowerBounds-obj.conFunctions;
                bqp_high = obj.conUpperBounds-obj.conFunctions;
                qpOpt = MinQPClass(0*obj.decVec, obj.hessLagrangian, obj.costJac, obj.conJac,...
                    obj.conLowerBounds-obj.conFunctions,obj.conUpperBounds-obj.conFunctions,[],2);
                
                
                %                 qpOpt = MinQPClass(obj.decVec, obj.hessLagrangian, obj.costJac, obj.conJac,...
                %                 obj.conLowerBounds,obj.conUpperBounds,[],2);
                
                if obj.debugSearchDir
                    qpOpt.WriteOutput = true;
                end
                if obj.debugSearchDir
                    disp('=========  Input to from QP subproblem ==========')
                    obj.PrintSearchDirDebug(Aqp,bqp_low,bqp_high)
                end
                %                 if abs(obj.decVec(2) - 1.73205080756888) < 100*eps
                %                     keyboard
                %                 end
                [px, f, lambdaQP, flag,qpIter,activeSet] = qpOpt.Optimize();
                plam = lambdaQP - obj.lagMultipliers;
                if obj.debugSearchDir
                    disp('=========  Output to from QP subproblem ==========')
                    disp('QP Solution')
                    disp(px)
                    disp('QP function value')
                    disp(f)
                    disp('QP LagMult')
                    disp(plam)
                end
            end
            
        end
        
        function PrintSearchDirDebug(obj, Aqp, bqplow,bqphigh)
            disp('Dec Vec')
            disp(obj.decVec)
            disp('Lag. Hess')
            disp(obj.hessLagrangian)
            disp('Cost Jac')
            disp(obj.costJac)
            disp('A matrix')
            disp(Aqp)
            disp('b matrix low')
            disp(bqplow)
            if nargin == 4
                disp('b matrix high')
                disp(bqphigh)
            end
        end
        
        function method = UpdateHessian(obj)

            if strcmp(obj.Options.hessUpdateMethod,'DampedBFGS')
                
                %----- The Damped BFGS Update formula. Ref. 1, Procedure 18.2.
                %      The values of 0.2 and 0.8 in the Ref. 1 are changed to
                %      0.1 and 0.9 based on empirical evidence:  they seem to
                %      work better for the test problem set.
                projHess = obj.stepTaken'*obj.hessLagrangian*obj.stepTaken;
                if obj.stepTaken'*obj.deltaGradLagrangian >= 0.1*projHess
                    theta = 1;
                    method = '   BFGS Update';
                else
                    theta = ( 0.9*projHess) / (projHess - obj.stepTaken'*obj.deltaGradLagrangian);
                    method = '   Damped BFGS Update';
                    
                end
                %  Ref 1. Eq. 18.14
                r = theta*obj.deltaGradLagrangian + (1 - theta)*obj.hessLagrangian*obj.stepTaken;
                %  Ref 1. Eq. 18.16
                obj.hessLagrangian = obj.hessLagrangian - ...
                    obj.hessLagrangian*(obj.stepTaken*obj.stepTaken')...
                    *obj.hessLagrangian/projHess + (r*r')/(obj.stepTaken'*r);
                
            elseif strcmp(obj.Options.hessUpdateMethod,'SelfScaledBFGS')
                
                %----- The self-scaled BFGS update.  See section 4.3.3 of
                %      Eldersveld.
                
                projHess = obj.stepTaken'*obj.hessLagrangian*obj.stepTaken;
                if obj.stepTaken'*obj.deltaGradLagrangian >= projHess
                    gamma = 1;
                    method = '   BFGS Update';
                    obj.hessLagrangian   = gamma*obj.hessLagrangian - gamma*...
                        obj.hessLagrangian*(obj.stepTaken*obj.stepTaken')*...
                        obj.hessLagrangian/projHess + obj.deltaGradLagrangian*...
                        obj.deltaGradLagrangian'/(obj.stepTaken'*obj.deltaGradLagrangian);
                elseif 0 < obj.stepTaken'*obj.deltaGradLagrangian && ...
                        obj.stepTaken'*obj.deltaGradLagrangian <= projHess
                    gamma = obj.deltaGradLagrangian'*obj.stepTaken/(projHess);
                    method = '   Self Scaled BFGS ';
                    obj.hessLagrangian   = gamma*obj.hessLagrangian - gamma*...
                        obj.hessLagrangian*(obj.stepTaken*obj.stepTaken')*...
                        obj.hessLagrangian/projHess + obj.deltaGradLagrangian*...
                        obj.deltaGradLagrangian'/(obj.stepTaken'*obj.deltaGradLagrangian);
                else
                    method = '   No Update ';
                end
            end
            obj.hessLagrangian   = (obj.hessLagrangian' + obj.hessLagrangian)*0.5;
        end
        
        function PrepareInitialGuess(obj)
            % Calls user problem to get guess and puts within dec vec bounds
            
            % Call user problem to get the guess
            obj.decVec = obj.userProblem.GetStartingPoint();
            
            % Put the guess within variable bounds
            for varIdx = 1:obj.numDecisionVars
                if obj.decVec(varIdx) > obj.varUpperBounds(varIdx)
                    obj.decVec(varIdx,1) = obj.varUpperBounds(varIdx);
                elseif obj.decVec(varIdx) < obj.varLowerBounds(varIdx)
                    obj.decVec(varIdx,1) = obj.varLowerBounds(varIdx);
                end
            end
        end
        
        function SetConstraintTypes(obj)
            % Creates arrays indicating which elements of the constraint vector
            % of eq. and ineq. contraints
            obj.eqInd = [];
            obj.ineqInd = [];
            for conIdx = 1:obj.totalNumCon
                if obj.userProblem.conType(conIdx) == 1 || obj.userProblem.conType(conIdx) == 2
                    obj.eqInd = [obj.eqInd;conIdx];
                else
                    obj.ineqInd = [obj.ineqInd;conIdx];
                end
            end
        end
        
        function  phi = CalcMeritFunction(obj, f, cviol, mu)
            % Computes the merit function given cost and constraint violations
            
            if strcmp(obj.Options.meritFunction,'NocWright')
                phi = f + mu*sum(cviol);
            end
            
        end
        
        function  [conViolation] = CalcConViolations(obj)
            
            %    This utility calculates the constraint violations for all constraints.
            %    If there is not a violation for a specific constraint, the violation
            %    is set to zero.  For equality constraints, the violation is the
            %    absolute value of the constraint value.  For inequality constraints,
            %    the constraint violation is nonzero only if the constraint value is
            %    negative.  ( If an inequality constraint is positive not violated)
            
            % TODO: fix for performance, elegance
            if obj.userProblem.conMode == 1
                conViolation = zeros(obj.totalNumCon,1);
                conType = obj.userProblem.conType;
                for conIdx = 1:obj.totalNumCon
                    if conType(conIdx) ==1 || conType(conIdx) == 2
                        conViolation(conIdx) = abs(obj.conFunctions(conIdx) - obj.conLowerBounds(conIdx));
                    else
                        if conType(conIdx) == 3
                            conViolation(conIdx) = max(-(obj.conFunctions(conIdx)- obj.conLowerBounds(conIdx)),0);
                        else
                            conViolation(conIdx) = max(-obj.conFunctions(conIdx),0);
                        end
                        
                    end
                end
            else
                conViolation = zeros(obj.totalNumCon,1);
                conType = obj.userProblem.conType;
                for conIdx = 1:obj.totalNumCon
                    % equality constraints so violation is just difference
                    % from bound (upper and lower are the same)
                    if conType(conIdx) ==1 || conType(conIdx) == 2
                        conViolation(conIdx) = abs(obj.conFunctions(conIdx) - obj.conLowerBounds(conIdx));
                    else
                        lowerError = obj.conFunctions(conIdx)- obj.conLowerBounds(conIdx);
                        upperError = obj.conUpperBounds(conIdx) - obj.conFunctions(conIdx);
                        if upperError < 0
                            conViol = abs(upperError);
                        elseif lowerError < 0
                            conViol = abs(lowerError);
                        else
                            conViol = 0;
                        end
                        conViolation(conIdx) = conViol;
                    end
                end
            end
        end
        
        function Converged = CheckConvergence(~,gradLagrangian, f, fnew, x, xnew,...
                alpha, maxConViolation, Options)
            % Checks multiple convergence criteria for algorithm termination
            
            if ~isempty(maxConViolation)
                if maxConViolation > Options.TolCon
                    Converged = 0;
                    return
                end
            end
            
            if (norm(gradLagrangian) < Options.TolGrad)
                Converged = 1;
                fprintf(['\n Optimization Terminated Successfully \n' ...
                    ' Magnitude of gradient of Lagrangian less that Options.tol \n']);
            elseif ( abs(fnew - f) < Options.TolF )
                Converged = 2;
                fprintf(['\n Optimization Terminated Successfully \n' ...
                    ' Absolute value of function improvement \n is less than tolerance']);
            elseif (norm(x - xnew) < Options.TolX)
                Converged = 3;
                fprintf(['\n Optimization Terminated Successfully \n' ...
                    ' Change in x is less than tolerance \n']);
            elseif alpha < Options.TolStep
                Converged = 4;
                fprintf(['\n Optimization Terminated Successfully \n' ...
                    ' Step length is less than tolerance \n']);
            else
                Converged = 0;
            end
            
        end
        
        function ReportNLPConfiguration(obj)
            % Write NLP Configuration to display and/or file
            disp(' NLP Configuration ')
            disp('-------------------')
            disp([ ' Hessian Update Method: ' obj.Options.hessUpdateMethod]);
            disp([ ' Merit Function       : ' obj.Options.meritFunction ]);
            disp(' ')
        end
        
        function ReportProblemSpecification(obj)
            % Write NLP Configuration to display and/or file
            numLinIneqCon = size(find(obj.userProblem.conType == 3),1);
            numLinEqCon = size(find(obj.userProblem.conType == 1),1);
            numNonLinIneqCon = size(find(obj.userProblem.conType == 4),1);
            numNonLinEqCon = size(find(obj.userProblem.conType == 1),1);
            disp(' Problem Specifications ')
            disp('------------------------')
            disp([ ' Number of Variables:                 ' num2str(obj.numDecisionVars,6)]);
            disp([ ' Number of Lin. Ineq. Constraints:    ' num2str(numLinIneqCon,6)]);
            disp([ ' Number of Lin. Eq. Constraints:      ' num2str(numLinEqCon,6)]);
            disp([ ' Number of NonLin. Ineq. Constraints: ' num2str(numNonLinIneqCon,6)]);
            disp([ ' Number of NonLin. Eq. Constraints:   ' num2str(numNonLinEqCon,6)]);
            disp([ ' Total Number of Eq. Constraints:     ' num2str(numLinEqCon + numNonLinEqCon,6)]);
            disp([ ' Total Number of Ineq. Constraints:   ' num2str(numLinIneqCon + numNonLinIneqCon ,6)]);
            disp([ ' Total Number of Constraints:         ' num2str(obj.totalNumCon,6)]);
            disp(' ')
        end
        
        function WriteIterationHeader(~)
            fprintf('Major Minors    Step nCon       Obj    MaxCon  MeritFunction    Method\n');
        end
        
    end
end