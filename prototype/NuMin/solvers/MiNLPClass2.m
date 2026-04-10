classdef MiNLPClass2 < handle
    
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
        % Std vector. Equality constraint indeces for conFunctions vector
        eqInd
        % std::vector. Inequality constraint indeces for conFunctions vector
        ineqInd
        % std:vec of integers. Indicates if constraints are equality or
        % inequality.  1 = equality, 3 = inequality
        constraintTypes
        debugProgress = false;
        isModeElastic = false;
        elasticWeight = 1.0;
        %%  From refactoring for reverse communication
        method = '';
        isConverged
        numNLPIterations
        rho
        sigma
        tau
        eta
        mu
        iterSinceHeaderWrite
        formatstr
        stateMachineState
        
        % From prepare line search
        alpha
        dirDeriv
        meritF
        foundStep
        fold
        xold
        Jold
        gradFold
        srchCount
        xk
        qpExitFlag
        px
        plam
        qpIter
        activeSet
        
        % From line searh
        meritFalpha
        maxConViolation
        
        % From CheckIfFinished
        isFinished
        
        % For state machine
        currentState
        
    end
    
    methods (Access = 'public')
        
        function obj = MiNLPClass2(userProblem,Options)
            % Class constructor
            
            % Instantiate a user function manager
            import NLPFunctionGenerator
            %obj.userFuncManager = NLPFunctionGenerator(userProblem,true,1.0);
            obj.userFuncManager = NLPFunctionGenerator(userProblem);
            obj.userProblem = userProblem;
            
            SetNLPAndBoundsInfo(obj)
            
            % Call user problem and configure initial guess
            obj.PrepareInitialGuess()
            obj.Options = Options;
            
            % Initialize counters
            obj.numFuncEvals = 0;
            obj.numDecisionVars = length(obj.decVec);
            obj.currentState = 'Instantiated';
            
        end
        
        function PrepareToOptimize(obj)
            % Perform last minute init before iterating
            
            % Evaluate all functions and derivatives
            [obj.costFunction,obj.costJac,numFEval,obj.conFunctions,obj.conJac] = ...
                obj.userFuncManager.EvaluateAllNLPFuncJac(obj.numDecisionVars,obj.decVec,true);
            obj.numFuncEvals = obj.numFuncEvals + numFEval;
            
            % Write data on the config and problem specs to file and disply
            obj.formatstr = '%5i %5i  %2.1e %4i % 4.2e % 4.2e % 8.7e %s';
            obj.ReportNLPConfiguration();
            obj.ReportProblemSpecification();
            obj.WriteIterationHeader();
            
            % Initializations
            obj.method = '';
            obj.isConverged = 0;
            obj.numNLPIterations = 0;
            obj.hessLagrangian = eye(obj.numDecisionVars); %  Guess for Hessian of the Lagragian
            obj.lagMultipliers = zeros(obj.totalNumCon,1);
            obj.SetConstraintTypes();
            
            % Line search parameters
            obj.rho   = 0.7;
            obj.sigma = 1.0;
            obj.tau   = 0.5;
            obj.eta   = 0.4;
            obj.mu    = 1;
            obj.iterSinceHeaderWrite = 0;
            obj.currentState = 'ReadyToOptimize';
        end
        
        function [decVec,costOut,exitFlag,OutPut] = PrepareFailedRunOutput(obj)
            
            [decVec,costOut,~,OutPut] = PrepareOutput(obj);
            exitFlag = 0;
            if strcmp(obj.Options.Display,'iter')
                fprintf('\n Optimization Failed due to failure in QP subproblem \n ');
            end
            
        end
        
        function RespondToData(obj)
            if strcmp(obj.currentState,'Initialized')
                obj.PrepareToOptimize();
                return
            end
            if strcmp(obj.currentState,'ReadyToOptimize')
                obj.PrepareLineSearch()
                return
            end
            if strcmp(obj.currentState,'ReadyForLineSearch') ||...
                    strcmp(obj.currentState,'LineSearchIteration');
                obj.TestStep()
                if strcmp(obj.currentState,'LineSearchConverged')
                    obj.PrepareForNextIteration();
                    obj.CheckIfFinished()
                    if ~obj.isFinished
                        obj.PrepareLineSearch()
                    end
                end
                return
            end
            if strcmp(obj.currentState,'LineSearchConverged') || ...
                    strcmp(obj.currentState,'Finished');
                obj.PrepareForNextIteration()
                obj.CheckIfFinished()
                if ~obj.isFinished
                    obj.PrepareLineSearch()
                end
                return
            end
            x =  6;
        end
        
        function [status,funTypes,decVec,isNewX,userFunPointer] = CheckStatus(obj)
            userFunPointer = obj.userProblem;
            if strcmp(obj.currentState,'Instantiated')
                status = 1;
                funTypes = 0;
                decVec = obj.decVec;
                isNewX = false;
                return
            end
            if strcmp(obj.currentState,'ReadyToOptimize')
                status = 1;
                funTypes = 2;
                decVec = obj.decVec;
                isNewX = true;
                return
            end
            if strcmp(obj.currentState,'ReadyForLineSearch') ||...
                    strcmp(obj.currentState,'LineSearchIteration');
                status = 1;
                funTypes = 1;
                decVec = obj.decVec;
                isNewX = true;
                return
            end
            
            if strcmp(obj.currentState,'LineSearchConverged')
                status = 1;
                funTypes = 2;
                decVec = obj.decVec;
                isNewX = true;
                return
            end
            
            if strcmp(obj.currentState,'Finished')
                status = 0;
                funTypes = 0;
                decVec = obj.decVec;
                isNewX = false;
                return
            end
            x = 6;
        end
        
        function [decVec,costOut,exitFlag,OutPut] = Optimize(obj)
            
            % Perform the last minute intitializations.
            obj.PrepareToOptimize();
            
            % Perform the iteration
            obj.isFinished = 0;
            while ~obj.isFinished
                
                obj.PrepareLineSearch();
                
                % Test to see if QP failed after elastic mode was tried
                if obj.qpExitFlag <= 0
                    [decVec,costOut,exitFlag,OutPut] = obj.PrepareFailedRunOutput();
                    return
                end
                
                % Keep testing the step until converged or too many
                % iterations
                while ~obj.foundStep && obj.srchCount < 10
                    obj.TestStep()
                end
                
                % Updates for next iteration
                obj.PrepareForNextIteration()
                
                % Check to see if the run is finished (converged, failed,
                % etc.)
                obj.CheckIfFinished()
                
            end
            
            % Prepare data for return.  Run is complete.
            [decVec,costOut,exitFlag,OutPut] = obj.PrepareOutput();
            
        end
        
        function [decVec,costOut,exitFlag,OutPut] = PrepareOutput(obj)
            %  If did not converge, determine why and write to screen
            if obj.isConverged == 0
                if obj.numFuncEvals >= obj.Options.MaxFunEvals
                    fprintf(['\n Optimization Failed \n' ...
                        ' Solution was not found within maximum number \n of ' ...
                        'allowed function evaluations \n']);
                end
                if obj.numNLPIterations >= obj.Options.MaxIter
                    fprintf(['\n Optimization Failed \n' ...
                        ' Solution was not found within maximum number \n ' ...
                        'of allowed iterations \n']);
                end
            end
            exitFlag = obj.isConverged;
            costOut = obj.costFunction;
            OutPut.iter   = obj.numNLPIterations;
            OutPut.fevals = obj.numFuncEvals;
            decVec = obj.decVec(1:obj.userFuncManager.GetNumUserDecisionVars());
            disp(' ');
            obj.currentState = 'Finished';
        end
        
        function PrepareLineSearch(obj)
            
            if obj.debugProgress
                disp(['Entering PrepareLineSearch(), current state is : ' obj.currentState]);
            end
            %----- Increment counters
            obj.numNLPIterations = obj.numNLPIterations + 1;
            obj.iterSinceHeaderWrite = obj.iterSinceHeaderWrite + 1;
            
            % Call QP problem to solve the QP problem, the set data to
            % get ready for the line search
            
            % Solve the QP subproblem to determine the search direction
            [obj.px, ~, obj.plam, obj.qpExitFlag,obj.activeSet,obj.qpIter] = obj.ComputeSearchDirection();
            
            cviol = obj.CalcConViolations();
            normc = norm(cviol,1);
            if normc >= 1
                muRHS = (obj.costJac'*obj.px + (obj.sigma/2)*obj.px'*obj.hessLagrangian*obj.px)/...
                    ((1 - obj.rho)*normc);
                if obj.mu <= muRHS
                    obj.mu = muRHS*1.1;
                elseif obj.mu >= 2 && obj.mu/2 > muRHS
                    obj.mu = obj.mu/2;
                end
            end
            
            obj.alpha = 1;
            obj.dirDeriv = obj.costJac'*obj.px - obj.mu*normc;   %  Eq. 18.29
            obj.meritF = obj.CalcMeritFunction(obj.costFunction, cviol, obj.mu);
            obj.foundStep = 0;
            obj.fold = obj.costFunction;
            obj.xold = obj.decVec;
            obj.Jold = obj.conJac;
            obj.gradFold  = obj.costJac;
            obj.srchCount = 0;
            obj.xk        = obj.decVec;
            obj.currentState = 'ReadyForLineSearch';
            
            if obj.debugProgress
                disp(['Exiting PrepareLineSearch(), current state is : ' obj.currentState]);
            end
        end
        
        function TestStep(obj)
            %% Perform the line search to determine the step length --------
            
            % Calculate the constraint penalty parameter, mu, to result in a
            %      descent direction for the merit function.  We use the L1 merit
            %      function, and calculate mu using N&W 2nd Ed. Eq 18.36.
            
            if obj.debugProgress
                disp(['Entering TestStep(), current state is : ' obj.currentState]);
            end
            
            % Increment counter and value of x
            obj.srchCount = obj.srchCount + 1;
            obj.stepTaken = obj.alpha*obj.px;
            obj.decVec = obj.xk + obj.alpha*obj.px;
            
            % Evaluate objective,constraints, and merit at at x = x + alpha*p
            [obj.costFunction,numFEval,obj.conFunctions] = ...
                obj.userFuncManager.EvaluteNLPFunctionsOnly(obj.numDecisionVars,...
                obj.decVec,true);
            obj.numFuncEvals = obj.numFuncEvals + numFEval;
            cviol = obj.CalcConViolations();
            obj.meritFalpha = obj.CalcMeritFunction(obj.costFunction, cviol, obj.mu);
            
            % Check sufficient decrease condition
            if (obj.meritFalpha > obj.meritF + obj.eta*obj.alpha*obj.dirDeriv && abs(obj.dirDeriv) > 1.e-8)
                obj.alpha = obj.alpha*obj.tau;
                obj.currentState = 'LineSearchIteration';
            else
                obj.foundStep = 1;
                obj.currentState = 'LineSearchConverged';
            end
            
            
            if obj.debugProgress
                disp(['Exiting TestStep(), current state is : ' obj.currentState]);
            end
            
            
        end
        
        function PrepareForNextIteration(obj)
            %% Evaluate Important Values Such as F, ce, ci at the new step
            [obj.costJac,obj.conJac] = obj.userFuncManager.EvaluateNLPDerivsOnly(...
                obj.numDecisionVars,obj.decVec,false);
            
            if obj.debugProgress
                disp(['Entering PrepareForNextIteration(), current state is : ' obj.currentState]);
            end
            
            if obj.debugProgress
                disp('Cost Function')
                disp(obj.costFunction)
                disp('Constraint Functions')
                disp(obj.conFunctions')
            end
            
            obj.lagMultipliers = obj.lagMultipliers + obj.alpha*obj.plam;     % N&W Eq.
            if ~isempty(obj.lagMultipliers)
                gradLagrangianXK = obj.gradFold - obj.Jold'*obj.lagMultipliers;
                obj.gradLagrangian   = obj.costJac - obj.conJac'*obj.lagMultipliers;
            else
                gradLagrangianXK = obj.gradFold;
                obj.gradLagrangian = obj.costJac;
            end
            obj.deltaGradLagrangian = obj.gradLagrangian - gradLagrangianXK;
            
            %% Output data, Check convergence, and prepare for next iteration
            
            % Ouput Data for New Iterate if Requested
            conViolations = obj.CalcConViolations();
            obj.maxConViolation = max(obj.CalcConViolations());
            if isempty(obj.maxConViolation)
                obj.maxConViolation = 0;
            end
            
            if obj.iterSinceHeaderWrite == 10
                obj.iterSinceHeaderWrite = 0;
                obj.WriteIterationHeader()
            end
            
            % Compute user constraints and cost without elastic
            % contributions for reporting.
            if obj.isModeElastic
                conOut = obj.userFuncManager.MaxUserConViolation(conViolations,obj.decVec);
                costOut = obj.userFuncManager.ShiftCost(obj.costFunction,obj.decVec);
            else
                costOut = obj.costFunction;
                conOut  = obj.maxConViolation;
            end
            iterdata = sprintf(obj.formatstr,obj.numNLPIterations,obj.qpIter, obj.alpha, ...
                length(obj.activeSet), costOut, conOut,obj.meritFalpha,obj.method);
            disp(iterdata); %#ok<DSPS>
            obj.currentState = 'ReadyForConvergenceTest';
            
            if obj.debugProgress
                disp(['Exiting PrepareForNextIteration(), current state is : ' obj.currentState]);
            end
            
        end
        
        function CheckIfFinished(obj)
            
            if obj.debugProgress
                disp(['Entering  CheckIfFinished(), current state is : ' obj.currentState]);
            end
            
            % Check for Convergence
            [obj.isConverged] = obj.CheckConvergence(obj.gradLagrangian, obj.fold,...
                obj.costFunction, obj.decVec, obj.xold,...
                obj.alpha, obj.maxConViolation, obj.Options);
            
            % Update the Hessian of the Lagrangian
            if obj.isConverged == 0
                obj.method = obj.UpdateHessian();
            end
            
            % Test for various reasons to stop iteration and set flag
            % to stop if one occurs
            if ~obj.isConverged && obj.numNLPIterations <= obj.Options.MaxIter && ...
                    obj.numFuncEvals <= obj.Options.MaxFunEvals
                obj.isFinished = false;
                obj.currentState = 'LineSearchIteration';
            else
                obj.isFinished = true;
                obj.currentState = 'Finished';
            end
            
            if obj.debugProgress
                disp(['Exiting  CheckIfFinished(), current state is : ' obj.currentState]);
            end
            
        end
        
        function SetNLPAndBoundsInfo(obj)
            
            % Call user problem and configure basic problem info
            [numVars,numCons] = obj.userFuncManager.GetNLPInfo();
            obj.numDecisionVars = numVars;
            obj.totalNumCon = numCons;
            
            % Call user problem and configure bounds info
            [varLowBounds,varUpBounds,conLowBounds,conUpBounds] = ...
                obj.userFuncManager.GetNLPBoundsInfo();
            obj.varLowerBounds = varLowBounds;
            obj.varUpperBounds = varUpBounds;
            obj.conLowerBounds = conLowBounds;
            obj.conUpperBounds = conUpBounds;
        end
        
    end
    
    methods (Access = 'private')
        
        function [px, f, plam, exitFlag,activeSet,qpIter] = ComputeSearchDirection(obj)
            
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
            
            if obj.debugProgress
                disp('===== Inputs to QP Subproblem for Iteration ')
                disp('Hessian of Lagrangian')
                obj.hessLagrangian
                disp('Cost Jacobian')
                obj.costJac
                disp('Constraint Jacobian')
                obj.conJac
                disp('QP Constraint lower bounds')
                disp(obj.conLowerBounds-obj.conFunctions)
                disp('QP Constraint upper bounds')
                disp(obj.conUpperBounds-obj.conFunctions)
            end
            
            qpOpt = MinQPClass(0*obj.decVec, obj.hessLagrangian, obj.costJac, obj.conJac,...
                obj.conLowerBounds-obj.conFunctions,obj.conUpperBounds-obj.conFunctions,[],2);
            
            if obj.debugSearchDir
                disp('=========  Input to from QP subproblem ==========')
                obj.PrintSearchDirDebug(obj.conJac,obj.conLowerBounds-obj.conFunctions,...
                    obj.conUpperBounds-obj.conFunctions)
            end
            
            % Call the QP solver
            [px, f, lambdaQP, exitFlag,qpIter,activeSet] = qpOpt.Optimize();
            
            if obj.isModeElastic && exitFlag ~=1
                % Something went really wrong in QP.  MinQP is not robust
                % enough if this happens.
                plam = -obj.lagMultipliers;
                return
            end
            
            % QP Failed. Switch to elastic Mode
            if exitFlag ~= 1
                obj.PrepareElasticMode();
                qpOpt = MinQPClass(0*obj.decVec, obj.hessLagrangian, obj.costJac, obj.conJac,...
                    obj.conLowerBounds-obj.conFunctions,obj.conUpperBounds-obj.conFunctions,[],2);
                [px, f, lambdaQP, exitFlag, qpIter,activeSet] = qpOpt.Optimize();
                obj.lagMultipliers = [obj.lagMultipliers;zeros(obj.userFuncManager.GetNumElasticVars,1)];
            end
            
            % Compute the lagrange multipliers
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
        
        function PrepareElasticMode(obj)
            
            % Switch the user function generator to use elastic mode
            obj.isModeElastic = true;
            oldDecVec = obj.decVec;
            obj.userFuncManager = NLPFunctionGenerator(obj.userProblem,true,obj.elasticWeight);
            
            % Update the bounds data for elastic mode
            obj.SetNLPAndBoundsInfo();
            obj.hessLagrangian = eye(obj.numDecisionVars);
            
            % Set up the decision vector
            obj.decVec = obj.userFuncManager.GetNLPStartingPoint();
            obj.decVec(1:obj.userFuncManager.GetNumUserDecisionVars(),1) = oldDecVec;
            
            % Evaluate functions and derivatives and determine constraint types
            [obj.costFunction,obj.costJac,~,obj.conFunctions,obj.conJac] = ...
                obj.userFuncManager.EvaluateAllNLPFuncJac(obj.numDecisionVars,obj.decVec,true);
            obj.SetConstraintTypes();
            
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
            obj.decVec = obj.userFuncManager.GetNLPStartingPoint();
            
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
            obj.constraintTypes = zeros(obj.totalNumCon,1);
            for conIdx = 1:obj.totalNumCon
                if abs(obj.conUpperBounds(conIdx) - obj.conLowerBounds(conIdx)) >= obj.Options.TolCon
                    obj.ineqInd = [obj.ineqInd;conIdx];
                    obj.constraintTypes(conIdx,1) = 3;
                else
                    obj.eqInd = [obj.eqInd;conIdx];
                    obj.constraintTypes(conIdx,1) = 1;
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
            
            conViolation = zeros(obj.totalNumCon,1);
            %conType = obj.userProblem.conType;
            for conIdx = 1:obj.totalNumCon
                % equality constraints so violation is just difference
                % from bound (upper and lower are the same)
                if obj.constraintTypes(conIdx) ==1 || obj.constraintTypes(conIdx) == 2
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
        
        function Converged = CheckConvergence(obj,gradLagrangian, f, fnew, x, xnew,...
                alpha, maxConViolation, Options)
            % Checks multiple convergence criteria for algorithm termination
            
            % If we are in elastic mode, check if slack variables are
            % zero or we are at the max elastic weight.  If not, we are not
            % converged yet.
            if obj.isModeElastic
                maxElasticVar = obj.userFuncManager.GetMaxElasticVar(obj.decVec);
                if maxElasticVar > Options.TolX
                    % Mode is elastic, and elastic variables are active
                    % (non-zero) adding contributions to objective and
                    % constriants
                    isElasticModeActive = true;
                else
                    isElasticModeActive = false;
                end
                if maxElasticVar > 1e-10 && obj.elasticWeight < obj.Options.MaxElasticWeight
                    obj.elasticWeight = 10.0*obj.elasticWeight;
                    obj.userFuncManager.SetElasticWeight(obj.elasticWeight)
                    Converged = false;
                    return
                end
            end
            
            % Constraints are not satisfied, cannot be converged
            if ~isempty(maxConViolation)
                if maxConViolation > Options.TolCon
                    Converged = 0;
                    return
                end
            end
            
            % Check convergence criteria
            if (norm(gradLagrangian) < Options.TolGrad)
                Converged = 1;
                messageHow = ' Magnitude of gradient of Lagrangian less that Options.tol \n';
            elseif ( abs(fnew - f) < Options.TolF )
                Converged = 2;
                messageHow = ' Absolute value of function improvement is less than tolerance\n';
            elseif (norm(x - xnew) < Options.TolX)
                Converged = 3;
                messageHow = ' Change in x is less than tolerance \n';
            elseif alpha < Options.TolStep
                Converged = 4;
                messageHow =  ' Step length is less than tolerance \n';
            else
                Converged = 0;
            end
            
            % If problem converged, and user requested output. Write
            % convergence type to console
            if Converged > 0 && strcmp(obj.Options.Display,'iter')
                if obj.isModeElastic && isElasticModeActive
                    messageMode = '\n The problem appears to be infeasible... contraint violations minimized \n';
                else
                    messageMode = '\n Optimization Terminated Successfully \n';
                end
                fprintf([messageMode messageHow])
            end
            
        end
        
        function ReportNLPConfiguration(obj)
            % Write NLP Configuration to display and/or file
            disp(' NLP Configuration ')
            disp('-------------------')
            disp([ ' Hessian Update Method: ' obj.Options.hessUpdateMethod]);
            disp([ ' Merit Function       : ' obj.Options.meritFunction ]);
            disp([ ' MaxIter              : ' num2str(obj.Options.MaxIter)]);
            disp([ ' MaxFunEvals          : ' num2str(obj.Options.MaxFunEvals)]);
            disp([ ' TolCon               : ' num2str(obj.Options.TolCon)]);
            disp([ ' TolX                 : ' num2str(obj.Options.TolX)]);
            disp([ ' TolF                 : ' num2str(obj.Options.TolF)]);
            disp([ ' TolGrad              : ' num2str(obj.Options.TolGrad)]);
            disp([ ' TolStep              : ' num2str(obj.Options.TolStep)]);
            disp([ ' Display              : ' obj.Options.Display]);
            disp([ ' MaxElasticWeight     : ' num2str(obj.Options.MaxElasticWeight)]);
            disp(' ')
        end
        
        function ReportProblemSpecification(obj)
            % Write NLP Configuration to display and/or file
            %             numLinIneqCon = size(find(obj.userProblem.conType == 3),1);
            %             numLinEqCon = size(find(obj.userProblem.conType == 1),1);
            %             numNonLinIneqCon = size(find(obj.userProblem.conType == 4),1);
            %             numNonLinEqCon = size(find(obj.userProblem.conType == 1),1);
            disp(' Problem Specifications ')
            disp('------------------------')
            disp([ ' Number of Variables:      ' ...
                num2str(obj.userFuncManager.GetNumUserDecisionVars(),6)]);
            disp([ ' Number of Constraints:    ' ...
                num2str(obj.userFuncManager.GetNumUserConstraints(),6)]);
            %             disp([ ' Number of Lin. Eq. Constraints:      ' num2str(numLinEqCon,6)]);
            %             disp([ ' Number of NonLin. Ineq. Constraints: ' num2str(numNonLinIneqCon,6)]);
            %             disp([ ' Number of NonLin. Eq. Constraints:   ' num2str(numNonLinEqCon,6)]);
            %             disp([ ' Total Number of Eq. Constraints:     ' num2str(numLinEqCon + numNonLinEqCon,6)]);
            %             disp([ ' Total Number of Ineq. Constraints:   ' num2str(numLinIneqCon + numNonLinIneqCon ,6)]);
            %             disp([ ' Total Number of Constraints:         ' num2str(obj.totalNumCon,6)]);
            disp(' ')
        end
        
        function WriteIterationHeader(~)
            fprintf('Major Minors    Step nCon       Obj    MaxCon  MeritFunction    Hess. Update\n');
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
        
    end
end


%             if obj.isModeElastic || exitFlag ~=1
%                             H,
%             f,
%             A = Aqp;
%             B,
%             lb,
%             ub,
%             X,
%             neqcstr
%             verbosity = -1;
%             caller 'nlconst';
%             ncstr = size(A,1);
%             numberOfVariables = obj.numDecisionVars;
%
%             ACTIND = []
%             phaseOneTotalScaling = 0;
%             options.MaxIter = 50;
%             options.TolCon = 1e-8;
%             [X,lambda,exitflag,output,how,ACTIND,msg] = ...
%             qpsub(H,f,A,B,lb,ub,X,neqcstr,verbosity,caller,ncstr, ...
%             numberOfVariables,options,ACTIND,phaseOneTotalScaling)
%
%                QP failed, something is really wrong.
%                plam = [];
%                return
%             end
%  Already in elastic mode and still failed to find a step.
%  Terminate the optimization
