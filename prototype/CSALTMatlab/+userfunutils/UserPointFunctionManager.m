classdef UserPointFunctionManager < userfunutils.UserFunctionManager
    
    %  USERPOINTFUNCTION is class to handle I/O for optimal control user
    %  point functions.
    
    properties
        
        %  Integer. The number of event functions
        numBoundaryFunctions = 0;
        %  Bool.  Flag indicating if user point func has cost contribution
        hasCostFunction  = false();
        %  Bool.  Flag indicating if user point func has event constraints
        hasBoundaryFunctions = false();
        %  Real Array.  Lower bound on boundary functions
        conUpperBound
        %  Real Array.  Upper bound on boundary functions
        conLowerBound
        %  number of phases in the problem
        numPhases
        
        %  Bool.  Flag to control some actions during intialization
        isInitializing = true()
        
        % Bool. Indicates if user provided Jac. of Dyn funcs w/r/t time
        %         hasUserFuncInitTimeJac = false();
        %         % Bool. Indicates if user provided Jac. of Dyn funcs w/r/t state
        %         hasUserFuncInitStateJac = false();
        %         % Bool. Indicates if user provided Jac. of Dyn funcs w/r/t time
        %         hasUserFuncFinalTimeJac = false();
        %         % Bool. Indicates if user provided Jac. of Dyn funcs w/r/t state
        %         hasUserFuncFinalStateJac = false();
        
        %% New data added during refactoring
        
        %  vector of pointers to the phases
        phaseList
        %  vector of input PathInputData objects with data for the beginning of each phase
        initialInputData
        %  vector of input PathInputData objects with data for the end of each phase
        finalInputData
        %  Pointer to FunctionContainer object.
        funcData
        %  pointer to UserPointFunction.  This is the user's object.
        userObject
        %  Pointer to JacobianData object. Jacobian data for bound functions
        boundJacData
        %  Pointer to JacobianData object. Jacobian data for bound functions
        costJacData
        %  Pointer to NLPFuncUtil_MultiPoint object.
        boundNLPUtil
        %  Pointer to NLPFuncUtil_MultiPoint object.
        costNLPUtil
        %  Integer.  Total number of NLP variables in the problem
        totalnumDecisionParams
        %  Vector of integers.  Contains the starting index for each phase
        %  into the NLP decision vector
        decVecStartIdxs
        
    end
    
    methods
        
        function Initialize(obj,userObject,phaseList,totalnumDecisionParams,decVecStartIdxs)
            %  Call the user function and determine problem characteristics
            
            %  If there is no function set flag and return
            if isempty(userObject)
                obj.hasFunction = false();
                return
            else
                obj.hasFunction = true();
            end
            
            %  Set pointers to input objects
            obj.decVecStartIdxs = decVecStartIdxs;
            obj.totalnumDecisionParams = totalnumDecisionParams;
            obj.phaseList = phaseList;
            obj.numPhases = length(phaseList);
            obj.userObject = userObject;
            import userfunutils.JacobianData
            obj.boundJacData = userfunutils.JacobianData();
            obj.costJacData = userfunutils.JacobianData();
            
            import collutils.NLPFuncUtil_MultiPoint
            obj.boundNLPUtil = collutils.NLPFuncUtil_MultiPoint();
            obj.costNLPUtil = collutils.NLPFuncUtil_MultiPoint();
            
            %  Call the function, then determine what was populated and set properties accordingly.
            InitializeInputData(obj)
            
            PrepareInputData(obj)
            InitializeUserObject(obj)
            obj.EvaluateUserFunction();
            InitializeUserFuncProperties(obj)
            obj.boundJacData.Initialize(obj.numBoundaryFunctions,obj.hasBoundaryFunctions,obj.phaseList,obj.decVecStartIdxs);
            if obj.hasCostFunction
                numFunctions = 1;
            else
                numFunctions = 0;
            end
            obj.costJacData.Initialize(numFunctions,obj.hasCostFunction,obj.phaseList,obj.decVecStartIdxs);
            %obj.EvaluateUserJacobian();
            obj.ComputeSparsityPatterns();
            obj.DetermineFuncDependencies();
            obj.isInitializing = false();
            obj.InitializeBoundNLPUtil();
            obj.InitializeCostNLPUtil();
        end
        
        function conLowerBound = GetConLowerBound(obj)
            %  Returns lower bounds on bound functions
            conLowerBound = obj.funcData.algData.GetLowerBounds();
        end
        
        function conUpperBound = GetConUpperBound(obj)
            % Returns upper bounds on bound functions
            conUpperBound = obj.funcData.algData.GetUpperBounds();
        end
        
        function funcValues = ComputeBoundNLPFunctions(obj)
            %  returns bound function values
            obj.EvaluateUserFunction();
            obj.boundNLPUtil.UpdateUserFunArrays(obj.funcData.algData,obj.boundJacData)
            funcValues = obj.boundNLPUtil.ComputeFunctions();
        end
        
        function jacArray = ComputeBoundNLPJacobian(obj)
            %  returns bound function Jacobian
            %obj.EvaluateUserJacobian();
            obj.boundNLPUtil.UpdateUserFunArrays(obj.funcData.algData,obj.boundJacData)
            jacArray = obj.boundNLPUtil.ComputeJacobian();
        end
        
        function jacArray = ComputeBoundNLPSparsityPattern(obj)
            % Returns bound function sparsity pattern
            obj.boundNLPUtil.UpdateUserFunArrays(obj.funcData.algData,obj.boundJacData)
            jacArray = obj.boundNLPUtil.ComputeSparsity();
        end
        
        function funcValues = ComputeCostNLPFunctions(obj)
            %  Returns cost function value
            obj.EvaluateUserFunction();
            obj.costNLPUtil.UpdateUserFunArrays(obj.funcData.costData,obj.costJacData)
            funcValues = obj.costNLPUtil.ComputeFunctions();
        end
        
        function jacArray = ComputeCostNLPJacobian(obj)
            % Returns cost function Jacobian
            %obj.EvaluateUserJacobian();
            obj.costNLPUtil.UpdateUserFunArrays(obj.funcData.costData,obj.costJacData)
            jacArray = obj.costNLPUtil.ComputeJacobian();
        end
        
        function jacArray = ComputeCostNLPSparsityPattern(obj)
            % Returns cost function sparsity pattern
            obj.costNLPUtil.UpdateUserFunArrays(obj.funcData.costData,obj.costJacData)
            jacArray = obj.costNLPUtil.ComputeSparsity();
        end
        
        function InitializeBoundNLPUtil(obj)
            % Initilizes the bound function NLP utility helper class
            obj.boundNLPUtil.Initialize(obj.phaseList,obj.funcData.algData,...
                obj.boundJacData,obj.totalnumDecisionParams)
        end
        
        function InitializeCostNLPUtil(obj)
            % Initilizes the cost function NLP utility helper class
            obj.costNLPUtil.Initialize(obj.phaseList,obj.funcData.costData,...
                obj.costJacData,obj.totalnumDecisionParams)
        end
        
        function InitializeUserFuncProperties(obj)
            %  Initializes basic properties based on user probelm
            obj.hasBoundaryFunctions = obj.funcData.algData.hasUserFunction;
            obj.hasCostFunction = obj.funcData.costData.hasUserFunction;
            obj.numBoundaryFunctions = obj.funcData.algData.GetNumFunctions();
        end
        
        function EvaluateUserJacobian(obj)
            %  Adds in missing partials
            %  Evaluate at the nominal point before finite differencing
            obj.EvaluateUserFunction();
            %if ~obj.isInitializing
            obj.ComputeStateJacobian();
            obj.ComputeTimeJacobian();
            %            obj.userClass.EvaluateUserJacobian();
            %end
        end
        
        function EvaluateUserFunction(obj)
            %  Evaluates user functions but does not add in mission
            %  partials
            try
                PrepareInputData(obj)
                obj.userObject.EvaluateUserFunction();
                obj.funcData = obj.userObject.GetFuncData();
            catch
                lstErr = lasterr;
                error('User Path Function Object Failed to Evaluate')
            end
        end
        
        function EvaluatePreparedUserFunction(obj)
            %  Evaluates user functions assuming data is preconfigured.
            %  Used for Jacobian evaluation especially, as perts are set
            try
                obj.userObject.EvaluateUserFunction();
                obj.funcData = obj.userObject.GetFuncData();
            catch
                lstErr = lasterr;
                error('User Path Function Object Failed to Evaluate')
            end
        end
        
        function InitializeInputData(obj)
            %  Creates and intializes input data objects
            import userfunutils.FunctionInputData
            for phaseIdx = 1:obj.numPhases
                %TODO:  Add config to class to avoid call to phase
                numStateVars = obj.phaseList{phaseIdx}.Config.GetNumStateVars();
                numControlVars = obj.phaseList{phaseIdx}.GetNumControlVars();
                obj.initialInputData{phaseIdx} = userfunutils.FunctionInputData;
                obj.initialInputData{phaseIdx}.Initialize(numStateVars,numControlVars);
                obj.finalInputData{phaseIdx} = userfunutils.FunctionInputData;
                obj.finalInputData{phaseIdx}.Initialize(numStateVars,numControlVars);
            end
        end
        
        function PrepareInputData(obj)
            %  Updates state, time, etc. of input data objects
            for phaseIdx = 1:obj.numPhases
                %  Data at initial point
                obj.initialInputData{phaseIdx}.SetStateVec( ...
                    obj.phaseList{phaseIdx}.DecVector.GetFirstStateVector());
                obj.initialInputData{phaseIdx}.SetTime( ...
                    obj.phaseList{phaseIdx}.DecVector.GetFirstTime());
                %  Data at final point
                obj.finalInputData{phaseIdx}.SetStateVec( ...
                    obj.phaseList{phaseIdx}.DecVector.GetLastStateVector());
                obj.finalInputData{phaseIdx}.SetTime( ...
                    obj.phaseList{phaseIdx}.DecVector.GetLastTime());
            end
        end
        
        function DetermineFuncDependencies(obj)
            %  Determines dependency of bound and cost functions on state
            %  and time
            for phaseIdx = 1:obj.numPhases
                
                if max(max(obj.boundJacData.initTimeJacPattern{phaseIdx})) ~= 0
                    obj.boundJacData.hasInitTimeDepend{phaseIdx} = true;
                else
                    obj.boundJacData.hasInitTimeDepend{phaseIdx} = false;
                end
                
                if max(max(obj.boundJacData.initStateJacPattern{phaseIdx})) ~= 0
                    obj.boundJacData.hasInitStateDepend{phaseIdx} = true;
                else
                    obj.boundJacData.hasInitStateDepend{phaseIdx} = false;
                end
                
                if max(max(obj.boundJacData.finalTimeJacPattern{phaseIdx})) ~= 0
                    obj.boundJacData.hasFinalTimeDepend{phaseIdx} = true;
                else
                    obj.boundJacData.hasFinalTimeDepend{phaseIdx} = false;
                end
                
                if max(max(obj.boundJacData.finalStateJacPattern{phaseIdx})) ~= 0
                    obj.boundJacData.hasFinalStateDepend{phaseIdx} = true;
                else
                    obj.boundJacData.hasFinalStateDepend{phaseIdx} = false;
                end
                
                if max(max(obj.costJacData.initTimeJacPattern{phaseIdx})) ~= 0
                    obj.costJacData.hasInitTimeDepend{phaseIdx} = true;
                else
                    obj.costJacData.hasInitTimeDepend{phaseIdx} = false;
                end
                
                if max(max(obj.costJacData.initStateJacPattern{phaseIdx})) ~= 0
                    obj.costJacData.hasInitStateDepend{phaseIdx} = true;
                else
                    obj.costJacData.hasInitStateDepend{phaseIdx} = false;
                end
                
                if max(max(obj.costJacData.finalTimeJacPattern{phaseIdx})) ~= 0
                    obj.costJacData.hasFinalTimeDepend{phaseIdx} = true;
                else
                    obj.costJacData.hasFinalTimeDepend{phaseIdx} = false;
                end
                
                if max(max(obj.costJacData.finalStateJacPattern{phaseIdx})) ~= 0
                    obj.costJacData.hasFinalStateDepend{phaseIdx} = true;
                else
                    obj.costJacData.hasFinalStateDepend{phaseIdx} = false;
                end
                
            end
        end
        
        function InitializeUserObject(obj)
            %  Initializes the users point funciton object
            obj.userObject.Initialize(obj.initialInputData,obj.finalInputData)
        end
        
        function ComputeSparsityPatterns(obj)
            %  Compute the function's pattern w/r/t problem params
            ComputeStateSparsity(obj)
            ComputeTimeSparsity(obj)
        end
        
        %% This block of methods computes Jacobians
        
        function ComputeTimeJacobian(obj)
            
            %  Save nominal values for later use
            obj.PrepareInputData();
            obj.EvaluatePreparedUserFunction()
            if obj.hasCostFunction
                nomCostValue = obj.funcData.costData.GetFunctionValues();
            end
            if obj.hasBoundaryFunctions
                nomBoundFuncValues = obj.funcData.algData.GetFunctionValues();
            end
            
            %  Loop over the phases to compute Jacobians w/r/t initial time
            for phaseIdx = 1:obj.numPhases
                
                %  Perturb the Time and recompute user functions
                nomTime = obj.initialInputData{phaseIdx}.GetTime();
                deltaTime = 1e-7;
                obj.initialInputData{phaseIdx}.SetTime(nomTime + deltaTime);
                obj.EvaluatePreparedUserFunction()
                
                %  Compute the Jacobian for initial time
                timeIdx = 1;
                if obj.hasBoundaryFunctions
                    jacValue = (obj.funcData.algData.GetFunctionValues() - nomBoundFuncValues)/deltaTime;
                    jacValue = obj.HandleGradient(jacValue);
                    obj.boundJacData.initTimeJac{phaseIdx}(1:obj.numBoundaryFunctions,timeIdx)=jacValue;
                end
                if obj.hasCostFunction
                    jacValue = (obj.funcData.costData.GetFunctionValues() - nomCostValue)/deltaTime;
                    jacValue = obj.HandleGradient(jacValue);
                    obj.costJacData.initTimeJac{phaseIdx}(1,timeIdx) = jacValue;
                end
                
                %  Set state back to nominal value and evaluate.
                obj.initialInputData{phaseIdx}.SetTime(nomTime);
                
            end
            
            
            %  Loop over the phases to compute Jacobians w/r/t final time
            for phaseIdx = 1:obj.numPhases
                
                %  Perturb the Time and recompute user functions
                nomTime = obj.finalInputData{phaseIdx}.GetTime();
                deltaTime = 1e-7;
                obj.finalInputData{phaseIdx}.SetTime(nomTime + deltaTime);
                obj.EvaluatePreparedUserFunction()
                
                %  Compute the Jacobian for final time
                timeIdx = 1;
                if obj.hasBoundaryFunctions
                    jacValue = (obj.funcData.algData.GetFunctionValues() - nomBoundFuncValues)/deltaTime;
                    jacValue = obj.HandleGradient(jacValue);
                    obj.boundJacData.finalTimeJac{phaseIdx}(1:obj.numBoundaryFunctions,timeIdx)=jacValue;
                end
                if  obj.hasCostFunction
                    jacValue = (obj.funcData.costData.GetFunctionValues() - nomCostValue)/deltaTime;
                    jacValue = obj.HandleGradient(jacValue);
                    obj.costJacData.finalTimeJac{phaseIdx}(1,timeIdx) = jacValue;
                end
                
                %  Set state back to nominal value and evaluate.
                obj.finalInputData{phaseIdx}.SetTime(nomTime);
                
            end
            % This computes the correct analytic partials (for those provided)
            %  and avoids noise due to finite of constraints taht required it.
            obj.PrepareInputData();
            obj.EvaluatePreparedUserFunction()
            
        end
        
        function ComputeStateJacobian(obj)
            %  Compute the Jacobian w/r/t state
            
            %  Save nominal values for later use
            obj.PrepareInputData();
            obj.EvaluatePreparedUserFunction()
            if obj.hasCostFunction
                nomCostValue = obj.funcData.costData.GetFunctionValues();
            end
            if obj.hasBoundaryFunctions
                nomBoundFuncValues = obj.funcData.algData.GetFunctionValues();
            end
            
            %  Loop over the phases to compute Jacobians w/r/t initial state
            for phaseIdx = 1:obj.numPhases
                %  Loop initializations for phase dependent data
                numStateVars = obj.initialInputData{phaseIdx}.GetNumStateVars();
                pertVec = ones(numStateVars,1)*1e-7;
                nomStateVec = obj.initialInputData{phaseIdx}.GetStateVec();
                
                
                %  Loop over state values and perturb them
                for stateIdx = 1:numStateVars
                    
                    %  Perturb the state and recompute user functions
                    deltaStateVec = zeros(numStateVars,1);
                    deltaStateVec(stateIdx) = pertVec(stateIdx);
                    obj.initialInputData{phaseIdx}.SetStateVec(nomStateVec + deltaStateVec);
                    obj.EvaluatePreparedUserFunction()
                    
                    %  Compute and save current row of Jacobian
                    % TODO: Fix to test if user has provided Jacobain on a
                    % phase level.
                    if obj.hasBoundaryFunctions
                        jacValue = (obj.funcData.algData.GetFunctionValues() - ...
                            nomBoundFuncValues)/pertVec(stateIdx);
                        jacValue = obj.HandleGradient(jacValue);
                        obj.boundJacData.initStateJac{phaseIdx}(...
                            1:obj.numBoundaryFunctions,stateIdx)= jacValue;
                    end
                    if  obj.hasCostFunction
                        jacValue = (obj.funcData.costData.GetFunctionValues() ...
                            - nomCostValue)/pertVec(stateIdx);
                        jacValue = obj.HandleGradient(jacValue);
                        obj.costJacData.initStateJac{phaseIdx}(1,stateIdx) = jacValue;
                    end
                    
                end
                
                %  Set state back to nominal value and evaluate.  This computes
                %  the correct analytic partials (for those provided)
                %  and avoids noise due to finite of constraints taht required it.
                obj.initialInputData{phaseIdx}.SetStateVec(nomStateVec);
                
            end
            
            %  Loop over the phases to compute Jacobians w/r/t final state
            for phaseIdx = 1:obj.numPhases
                %  Loop initializations for phase dependent data
                numStateVars = obj.finalInputData{phaseIdx}.GetNumStateVars();
                pertVec = ones(numStateVars,1)*1e-7;
                nomStateVec = obj.finalInputData{phaseIdx}.GetStateVec();
                
                %  Loop over state values and perturb them
                for stateIdx = 1:numStateVars
                    
                    %  Perturb the state and recompute user functions
                    deltaStateVec = zeros(numStateVars,1);
                    deltaStateVec(stateIdx) = pertVec(stateIdx);
                    obj.finalInputData{phaseIdx}.SetStateVec(nomStateVec + deltaStateVec);
                    obj.EvaluatePreparedUserFunction()
                    
                    if obj.hasBoundaryFunctions
                        jacValue = (obj.funcData.algData.GetFunctionValues() - ...
                            nomBoundFuncValues)/pertVec(stateIdx);
                        jacValue = obj.HandleGradient(jacValue);
                        obj.boundJacData.finalStateJac{phaseIdx}(...
                            1:obj.numBoundaryFunctions,stateIdx)= jacValue;
                    end
                    if obj.hasCostFunction
                        jacValue = (obj.funcData.costData.GetFunctionValues() ...
                            - nomCostValue)/pertVec(stateIdx);
                        jacValue = obj.HandleGradient(jacValue);
                        obj.costJacData.finalStateJac{phaseIdx}(1,stateIdx) = jacValue;
                    end
                    
                end
                %  Set state back to nominal value and evaluate.
                obj.finalInputData{phaseIdx}.SetStateVec(nomStateVec);
                
            end
            
            %  This computes the correct analytic partials (for those provided)
            %  and avoids noise due to finite of constraints taht required it.
            obj.PrepareInputData();
            obj.EvaluatePreparedUserFunction()
        end
        
        %% This block of functions computes sparsity patterns
        function ComputeStateSparsity(obj)
            %  Computes sparsity pattern of user functions w/r/t to state
            
            
            for phaseIdx = 1:obj.numPhases
                nominalState = obj.initialInputData{phaseIdx}.GetStateVec();
                stateLowerBound = obj.GetStateLowerBound(phaseIdx);
                stateUpperBound = obj.GetStateUpperBound(phaseIdx);
                
                %  Evaluate at lower bound
                obj.initialInputData{phaseIdx}.SetStateVec(stateLowerBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeStateJacobian();
                obj.UpdateStateSparsityPattern(phaseIdx);
                
                %  Evaluate at upper bound
                obj.initialInputData{phaseIdx}.SetStateVec(stateUpperBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeStateJacobian();
                obj.UpdateStateSparsityPattern(phaseIdx);
                
                %  Evalaute at random points
                numRandEvals = 3;
                for randIdx = 1:numRandEvals
                    stateVec = obj.GetRandomVector(stateUpperBound,stateLowerBound);
                    obj.initialInputData{phaseIdx}.SetStateVec(stateVec);
                    obj.EvaluatePreparedUserFunction();
                    obj.ComputeStateJacobian();
                    obj.UpdateStateSparsityPattern(phaseIdx);
                end
                %  Set back to nominal values
               obj.initialInputData{phaseIdx}.SetStateVec(nominalState);
            end
            
            for phaseIdx = 1:obj.numPhases
                
                nominalState = obj.finalInputData{phaseIdx}.GetStateVec();
                %  Evaluate at lower bound
                obj.finalInputData{phaseIdx}.SetStateVec(stateLowerBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeStateJacobian();
                obj.UpdateStateSparsityPattern(phaseIdx);
                
                %  Evaluate at upper bound
                obj.finalInputData{phaseIdx}.SetStateVec(stateUpperBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeStateJacobian();
                obj.UpdateStateSparsityPattern(phaseIdx);
                
                %  Evalaute at random points
                numRandEvals = 3;
                for randIdx = 1:numRandEvals
                    stateVec = obj.GetRandomVector(stateUpperBound,stateLowerBound);
                    obj.finalInputData{phaseIdx}.SetStateVec(stateVec);
                    obj.EvaluatePreparedUserFunction();
                    obj.ComputeStateJacobian();
                    obj.UpdateStateSparsityPattern(phaseIdx);
                end
                
                %  Set back to nominal values
                obj.finalInputData{phaseIdx}.SetStateVec(nominalState);
                
            end
            
        end
        
        function ComputeTimeSparsity(obj,timeUpperBound,timeLowerBound)
            %  Computes sparsity pattern of user functions w/r/t to time
            
            for phaseIdx = 1:obj.numPhases
                nominalTime = obj.initialInputData{phaseIdx}.GetTime();
                timeLowerBound = obj.GetTimeLowerBound(phaseIdx);
                timeUpperBound = obj.GetTimeUpperBound(phaseIdx);
                
                %  Evaluate at lower bound
                obj.initialInputData{phaseIdx}.SetTime(timeLowerBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeTimeJacobian();
                obj.UpdateTimeSparsityPattern(phaseIdx);
                
                %  Evaluate at upper bound
                obj.initialInputData{phaseIdx}.SetTime(timeUpperBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeTimeJacobian();
                obj.UpdateTimeSparsityPattern(phaseIdx);
                
                %  Evalaute at random points
                numRandEvals = 3;
                for randIdx = 1:numRandEvals
                    time = obj.GetRandomVector(timeUpperBound,timeLowerBound);
                    obj.initialInputData{phaseIdx}.SetTime(time);
                    obj.EvaluatePreparedUserFunction();
                    obj.ComputeTimeJacobian();
                    obj.UpdateTimeSparsityPattern(phaseIdx);
                end
                %  Set back to nominal values
               obj.initialInputData{phaseIdx}.SetTime(nominalTime);
            end
            
            for phaseIdx = 1:obj.numPhases
                
                nominalTime = obj.finalInputData{phaseIdx}.GetTime();
                %  Evaluate at lower bound
                obj.finalInputData{phaseIdx}.SetTime(timeLowerBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeTimeJacobian();
                obj.UpdateTimeSparsityPattern(phaseIdx);
                
                %  Evaluate at upper bound
                obj.finalInputData{phaseIdx}.SetTime(timeUpperBound);
                obj.EvaluatePreparedUserFunction();
                obj.ComputeTimeJacobian();
                obj.UpdateTimeSparsityPattern(phaseIdx);
                
                %  Evalaute at random points
                numRandEvals = 3;
                for randIdx = 1:numRandEvals
                    time = obj.GetRandomVector(timeUpperBound,timeLowerBound);
                    obj.finalInputData{phaseIdx}.SetTime(time);
                    obj.EvaluatePreparedUserFunction();
                    obj.ComputeTimeJacobian();
                    obj.UpdateTimeSparsityPattern(phaseIdx);
                end
                
                %  Set back to nominal values
                obj.finalInputData{phaseIdx}.SetTime(nominalTime);
                
            end
            
        end
        
        function UpdateStateSparsityPattern(obj,phaseIdx)
            %  Add elements to sparsity pattern for state Jacobians.
            
            %  Dynamics Function
            if obj.hasBoundaryFunctions
                for funcIdx = 1:obj.numBoundaryFunctions
                    for stateIdx = 1:obj.initialInputData{phaseIdx}.GetNumStateVars()
                        if obj.boundJacData.initStateJac{phaseIdx}(funcIdx,stateIdx) ~= 0
                            obj.boundJacData.initStateJacPattern{phaseIdx}(funcIdx,stateIdx) = 1;
                        end
                        if obj.boundJacData.finalStateJac{phaseIdx}(funcIdx,stateIdx) ~= 0
                            obj.boundJacData.finalStateJacPattern{phaseIdx}(funcIdx,stateIdx) = 1;
                        end
                    end
                end
            end
            %  Cost Function
            if obj.hasCostFunction
                for stateIdx = 1:obj.initialInputData{phaseIdx}.GetNumStateVars()
                    if obj.costJacData.initStateJac{phaseIdx}(1,stateIdx) ~= 0
                        obj.costJacData.initStateJacPattern{phaseIdx}(1,stateIdx) = 1;
                    end
                    if obj.costJacData.finalStateJac{phaseIdx}(1,stateIdx) ~= 0
                        obj.costJacData.finalStateJacPattern{phaseIdx}(1,stateIdx) = 1;
                    end
                end
            end
        end
        
        function UpdateTimeSparsityPattern(obj,phaseIdx)
            %  Add elements to sparsity pattern for time Jacobians.
            
            %  Dynamics Function
            if obj.hasBoundaryFunctions
                for funIdx = 1:obj.numBoundaryFunctions
                    if obj.boundJacData.initTimeJac{phaseIdx}(funIdx,1) ~= 0
                        obj.boundJacData.initTimeJacPattern{phaseIdx}(funIdx, 1) = 1;
                    end
                    if obj.boundJacData.finalTimeJac{phaseIdx}(funIdx,1) ~= 0
                        obj.boundJacData.finalTimeJacPattern{phaseIdx}(funIdx, 1) = 1;
                    end
                end
            end
            %  Cost Function
            if obj.hasCostFunction
                if obj.costJacData.initTimeJac{phaseIdx}(1,1) ~= 0
                    obj.costJacData.initTimeJacPattern{phaseIdx}(1, 1) = 1;
                end
                if obj.costJacData.finalTimeJac{phaseIdx}(1,1) ~= 0
                    obj.costJacData.finalTimeJacPattern{phaseIdx}(1, 1) = 1;
                end
            end
        end
        
        function upperBound = GetStateUpperBound(obj,phaseIdx)
            % Returns upper bound on the state vector
            upperBound = obj.phaseList{phaseIdx}.GetStateUpperBound();
        end
        
        function lowerBound = GetStateLowerBound(obj,phaseIdx)
            % Returns lower bound on the state vector
            lowerBound = obj.phaseList{phaseIdx}.GetStateLowerBound();
        end
        
        function upperBound = GetTimeUpperBound(obj,phaseIdx)
            % Returns upper bound on time
            upperBound = obj.phaseList{phaseIdx}.GetTimeUpperBound();
        end
        
        function lowerBound = GetTimeLowerBound(obj,phaseIdx)
            % Returns lower bound on time
            lowerBound = obj.phaseList{phaseIdx}.GetTimeLowerBound();
        end
        
    end
    
end

