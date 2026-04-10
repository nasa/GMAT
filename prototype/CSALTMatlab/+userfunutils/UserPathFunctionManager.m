classdef UserPathFunctionManager < userfunutils.UserFunctionManager
    
    %  USERPATHFUNCTION handle I/O for optimal control user path functions
    %
    %  This class manages computation of user functions and
    %  partial derivatives.  If the user does not provide analytic partials
    %  finite differencing is employed.
    
    
    properties (Access = 'private')
        
        % Integer.  The number of algebraic functions
        numAlgFunctions = 0;
        % Integer. The number of dynamics functions
        numDynFunctions = 0;
        % Integer. The number of integral functions
        numIntFunctions = 0;
        %  Bool.  Indicates if user function has cost component
        hasCostFunction = false();
        %  Bool.  Indicates if user function has algebraic functions
        hasAlgFunctions = false();
        %  Bool.  Indicates if user function has dynamics functions
        hasDynFunctions = false();
        %  Bool.  Indicates if user function has integral functions
        hasIntFunctions = false();
        
        %  Note:  it may be possible to make these *FuncTimeJac data
        %  members local data in functions.  I did it this way mainly for
        %  performance because they would get dimensioned 100k times which
        %  would be a performance problem.  This way it is only done once.
        % Real Array. The Jacobian of dynamics w/r/t time
        dynFuncTimeJac
        % Real Array. The Jacobian of dynamics w/r/t state
        dynFuncStateJac
        % Real Array. The Jacobian of dynamics w/r/t control
        dynFuncControlJac
        % Real Array. The Jacobian of algebraic functions w/r/t time
        algFuncTimeJac
        % Real Array. The Jacobian of algebraic functions w/r/t static
        algFuncStateJac
        % Real Array. The Jacobian of algebraic functions w/r/t control
        algFuncControlJac
        % Real Array. The Jacobian of integral cost function w/r/t time
        costFuncTimeJac
        % Real Array. The Jacobian of integral cost function w/r/t state
        costFuncStateJac
        % Real Array. The Jacobian of integral cost function w/r/t control
        costFuncControlJac
        
        % Real Array. Sparsity of Jac. of dynamics w/r/t time
        dynFuncTimeJacPattern
        % Real Array. Sparsity of Jac. of dynamics w/r/t state
        dynFuncStateJacPattern
        % Real Array. Sparsity of Jac. of dynamics w/r/t control
        dynFuncControlJacPattern
        % Real Array. Sparsity of Jac. of alg. functions w/r/t time
        algFuncTimeJacPattern
        % Real Array. Sparsity of Jac. of alg. functions w/r/t time
        algFuncStateJacPattern
        % Real Array. Sparsity of Jac. of alg. functions w/r/t state
        algFuncControlJacPattern
        % Real Array. Sparsity of Jac. of cost function w/r/t time
        costFuncTimeJacPattern
        % Real Array. Sparsity of Jac. of cost function w/r/t state
        costFuncStateJacPattern
        % Real Array. Sparsity of Jac. of cost function w/r/t control
        costFuncControlJacPattern
        
        %  Bool.  Flag to control some actions during intialization
        isInitializing = true()
        
        
        % Real array.  Upper bound on alg function values
        algFuncUpperBound
        % Real array.  Lower bound on alg function values
        algFuncLowerBound
        
        %  Pointet to object of type UserFunctionData_Path.
        paramData
        %  Pointer to PathFunctionContainer
        funcData
        
        %  Bool.  Indicates if there are any state Jacobians that must be finite differenced
        needsStateJacFiniteDiff
        %  Bool.  Indicates if there are any control Jacobians that must be finite differenced
        needsControlJacFiniteDiff
        %  Bool.  Indicates if there are any time Jacobians that must be finite differenced
        needsTimeJacFiniteDiff
        
    end
    
    methods
        
        function Initialize(obj,userClass,paramData,funcData,boundData)

			obj.isInitializing = true();
            
            %  Dimension the optimization parameters.
            obj.SetParameterDimensions(paramData.GetNumStateVars(),...
                paramData.GetNumControlVars())
            
            %  Set pointers to the data        
            obj.SetParamData(paramData)
            obj.SetFunctionData(funcData)
     
            %  If there is no function set flag and return
            if isempty(userClass)
                obj.hasFunction = false();
                return
            else
                obj.hasFunction = true();
                obj.userClass = userClass;
            end
			
            obj.SetIsInitializing(true());
            
            %  Intialize the user's path function object
            % try %  Lots can go wrong in user's function so trap errors
                obj.userClass.Initialize(paramData,funcData);
			% catch
	%                 lastError = lasterr; %#ok<LERR>
	%                 error(['User path object failed intialization with Error: ' lastError]);
	%             end
			
            %  Initialize the data members according to the users problem
            obj.InitFunctionData(funcData);
								
            %  Compute the sparsity pattern for the user's problem
            obj.ComputeSparsityPatterns(paramData,funcData,boundData)	

            %  Re-evaluate at the input values... sparsity determination
            %  made random changes and now we should set back to be
            %  consistent with the input
            obj.EvaluateUserFunction(paramData,funcData);
            
            obj.isInitializing = false();
            obj.SetIsInitializing(false());
            
            obj.algFuncUpperBound = funcData.algData.GetUpperBounds();
            obj.algFuncLowerBound = funcData.algData.GetLowerBounds();
            obj.CheckIfNeedsFiniteDiffJacobian();
        		    
            % obj.WriteDebugData();
        end
        
        function WriteDebugData(obj)
            % Real Array. Sparsity of Jac. of dynamics w/r/t time
            disp('Sparsity of Jac. of dynamics w/r/t time')
            disp(obj.dynFuncTimeJacPattern)
            % Real Array. Sparsity of Jac. of dynamics w/r/t state
            disp('Sparsity of Jac. of dynamics w/r/t state')
            disp(obj.dynFuncStateJacPattern)
            % Real Array. Sparsity of Jac. of dynamics w/r/t control
            disp('Sparsity of Jac. of dynamics w/r/t control')
            disp(obj.dynFuncControlJacPattern)
            % Real Array. Sparsity of Jac. of alg. functions w/r/t time
            disp('Sparsity of Jac. of alg. functions w/r/t time')
            disp(obj.algFuncTimeJacPattern)
            % Real Array. Sparsity of Jac. of alg. functions w/r/t time
            disp('Sparsity of Jac. of alg. functions w/r/t state')
            disp(obj.algFuncStateJacPattern)
            % Real Array. Sparsity of Jac. of alg. functions w/r/t state
            disp('Sparsity of Jac. of alg. functions w/r/t control')
            disp(obj.algFuncControlJacPattern)
            % Real Array. Sparsity of Jac. of cost function w/r/t time
            disp('Sparsity of Jac. of cost function w/r/t time')
            disp(obj.costFuncTimeJacPattern)
            % Real Array. Sparsity of Jac. of cost function w/r/t state
            disp('Sparsity of Jac. of cost function w/r/t state')
            disp(obj.costFuncStateJacPattern)
            % Real Array. Sparsity of Jac. of cost function w/r/t control
            disp('Sparsity of Jac. of cost function w/r/t control')
            disp(obj.costFuncControlJacPattern)
        end
        
        %%  Miscellaneous functions
        function funcData = EvaluateUserFunction(obj,paramData,funcData)
            %  Evaluates user functions but does not add in mission
            %  partials
             % try
                paramData.SetIsPerturbing(false);
                obj.userClass.EvaluateUserFunction(paramData,funcData);
             % catch
             %     lstErr = lasterr; %#ok<LERR,NASGU>
             %     error('User Path Function Object Failed to Evaluate')
             % end
        end
        
        function funcData = EvaluateUserJacobian(obj,paramData,funcData)
            %  Adds in missing partials
            %  Evaluate at the nominal point before finite differencing
            obj.EvaluateUserFunction(paramData,funcData);
            if ~obj.isInitializing
                paramData.SetIsPerturbing(true);
                obj.ComputeControlJacobian(paramData,funcData);
                obj.ComputeStateJacobian(paramData,funcData);
                obj.ComputeTimeJacobian(paramData,funcData);
                obj.userClass.EvaluateUserJacobian(paramData,funcData);
            end
        end
        
        function dynFuncProps = GetDynFuncProperties(obj)
            %  Returns global function properties for dyn functions
            import userfunutils.UserFunProperties;
            dynFuncProps = UserFunProperties();
            dynFuncProps.SetStateJacPattern(obj.dynFuncStateJacPattern);
            dynFuncProps.SetTimeJacPattern(obj.dynFuncTimeJacPattern);
            dynFuncProps.SetControlJacPattern(obj.dynFuncControlJacPattern);
            dynFuncProps.SetNumFunctions(obj.numDynFunctions);
        end
        
        function algFuncProps = GetAlgFuncProperties(obj)
            %  Returns global function properties for algebraic functions
            import userfunutils.UserFunProperties;
            algFuncProps = UserFunProperties();
            algFuncProps.SetStateJacPattern(obj.algFuncStateJacPattern);
            algFuncProps.SetTimeJacPattern(obj.algFuncTimeJacPattern);
            algFuncProps.SetControlJacPattern(obj.algFuncControlJacPattern);
            algFuncProps.SetNumFunctions(obj.numAlgFunctions);
        end
        
        function costFuncProps = GetCostFuncProperties(obj)
            %  Returns global function properties for cost functions
            import userfunutils.UserFunProperties;
            costFuncProps = UserFunProperties();
            costFuncProps.SetStateJacPattern(obj.costFuncStateJacPattern);
            costFuncProps.SetTimeJacPattern(obj.costFuncTimeJacPattern);
            costFuncProps.SetControlJacPattern(obj.costFuncControlJacPattern);
            % If calling there isa  cost function, and there can only be one.
            costFuncProps.SetNumFunctions(1);
        end
        
        function hasAlgFunctions = GetHasAlgFunctions(obj)
            %  Returns hasAlgFunctions flag value
            hasAlgFunctions = obj.hasAlgFunctions;
        end
        
        function hasDynFunctions = GetHasDynFunctions(obj)
            %  Returns hasDynFunctions flag value
            hasDynFunctions = obj.hasDynFunctions;
        end
        
        function hasCostFunctions = GetHasCostFunction(obj)
            %  Returns hasDynFunctions flag value
            hasCostFunctions = obj.hasCostFunction;
        end
        
        function numAlgFunctions = GetNumAlgFunctions(obj)
            % Returns number of algebraic functions
            numAlgFunctions = obj.numAlgFunctions;
        end
        
        function algFuncUpperBound = GetAlgFunctionsUpperBounds(obj)
            % Returns uppper bound array for algebraic functions
            algFuncUpperBound =  obj.algFuncUpperBound;
        end
        
        function algFuncLowerBound = GetAlgFunctionsLowerBounds(obj)
            % Returns lower bound array for algebraic functions
            algFuncLowerBound = obj.algFuncLowerBound;
        end
        
        function SetParamData(obj,paramData)
            % Sets pointer to parameter input data
            obj.paramData = paramData;
        end
        
        function SetFunctionData(obj,funcData)
            % Sets pointer to the function data container
            obj.funcData = funcData;
        end
        
        function SetIsInitializing(obj,isInitializing)
            obj.userClass.SetIsInitializing(isInitializing);
        end
