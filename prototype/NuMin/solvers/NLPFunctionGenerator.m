classdef NLPFunctionGenerator < handle
    %NLPFunctionGenerator Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % user function pointer.  Pointer to object that the user provides
        userFunction
        % Integer.  Number of decision variables in the user's problem
        numUserDecisionVars
        % Integer.  Number of (non-bound) constraints in the user's problem
        numUserConstraints
        % Integer.  Number of decision variables with non -inf to inf bounds
        numBoundCons
        % Integer.  Total number of decision varialbes (user + elastic)
        totalNumDecisionVars
        % Integer. Total number of constraints (user con + bound con +
        % elastic var con)
        totalNumContraints
        % Integer. Number of elastic mode decision variables
        numElasticVars = 0;
        % Integer. Number of elasetic mode constraints
        numElasticConstraints = 0;
        % Bool.  Flag indicating if problem is in elastic mode
        isModeElastic = false;
        % Vector of reals.  Vector of all constraints (user cons + bound cons)
        conValues
        % Real Vector.  The Jacobian of the cost function
        costJac
        % Real Matrix. The Jacobian of the constraint functions.
        conJac
        % Real Vector. The lower bounds on NLP constraints.
        nlpConLowerBounds
        % Real Vector. The upper bounds on NLP constraints.
        nlpConUpperBounds
        % Real Vector. The lower bounds on NLP decision variables
        nlpVarLowerBounds
        % Real Vector. The upper bounds on NLP decision variables
        nlpVarUpperBounds
        % Real Matrix. A matrix for computing bound constraints
        boundAMatrix
        % Ingeter.  Index into conValues array indicating where user cons start
        userConJacRowStartIdx
        % Ingeter.  Index into conValues array indicating where user cons end
        userConJacRowStopIdx
        % Integer. Index into conValues array indicating where user vars start
        userConJacColStartIdx
        % Integer. Index into conValues array indicating where user vars end
        userConJacColStopIdx
        % Real. Elastic weight parameter
        elasticWeight
        % Integer. Index into decision vector indicating where elastic V
        % vars start
        elasticVarVStartIdx
        % Integer. Index into decision vector indicating where elastic V
        % vars end
        elasticVarVStopIdx
        % Integer. Index into decision vector indicating where elastic W
        % vars start
        elasticVarWStartIdx
        % Integer. Index into decision vector indicating where elastic W
        % vars end
        elasticVarWStopIdx
        % Integer Array. Index into conValues indicating constraints
        % associated with user decision variable bounds
        userVarBoundConIdxs
    end
    
    methods
        
        function obj = NLPFunctionGenerator(userFunction,isModeElastic,elasticWeight)
            % NOTE:  Elastic mode inputs/outputs are probably best handled
            % by overloaded constructor in C++.  Not supported in MATLAB.
            
            % Set the pointer to the user function
            obj.userFunction = userFunction;
            
            % Call the user function to get info needed for initialization
            try
                [numVars,numCons] = obj.userFunction.GetNLPInfo();
                obj.numUserDecisionVars = numVars;
                obj.numUserConstraints = numCons;
            catch
                error('Call to ::GetNLPInfo() failed')
            end
            
            if nargin > 1
                if isModeElastic
                    obj.PrepareElasticMode(elasticWeight);
                end
            end
            
            % Perform basic checks on the user implementation
            ValidateUserProblem(obj)
            
            %  Set up the array sizes based on user inputs, mode, and
            %  bounds constraints
            PrepareArrays(obj)

        end
        
        function ValidateUserProblem(obj)
            
            % Check starting point function call and dimensions of the
            % starting point
            try
                startingPoint = obj.userFunction.GetStartingPoint();
            catch
                error('User Function ::GetStartingPointMethod() failed to evaluate.')
            end
            if size(startingPoint,2) ~= 1 || size(startingPoint,1) ~= obj.numUserDecisionVars
                error(['Starting Point dimension is not consistent ' ...
                    'with number of decision variables'])
            end
            
            % Check bound vector dimensions
            try
                [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                    obj.userFunction.GetBoundsInfo(obj.numUserDecisionVars,obj.numUserConstraints);
            catch
                error('User Function ::GetBoundsInfo() failed to evaluate.')
            end
            if size(varLowerBounds,2) ~= 1 || size(varLowerBounds,1) ~= obj.numUserDecisionVars
                error(['Variable lower bounds dimension is not consistent ' ...
                    'with number of decision variables'])
            end
            if size(varUpperBounds,2) ~= 1 || size(varUpperBounds,1) ~= obj.numUserDecisionVars
                error(['Variable upper bounds dimension is not consistent ' ...
                    'with number of decision variables'])
            end
            if size(conLowerBounds,2) ~= 1 || size(conLowerBounds,1) ~= obj.numUserConstraints
                error(['Constraint lower bounds dimension is not consistent ' ...
                    'with number of constraints'])
            end
            if size(conUpperBounds,2) ~= 1 || size(conUpperBounds,1) ~= obj.numUserConstraints
                error(['Constraint upper bounds dimension is not consistent ' ...
                    'with number of constraints'])
            end
            
            % Call cost fuction
            isNewX = true;
            try
                costFunction = obj.userFunction.EvaluateCostFunc(obj.numUserDecisionVars,...
                startingPoint,isNewX);
            catch
                error('User Function ::EvaluateCostFunc() failed to evaluate.')
            end
            
            % Call cost Jacobian and test dimentions
            isNewX = false;
            try
                costJacobian= obj.userFunction.EvaluateCostJac(obj.numUserDecisionVars,...
                startingPoint,isNewX);
            catch
                error('User Function ::EvaluateCostJac() failed to evaluate.')
            end
            if size(costJacobian,2) ~= 1 || size(costJacobian,1) ~= obj.numUserDecisionVars
                error(['Cost derivative dimension is not consistent ' ...
                    'with number of decision variables'])
            end
            
            % Call con functions
            try
                conFunctions = obj.userFunction.EvaluateConFunc(obj.numUserDecisionVars,...
                startingPoint,isNewX);
            catch
                error('User Function ::EvaluateConFunc() failed to evaluate.')
            end
            if size(conFunctions,2) ~= 1 || size(conFunctions,1) ~= obj.numUserConstraints
                error(['Constraint vector dimension is not consistent ' ...
                    'with number of constraints'])
            end
            
            % Call con Jacobian and test dimenions
            try
                conJacobian = obj.userFunction.EvaluateConJac(obj.numUserDecisionVars,...
                startingPoint,isNewX);
            catch
                error('User Function ::EvaluateConJac() failed to evaluate.')
            end
            if size(conJacobian,2) ~= obj.numUserDecisionVars || ...
                    size(conJacobian,1) ~= obj.numUserConstraints
                error(['Constraint Jacobian dimension is not consistent ' ...
                    'with number of decision variables and constraints'])
            end

        end
        
        function PrepareElasticMode(obj,elasticWeight)
            % Some initialization for elastic mode
            obj.isModeElastic = true;
            obj.elasticWeight = elasticWeight;
            obj.elasticVarVStartIdx = obj.numUserDecisionVars + 1;
            obj.elasticVarVStopIdx = obj.elasticVarVStartIdx + obj.numUserConstraints - 1;
            obj.elasticVarWStartIdx = obj.elasticVarVStopIdx + 1;
            obj.elasticVarWStopIdx = obj.elasticVarWStartIdx + obj.numUserConstraints - 1;
        end
        
        function PrepareArrays(obj)
            
            % Call the user function to get properties
            [userVarLowerBounds,userVarUpperBounds,userConLowerBounds,userConUpperBounds] = ...
                obj.userFunction.GetBoundsInfo();
            
            %% Configure the lower and upper variable bounds constraints
            % For variables that have that have user provided bounds.
            % If the constraint  bounts are -inf to inf, discard to avoid
            % numerical issues in matrix factorizations etc.
            if isempty(userVarLowerBounds)
                userVarLowerBounds = -Inf*ones(obj.numUserDecisionVars,1);
            end
            if isempty(userVarUpperBounds)
                userVarUpperBounds = Inf*ones(obj.numUserDecisionVars,1);
            end
            
            %% Only apply bounds on variables that have them to avoid numerical issues.
            % So only retain bounds that are not -inf < var < inf.
            obj.userVarBoundConIdxs = [];
            for conIdx = 1:obj.numUserDecisionVars
                if userVarLowerBounds(conIdx) == -Inf && userVarUpperBounds(conIdx) == Inf
                else
                    obj.userVarBoundConIdxs = [obj.userVarBoundConIdxs conIdx];
                end
            end
            numVarIdent = eye(obj.numUserDecisionVars);
            AMatBound = numVarIdent(obj.userVarBoundConIdxs,:);
            userNLPVarLowerBounds = userVarLowerBounds(obj.userVarBoundConIdxs);
            userNLPVarUpperBounds = userVarUpperBounds(obj.userVarBoundConIdxs);
            if ~isempty(AMatBound)
                obj.numBoundCons = size(AMatBound,1);
            else
                obj.numBoundCons = 0;
            end
            obj.boundAMatrix = AMatBound;
            
            %% Set variables based on the mode
            if obj.isModeElastic
                obj.numElasticVars = 2*obj.numUserConstraints;
                obj.numElasticConstraints = 2*obj.numUserConstraints;
                elasticVarLowerBounds = zeros(obj.numElasticVars,1);
                elasticVarUpperBounds = realmax*ones(obj.numElasticVars,1);
            else
                obj.numElasticVars = 0;
                obj.numElasticConstraints = 0;
                elasticVarLowerBounds = [];
                elasticVarUpperBounds = [];
            end
            
            %% Dimension the Jacobian arrays and fill in constant terms
            % associated with bound constraints and elastic variables
            numRowsInJacobian = obj.numUserConstraints + + obj.numBoundCons + obj.numElasticVars;
            numColsInConJacobian = obj.numUserDecisionVars  + obj.numElasticVars;
            obj.costJac = zeros(numColsInConJacobian,1);
            obj.conJac = zeros(numRowsInJacobian,numColsInConJacobian);
            obj.userConJacRowStartIdx = 1;
            obj.userConJacRowStopIdx = obj.numUserConstraints;
            obj.userConJacColStartIdx = 1;
            obj.userConJacColStopIdx = obj.numUserDecisionVars;
            rowStart = obj.numUserConstraints+1;
            rowStop = rowStart+size(AMatBound,1) - 1;
            colStart = 1;
            colStop = obj.numUserDecisionVars;
            obj.conJac(rowStart:rowStop,colStart:colStop) = AMatBound;
            
            %% If we are in elastic mode, fill in parts of jacobians associated
            % with elastic variables
            if obj.isModeElastic
                rowStartIdx = 1;
                rowStopIdx = obj.numUserConstraints;
                obj.conJac(rowStartIdx:rowStopIdx,obj.elasticVarVStartIdx:obj.elasticVarVStopIdx)...
                    = -eye(obj.numUserConstraints);
                obj.conJac(rowStartIdx:rowStopIdx,obj.elasticVarWStartIdx:obj.elasticVarWStopIdx)...
                    = eye(obj.numUserConstraints);
                rowStartIdx = obj.numUserConstraints + obj.numBoundCons + 1;
                rowStopIdx = rowStartIdx + obj.numElasticVars - 1;
                obj.conJac(rowStartIdx:rowStopIdx,obj.elasticVarVStartIdx:obj.elasticVarWStopIdx)...
                    = eye(obj.numElasticVars);
                obj.boundAMatrix(:,obj.numUserDecisionVars+1:obj.totalNumDecisionVars);
            end
            
            % Fill in the NLP bounds arrays
            obj.nlpConLowerBounds = [userConLowerBounds;userNLPVarLowerBounds;elasticVarLowerBounds];
            obj.nlpConUpperBounds = [userConUpperBounds;userNLPVarUpperBounds;elasticVarUpperBounds];
            obj.nlpVarLowerBounds = [userVarLowerBounds;elasticVarLowerBounds];
            obj.nlpVarUpperBounds = [userVarUpperBounds;elasticVarUpperBounds];
            
        end
        
        %% User Function Interfaces
        function [fun,funJac,numFunEvals] = EvaluateCostAndJac(obj,numVars,decVector,isNewX)
            % Evaluates the user's cost and Jacobian
            fun = obj.userFunction.EvaluateCostFunc(numVars,decVector,isNewX);
            funJac = obj.userFunction.EvaluateCostJac(numVars,decVector,isNewX);
            numFunEvals = 1;
        end
        
        function [conFunc,conJac,numFunEvals] = EvaluateConAndJac(obj,numVars,decVector,isNewX)
            % Evaluates the user's constraints and Jacobian
            conFunc = obj.userFunction.EvaluateConFunc(numVars,decVector,isNewX);
            conJac = obj.userFunction.EvaluateConJac(numVars,decVector,isNewX);
            numFunEvals = 1;
        end
        
        function [fun,funJac,numFunEvals,conFunc,conJac,numGEvals] = ...
                EvaluateAllFuncJac(obj,numVars,decVector,isNewX)
            % Evaluates the user's function and derivatives
            [fun,funJac,numFunEvals] = EvaluateCostAndJac(obj,numVars,decVector,isNewX);
            [conFunc,conJac,numGEvals] = EvaluateConAndJac(obj,numVars,decVector,false);
        end
        
        function [fun,numFunEvals,conFunc,numGEvals] = ...
                EvaluateFuncsOnly(obj,numVars,decVector,isNewX)
            % Evaluates the user's function to evaluate functions only
            fun = obj.userFunction.EvaluateCostFunc(numVars,decVector,isNewX);
            conFunc = obj.userFunction.EvaluateConFunc(numVars,...
                decVector(1:obj.numUserDecisionVars),false);
            numFunEvals = 1;
            numGEvals = 1;
        end
        
        function [costJac,conJac,numGEvals] = EvaluateDerivsOnly(obj,numVars,decVector,isNewX)
            % Evaluates the user's function to evaluate derivatives only
            costJac = obj.userFunction.EvaluateCostJac(numVars,decVector,isNewX);
            conJac = obj.userFunction.EvaluateConJac(numVars,decVector,false);
            numGEvals = 1;
        end
        
        
        %% NLP Function Interfaces
        
        function [numVars,numCons]= GetNLPInfo(obj)
            % Returns number of variables and constraints in the full NLP
            numVars = obj.numUserDecisionVars + obj.numElasticVars;
            numCons = obj.numUserConstraints + obj.numBoundCons + obj.numElasticConstraints;
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = GetNLPBoundsInfo(obj)
            % Returns the constraint bounds for the full NLP
            varLowerBounds = obj.nlpVarLowerBounds;
            varUpperBounds = obj.nlpVarUpperBounds;
            conLowerBounds = obj.nlpConLowerBounds;
            conUpperBounds = obj.nlpConUpperBounds;
        end
        
        function [initGuess] = GetNLPStartingPoint(obj)
            % Returns the constraint bounds for the full NLP
            if ~obj.isModeElastic
                initGuess = obj.userFunction.GetStartingPoint();
            else
                initGuess = [obj.userFunction.GetStartingPoint(); ones(obj.numElasticVars,1)];
            end
        end
        
        function [fun,numFunEvals,conFunc,numGEvals] = ...
                EvaluteNLPFunctionsOnly(obj,numVars,decVector,isNewX)
            % Evaluates only the NLP Functions
            
            [fun,numFunEvals,userConFunc,numGEvals] = ...
                EvaluateFuncsOnly(obj,numVars,decVector(1:obj.numUserDecisionVars),isNewX);
            if obj.isModeElastic
                userConFunc =  userConFunc - obj.GetElasticV(decVector) + obj.GetElasticW(decVector);
                fun = fun + obj.elasticWeight*(sum(obj.GetElasticV(decVector) + obj.GetElasticW(decVector)));
            end
            conFunc = [userConFunc;decVector(obj.userVarBoundConIdxs)];
            if obj.isModeElastic
                conFunc = [conFunc;decVector(obj.elasticVarVStartIdx:obj.elasticVarWStopIdx)];
            end
        end
        
        function [nlpCostJac,nlpConJac,numGEvals] = EvaluateNLPDerivsOnly(obj,numVars,decVector,isNewX)
            % Evaluates only the NLP Derivatives
            userCostJac = obj.userFunction.EvaluateCostJac(numVars,...
                decVector(1:obj.numUserDecisionVars),isNewX);
            userConJac = obj.userFunction.EvaluateConJac(numVars,...
                decVector(1:obj.numUserDecisionVars),false);
            obj.InsertUserConJac(userConJac);
            if obj.isModeElastic
                obj.InsertUserCostJac(userCostJac)
                obj.UpdateElasticCostJacobian()
                nlpCostJac = obj.costJac;
            else
                nlpCostJac = userCostJac;
            end
            nlpConJac = obj.conJac;
            numGEvals = 1;
        end
        
        function [nlpCost,nlpCostJac,numCostEvals,nlpConFunc,nlpConJac,numGEvals] = ...
                EvaluateAllNLPFuncJac(obj,numVars,decVector,isNewX)
            % Evaluates all NLP functions and Jacobians
            [nlpCost,numCostEvals,nlpConFunc] = ...
                EvaluteNLPFunctionsOnly(obj,numVars,decVector,isNewX);
            [nlpCostJac,nlpConJac,numGEvals] = EvaluateNLPDerivsOnly(obj,numVars,decVector,isNewX);
        end
        
        function InsertUserConJac(obj,userCostJac)
            % Inserts the users constraint Jacobian into the total NLP con
            % Jacobian
            obj.conJac(obj.userConJacRowStartIdx:obj.userConJacRowStopIdx,...
                obj.userConJacColStartIdx:obj.userConJacColStopIdx) = userCostJac;
        end
        
        function InsertUserCostJac(obj,userCostJac)
            % Inserts the user Jacobian into the total NLP cost Jacobian
            obj.costJac(1:obj.numUserDecisionVars,1) = userCostJac;
        end
        
        function UpdateElasticCostJacobian(obj)
            % Adds in the portion of the cost Jacobian due to elastic vars and weight
            obj.costJac(obj.elasticVarVStartIdx:obj.elasticVarWStopIdx) = ...
                obj.elasticWeight*ones(obj.numElasticVars,1);
        end
        
        function numUserDecisionVars = GetNumUserDecisionVars(obj)
            % Returns the number of decision variables in the user's problem
            numUserDecisionVars = obj.numUserDecisionVars;
        end
        
        function numCons = GetNumUserConstraints(obj)
            % Returns the number of decision variables in the user's problem
            numCons = obj.numUserConstraints;
        end
        
        function numElasticVars = GetNumElasticVars(obj)
            % Returns the number of elastic variables in problem
            numElasticVars = obj.numElasticVars;
        end
        
        function elasticV = GetElasticV(obj,decVec)
            % Returns the "V" elastic mode variables
            if obj.isModeElastic
                elasticV = decVec(obj.elasticVarVStartIdx:obj.elasticVarVStopIdx,1);
            else
                error('Cannot compute elasticV because the problem is not in elastic mode')
            end
        end
        
        function elasticW = GetElasticW(obj,decVec)
            % Returns the "W" elastic mode variables
            if obj.isModeElastic
                elasticW = decVec(obj.elasticVarWStartIdx:obj.elasticVarWStopIdx,1);
            else
                error('Cannot compute elasticV because the problem is not in elastic mode')
            end
        end
        
        function maxElasticVar = GetMaxElasticVar(obj,decVec)
            % Returns the maximum elastic variable
            maxElasticVar = max(decVec(obj.elasticVarVStartIdx:obj.elasticVarWStopIdx,1));
        end
        
        function SetElasticWeight(obj,elasticWeight)
            % Sets the elastic weight
           obj.elasticWeight = elasticWeight; 
           obj.UpdateElasticCostJacobian()
        end
        
        function conViolation = MaxUserConViolation(obj,conViolation,decVec)
            % Takes constraints with elastic shift and removes it ( results
            % in user constraint values being returned
            conViolation =  conViolation(1:obj.numUserConstraints) ...
                            + obj.GetElasticV(decVec) - obj.GetElasticW(decVec);
        end
        
        function cost = ShiftCost(obj,cost,decVector)
            % Takes cost with elastic shift and removes it ( results
            % in user cost value being returned
            cost = cost - obj.elasticWeight*(sum(obj.GetElasticV(decVector) + obj.GetElasticW(decVector)));
        end
        
    end
    
end