%         function SetParamInterpData(obj,time,stateVec,controlVec)
%             % Sets state data to values needed to interpolate between
%             % mesh/state points
%             obj.paramData.SetStateVec(stateVec)
%             obj.paramData.SetControlVec(controlVec)
%             obj.paramData.SetTime(time);
%         end
        
    end
    
    methods (Access = 'private')
        
        %% This block of functions initializes member data
        
        function SetParameterDimensions(obj,numStateVars,numControlVars)
            %  Sets the dimensions for optimization variables
            obj.numStateVars = numStateVars;
            obj.numControlVars = numControlVars;
            obj.numTimeVars = 2;
        end
        
        function CheckIfNeedsFiniteDiffJacobian(obj)
            %  Sets flags for which Jacobians need finite differencing
            
            %  Check if there aren any state Jacobians that require finite differencing
            obj.needsStateJacFiniteDiff = false;
            if obj.hasDynFunctions && ~obj.funcData.dynData.GetHasUserStateJac()
                obj.needsStateJacFiniteDiff = true;
            end
            if obj.hasAlgFunctions && ~obj.funcData.algData.GetHasUserStateJac()
                obj.needsStateJacFiniteDiff = true;
            end
            if obj.hasCostFunction && ~obj.funcData.costData.GetHasUserStateJac()
                obj.needsStateJacFiniteDiff = true;
            end
            
            %  Check if there aren any control Jacobians that require finite differencing
            obj.needsControlJacFiniteDiff = false;
            if obj.hasDynFunctions && ~obj.funcData.dynData.GetHasUserControlJac()
                obj.needsControlJacFiniteDiff = true;
            end
            if obj.hasAlgFunctions && ~obj.funcData.algData.GetHasUserControlJac()
                obj.needsControlJacFiniteDiff = true;
            end
            if obj.hasCostFunction && ~obj.funcData.costData.GetHasUserControlJac()
                obj.needsControlJacFiniteDiff = true;
            end
            
            %  Check if there aren any control Jacobians that require finite differencing
            obj.needsTimeJacFiniteDiff = false;
            if obj.hasDynFunctions && ~obj.funcData.dynData.GetHasUserTimeJac()
                obj.needsTimeJacFiniteDiff = true;
            end
            if obj.hasAlgFunctions && ~obj.funcData.algData.GetHasUserTimeJac()
                obj.needsTimeJacFiniteDiff = true;
            end
            if obj.hasCostFunction && ~obj.funcData.costData.GetHasUserTimeJac()
                obj.needsTimeJacFiniteDiff = true;
            end
            
        end
        
        function InitFunctionData(obj,funcData)
            %  Initializes data for all function types
            obj.InitCostFunctionData(funcData);
            obj.InitAlgFunctionData(funcData);
            obj.InitDynFunctionData(funcData);
        end
        
        function InitDynFunctionData(obj,funcData)
            obj.hasDynFunctions = funcData.dynData.hasUserFunction;
            
            if obj.hasDynFunctions
                obj.numDynFunctions = length(funcData.dynData.GetFunctionValues());
                obj.dynFuncTimeJacPattern = zeros(obj.numDynFunctions,1);
                obj.dynFuncStateJacPattern  = zeros(obj.numDynFunctions,obj.numStateVars);
                obj.dynFuncControlJacPattern = zeros(obj.numDynFunctions,obj.numControlVars);
                obj.dynFuncTimeJac = zeros(obj.numDynFunctions,1);
                obj.dynFuncStateJac  = zeros(obj.numDynFunctions,obj.numStateVars);
                obj.dynFuncControlJac = zeros(obj.numDynFunctions,obj.numControlVars);
            end
        end
        
        function InitAlgFunctionData(obj,funcData)
            %  If has algebraic functions, set flags and dimension arrays
            obj.hasAlgFunctions = funcData.algData.hasUserFunction;
            if obj.hasAlgFunctions
                obj.numAlgFunctions = length(funcData.algData.GetFunctionValues());
                obj.algFuncTimeJacPattern = zeros(obj.numAlgFunctions,1);
                obj.algFuncStateJacPattern = zeros(obj.numAlgFunctions,obj.numStateVars);
                obj.algFuncControlJacPattern = zeros(obj.numAlgFunctions,obj.numControlVars);
                obj.algFuncTimeJac = zeros(obj.numAlgFunctions,1);
                obj.algFuncStateJac = zeros(obj.numAlgFunctions,obj.numStateVars);
                obj.algFuncControlJac = zeros(obj.numAlgFunctions,obj.numControlVars);
            end
        end
        
        function InitCostFunctionData(obj,funcData)
            obj.hasCostFunction = funcData.costData.hasUserFunction;
            if obj.hasCostFunction
                obj.costFuncTimeJacPattern = zeros(1,1);
                obj.costFuncStateJacPattern = zeros(1,obj.numStateVars);
                obj.costFuncControlJacPattern = zeros(1,obj.numControlVars);
                obj.costFuncTimeJac = zeros(1,1);
                obj.costFuncStateJac = zeros(1,obj.numStateVars);
                obj.costFuncControlJac = zeros(1,obj.numControlVars);
            end
        end
        
        %% This block of methods computes function Jacobians
        function funcData = ComputeStateJacobian(obj,paramData,funcData)
            %  Compute the Jacobian w/r/t state
            
            %  If not initializing and all Jacobians are provided, nothing to do
            if ~obj.isInitializing && ~obj.needsStateJacFiniteDiff
                return
            end
            
            %  Save nominal values for later use
            if obj.hasCostFunction
                nomCostValue = funcData.costData.GetFunctionValues();
            end
            if obj.hasAlgFunctions
                nomAlgFuncValues = funcData.algData.GetFunctionValues();
            end
            if obj.hasDynFunctions
                nomDynFuncValues = funcData.dynData.GetFunctionValues();
            end
            nomStateVec = paramData.GetStateVec();
            
            %  Loop over state values and perturb them
            pertVec = ones(obj.numStateVars,1)*1e-7;
            for stateIdx = 1:obj.numStateVars
                
                %  Perturb the state and recompute user functions
                deltaStateVec = zeros(obj.numStateVars,1);
                deltaStateVec(stateIdx) = pertVec(stateIdx);
                paramData.SetStateVec(nomStateVec + deltaStateVec);
                obj.EvaluateUserFunction(paramData,funcData);
                
                %  Compute and save current row of Jacobian
                if obj.hasDynFunctions && ...
                        (~funcData.dynData.GetHasUserStateJac() || obj.isInitializing)
                    jacValue = (funcData.dynData.GetFunctionValues() - ...
                        nomDynFuncValues)/pertVec(stateIdx);
                    jacValue = obj.HandleGradient(jacValue);
                    obj.dynFuncStateJac(1:obj.numDynFunctions,stateIdx)= jacValue;
                end
                if obj.hasAlgFunctions && ...
                        (~funcData.algData.GetHasUserStateJac() || obj.isInitializing)
                    jacValue = (funcData.algData.GetFunctionValues() - ...
                        nomAlgFuncValues)/pertVec(stateIdx);
                    jacValue = obj.HandleGradient(jacValue);
                    obj.algFuncStateJac(1:obj.numAlgFunctions,stateIdx) = jacValue;
                end
                if obj.hasCostFunction && ...
                        (~funcData.costData.GetHasUserStateJac() || obj.isInitializing)
                    jacValue = (funcData.costData.GetFunctionValues() - ...
                        nomCostValue)/pertVec(stateIdx);
                    jacValue = obj.HandleGradient(jacValue);
                    obj.costFuncStateJac(1,stateIdx) = jacValue;
                end
                
            end
            %  Set state back to nominal value and evaluate.  This computes
            %  the correct analytic partials (for those provided)
            %  and avoids noise due to finite of constraints taht required it.
            paramData.SetStateVec(nomStateVec);
            obj.userClass.EvaluateUserJacobian(paramData,funcData);
            
            %  Now set the computed Jacobian on the funcData object
            if obj.hasCostFunction && ~funcData.costData.GetHasUserStateJac()
                funcData.costData.SetStateJac(obj.costFuncStateJac);
            end
            if obj.hasAlgFunctions && ~funcData.algData.GetHasUserStateJac()
                funcData.algData.SetStateJac(obj.algFuncStateJac);
            end
            if obj.hasDynFunctions && ~funcData.dynData.GetHasUserStateJac()
                funcData.dynData.SetStateJac(obj.dynFuncStateJac);
            end
        end
        
        function funcData = ComputeControlJacobian(obj,paramData,funcData)
            %  Compute the Jacobian w/r/t state
            
            %  If not initializing and all Jacobians are provided, nothing to do
            if ~obj.isInitializing && ~obj.needsControlJacFiniteDiff
                return
            end
            
            %  Save nominal values for later use
            if obj.hasCostFunction
                nomCostValue = funcData.costData.GetFunctionValues();
            end
            if obj.hasAlgFunctions
                nomAlgFuncValues = funcData.algData.GetFunctionValues();
            end
            if obj.hasDynFunctions
                nomDynFuncValues = funcData.dynData.GetFunctionValues();
            end
            nomControlVec = paramData.GetControlVec();
            
            %  Loop over state values and perturb them
            pertVec = ones(obj.numControlVars,1)*1e-7;
            for controlIdx = 1:obj.numControlVars
                
                %  Perturb the Control and recompute user functions
                deltaControlVec = zeros(obj.numControlVars,1);
                deltaControlVec(controlIdx) = pertVec(controlIdx);
                paramData.SetControlVec(nomControlVec + deltaControlVec);
                obj.EvaluateUserFunction(paramData,funcData);
                
                %  Compute and save current row of Jacobian
                if obj.hasDynFunctions && ...
                        (~funcData.dynData.GetHasUserControlJac()  || obj.isInitializing)
                    jacValue = (funcData.dynData.GetFunctionValues() - ...
                        nomDynFuncValues)/pertVec(controlIdx);
                    jacValue = obj.HandleGradient(jacValue);
                    obj.dynFuncControlJac(1:obj.numDynFunctions,controlIdx)=jacValue;
                end
                if obj.hasAlgFunctions && ...
                        (~funcData.algData.GetHasUserControlJac()  || obj.isInitializing)
                    jacValue =(funcData.algData.GetFunctionValues() - ...
                        nomAlgFuncValues)/pertVec(controlIdx);
                    jacValue = obj.HandleGradient(jacValue);
                    obj.algFuncControlJac(1:obj.numAlgFunctions,controlIdx) = jacValue;
                end
                if obj.hasCostFunction && ...
                        (~funcData.costData.GetHasUserControlJac()  || obj.isInitializing)
                    jacValue =(funcData.costData.GetFunctionValues() - ...
                        nomCostValue)/pertVec(controlIdx);
                    jacValue = obj.HandleGradient(jacValue);
                    obj.costFuncControlJac(1,controlIdx) = jacValue;
                end
                
            end
            
            %  Set Control back to nominal value.   This makes sure the
            %  user provided partials are not evaluated at a pertubation
            %  point.
            paramData.SetControlVec(nomControlVec);
            obj.userClass.EvaluateUserJacobian(paramData,funcData);
            
            %  Now set the computed Jacobian on the funcData object
            if obj.hasCostFunction && ~funcData.costData.GetHasUserControlJac()
                funcData.costData.SetControlJac(obj.costFuncControlJac);
            end
            if obj.hasAlgFunctions && ~funcData.algData.GetHasUserControlJac()
                funcData.algData.SetControlJac(obj.algFuncControlJac);
            end
            if obj.hasDynFunctions && ~funcData.dynData.GetHasUserControlJac()
                funcData.dynData.SetControlJac(obj.dynFuncControlJac);
            end
            
        end
        
        function funcData = ComputeTimeJacobian(obj,paramData,funcData)
            %  Compute the Jacobian w/r/t state
            
            %  If not initializing and all Jacobians are provided, nothing to do
            if ~obj.isInitializing && ~obj.needsTimeJacFiniteDiff
                return
            end
            
            %  Save nominal values for later use
            if obj.hasCostFunction
                nomCostValue = funcData.costData.GetFunctionValues();
            end
            if obj.hasAlgFunctions
                nomAlgFuncValues = funcData.algData.GetFunctionValues();
            end
            if obj.hasDynFunctions
                nomDynFuncValues = funcData.dynData.GetFunctionValues();
            end
            nomTime = paramData.GetTime();
            
            %  Perturb the Time and recompute user functions
            deltaTime = 1e-7;
            paramData.SetTime(nomTime + deltaTime);
            obj.EvaluateUserFunction(paramData,funcData);
            
            %  Compute and save current row of Jacobian
            timeIdx = 1;
            if obj.hasDynFunctions && ...
                    ( ~funcData.dynData.GetHasUserTimeJac() || obj.isInitializing)
                jacValue = (funcData.dynData.GetFunctionValues() - ...
                    nomDynFuncValues)/deltaTime;
                jacValue = obj.HandleGradient(jacValue);
                obj.dynFuncTimeJac(1:obj.numDynFunctions,timeIdx)=jacValue;
            end
            if obj.hasAlgFunctions && ...
                    (~funcData.algData.GetHasUserTimeJac() || obj.isInitializing)
                jacValue = (funcData.algData.GetFunctionValues() - ...
                    nomAlgFuncValues)/deltaTime;
                jacValue = obj.HandleGradient(jacValue);
                obj.algFuncTimeJac(1:obj.numAlgFunctions,timeIdx) = jacValue;
            end
            if obj.hasCostFunction && ...
                    (~funcData.costData.GetHasUserTimeJac() || obj.isInitializing)
                jacValue = (funcData.costData.GetFunctionValues() - ...
                    nomCostValue)/deltaTime;
                jacValue = obj.HandleGradient(jacValue);
                obj.costFuncTimeJac(1,timeIdx) = jacValue;
            end
            
            %  Set Time back to nominal value
            paramData.SetTime(nomTime);
            obj.userClass.EvaluateUserJacobian(paramData,funcData);
            
            %  Now set the computed Jacobian on the funcData object
            if obj.hasCostFunction && ~funcData.costData.GetHasUserTimeJac()
                funcData.costData.SetTimeJac(obj.costFuncTimeJac);
            end
            if obj.hasAlgFunctions && ~funcData.algData.GetHasUserTimeJac()
                funcData.algData.SetTimeJac(obj.algFuncTimeJac);
            end
            if obj.hasDynFunctions && ~funcData.dynData.GetHasUserTimeJac()
                funcData.dynData.SetTimeJac(obj.dynFuncTimeJac);
            end
            
        end
        
        %% This block of functions computes sparsity patterns
        
        function ComputeSparsityPatterns(obj,paramData,funcData,boundData)
            paramData.SetIsSparsity(true());
            %  Compute the function's pattern w/r/t problem params
            ComputeStateSparsity(obj,paramData,funcData,...
                boundData.stateUpperBound,boundData.stateLowerBound)
            ComputeControlSparsity(obj,paramData,funcData,...
                boundData.controlUpperBound,boundData.controlLowerBound)
            ComputeTimeSparsity(obj,paramData,funcData,...
                boundData.timeUpperBound,boundData.timeLowerBound)
            paramData.SetIsSparsity(false());
        end
        
        function ComputeStateSparsity(obj,paramData,funcData,...
                stateUpperBound,stateLowerBound)
            %  Computes sparsity pattern of user functions w/r/t to state
            
            %nominalState = paramData.GetStateVec();
            
            %  Evaluate at lower bound
            paramData.SetStateVec(stateLowerBound);
            obj.EvaluateUserFunction(paramData,funcData);
            obj.ComputeStateJacobian(paramData,funcData);
            obj.UpdateStateSparsityPattern();
            
            %  Evaluate at upper bound
            paramData.SetStateVec(stateUpperBound);
            obj.EvaluateUserFunction(paramData,funcData);
            obj.ComputeStateJacobian(paramData,funcData);
            obj.UpdateStateSparsityPattern();
            
            %  Evalaute at random points
            numRandEvals = 3;
            for randIdx = 1:numRandEvals
                paramData.SetStateVec(obj.GetRandomVector(stateUpperBound,stateLowerBound));
                obj.EvaluateUserFunction(paramData,funcData);
                obj.ComputeStateJacobian(paramData,funcData);
                obj.UpdateStateSparsityPattern();
            end
            
            %paramData.SetStateVec(nominalState);
        end
        
        function ComputeControlSparsity(obj,paramData,funcData,controlUpperBound,controlLowerBound)
            %  Computes sparsity pattern of user functions w/r/t to control
            
            %nominalControl = paramData.GetControlVec();
            
            %  Evaluate at lower bound
            paramData.SetControlVec(controlLowerBound);
            obj.EvaluateUserFunction(paramData,funcData);
            obj.ComputeControlJacobian(paramData,funcData);
            obj.UpdateControlSparsityPattern();
            
            %  Evaluate at upper bound
            paramData.SetControlVec(controlUpperBound);
            obj.EvaluateUserFunction(paramData,funcData);
            obj.ComputeControlJacobian(paramData,funcData);
            obj.UpdateControlSparsityPattern();
            
            %  Evalaute at random points
            numRandEvals = 3;
            for randIdx = 1:numRandEvals
                paramData.SetControlVec(...
                    obj.GetRandomVector(controlUpperBound,controlLowerBound));
                obj.EvaluateUserFunction(paramData,funcData);
                obj.ComputeControlJacobian(paramData,funcData);
                obj.UpdateControlSparsityPattern();
            end
        end
        
        function ComputeTimeSparsity(obj,paramData,funcData,timeUpperBound,timeLowerBound)
            %  Computes sparsity pattern of user functions w/r/t to time
           
            %nominalTime = paramData.GetTime();
            
            %  Evaluate at lower bound
            paramData.SetTime(timeLowerBound);
            obj.EvaluateUserFunction(paramData,funcData);
            obj.ComputeTimeJacobian(paramData,funcData);
            obj.UpdateTimeSparsityPattern();
            
            %  Evaluate at upper bound
            paramData.SetTime(timeUpperBound);
            obj.EvaluateUserFunction(paramData,funcData);
            obj.ComputeTimeJacobian(paramData,funcData);
            obj.UpdateTimeSparsityPattern();
            
            %  Evaluate at random points
            numRandEvals = 3;
            for randIdx = 1:numRandEvals
                paramData.SetTime(obj.GetRandomVector(timeUpperBound,timeLowerBound));
                obj.EvaluateUserFunction(paramData,funcData);
                obj.ComputeTimeJacobian(paramData,funcData);
                obj.UpdateTimeSparsityPattern();
            end
            
            %paramData.SetTime(nominalTime);
        end
        
        function UpdateStateSparsityPattern(obj)
            %  Add elements to sparsity pattern for state Jacobians.
            
            %  Dynamics Function
            if obj.hasDynFunctions
                for funcIdx = 1:obj.numDynFunctions
                    for stateIdx = 1:obj.numStateVars
                        if obj.dynFuncStateJac(funcIdx,stateIdx) ~= 0
                            obj.dynFuncStateJacPattern(funcIdx,stateIdx) = 1;
                        end
                    end
                end
            end
            %  Algebraic Functions
            if obj.hasAlgFunctions
                for funcIdx = 1:obj.numAlgFunctions
                    for stateIdx = 1:obj.numStateVars
                        if obj.algFuncStateJac(funcIdx,stateIdx) ~= 0
                            obj.algFuncStateJacPattern(funcIdx,stateIdx) = 1;
                        end
                    end
                end
            end
            %  Cost Function
            if obj.hasCostFunction
                for stateIdx = 1:obj.numStateVars
                    if obj.costFuncStateJac(1,stateIdx) ~= 0
                        obj.costFuncStateJacPattern(1,stateIdx) = 1;
                    end
                end
            end
        end
        
        function UpdateControlSparsityPattern(obj)
            %  Add elements to sparsity pattern for control Jacobians.
            
            %  Dynamics Function
            if obj.hasDynFunctions
                for funcIdx = 1:obj.numDynFunctions
                    for controlIdx = 1:obj.numControlVars
                        if obj.dynFuncControlJac(funcIdx,controlIdx) ~= 0
                            obj.dynFuncControlJacPattern(funcIdx,controlIdx) = 1;
                        end
                    end
                end
            end
            %  Algebraic Functions
            if obj.hasAlgFunctions
                for funcIdx = 1:obj.numAlgFunctions
                    for controlIdx = 1:obj.numControlVars
                        if obj.algFuncControlJac(funcIdx,controlIdx) ~= 0
                            obj.algFuncControlJacPattern(funcIdx,controlIdx) = 1;
                        end
                    end
                end
            end
            %  Cost Function
            if obj.hasCostFunction
                for controlIdx = 1:obj.numControlVars
                    if obj.costFuncControlJac(1,controlIdx) ~= 0
                        obj.costFuncControlJacPattern(1,controlIdx) = 1;
                    end
                end
            end
        end
        
        function UpdateTimeSparsityPattern(obj)
            %  Add elements to sparsity pattern for time Jacobians.
            
            %  Dynamics Function
            if obj.hasDynFunctions
                for funIdx = 1:obj.numDynFunctions
                    if obj.dynFuncTimeJac(funIdx,1) ~= 0
                        obj.dynFuncTimeJacPattern(funIdx, 1) = 1;
                    end
                end
            end
            %  Algebraic Functions
            if obj.hasAlgFunctions
                for funIdx = 1:obj.numAlgFunctions
                    if obj.algFuncTimeJac(funIdx,1) ~= 0
                        obj.algFuncTimeJacPattern(funIdx, 1) = 1;
                    end
                end
            end
            %  Cost Function
            if obj.hasCostFunction
                if obj.costFuncTimeJac(1,1) ~= 0
                    obj.costFuncTimeJacPattern(1, 1) = 1;
                end
            end
        end
        
    end
    
end

