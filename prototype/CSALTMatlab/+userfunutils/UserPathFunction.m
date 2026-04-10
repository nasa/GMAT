classdef UserPathFunction < userfunutils.UserFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = 'private')
        
		% Bool. Indicates if the User Path Function is Initializing
		isInitializing = false();
		
        % Integer.  The number of algebraic functions
        numAlgFunctions = 0;
        % Integer. The number of dynamics functions
        numDynFunctions = 0;
        
        % Bool. Indicates if there is a path function portion to cost
        hasCostFunction = false();
        %  Bool.  Indicates if user function has algebraic functions
        hasAlgFunctions = false();
        %  Bool.  Indicates if user function has dynamics functions
        hasDynFunctions = false();
        
        %  Pointer to FunctionInputData
        paramData
        %  Pointer to PathFunction Container
        funcData
    end
    
    methods
        
        function  Initialize(obj,paramData,funcData)
            %  Intializes the user function object
            
			obj.isInitializing = true();
			
            %  Update pointers to data objects
            obj.SetParamData(paramData);
            obj.SetFuncData(funcData);
            
            %  Call evaluate on the user's function to populate data
            obj.paramData.SetIsPerturbing(false());
            obj.EvaluateFunctions();
            obj.EvaluateJacobians();
            
            obj.funcData.costData.SetIsInitializing(false());
            obj.funcData.dynData.SetIsInitializing(false());
            obj.funcData.algData.SetIsInitializing(false());
            
            %  Set flags according to user configuration
            obj.hasCostFunction = funcData.costData.hasUserFunction;
            obj.hasDynFunctions = funcData.dynData.hasUserFunction;
            obj.numDynFunctions = funcData.dynData.GetNumFunctions();
            obj.hasAlgFunctions = funcData.algData.hasUserFunction;
            obj.numAlgFunctions = funcData.algData.GetNumFunctions();
			
			obj.isInitializing = false();
            
        end
        
        function funcData = EvaluateUserFunction(obj,paramData,funcData)
            %  Calls EvaluateFunctions() on the user function object
            
            %  Update pointers to data objects
            obj.SetParamData(paramData);
            obj.SetFuncData(funcData);
            
            %  Call the user's Evaluate() method
            % try
                obj.EvaluateFunctions();
            % catch
                % error('User Function Failed to Evaluate')
            % end
            
        end
        
        function funcData = EvaluateUserJacobian(obj,paramData,funcData)
            %  Calls EvaluateJacobians() on the user function object
			
            %  Evaluate the functions at the nominal point first
            obj.EvaluateUserFunction(paramData,funcData);
            
            %  Call the user's Evaluate() method
            try
                obj.EvaluateJacobians();
            catch
                error('User Jacobian Failed to Evaluate')
            end
            
        end
        
        function SetParamData(obj,paramData)
            %  Sets pointer to parameter data object
            obj.paramData = paramData;
        end
        
        function SetFuncData(obj,funcData)
            %  Sets pointer to function data object
            obj.funcData = funcData;
        end
        
        function stateVec = GetStateVec(obj)
            % Returns the state vector
            stateVec = obj.paramData.GetStateVec();
        end
        
        function controlVec = GetControlVec(obj)
            % Returns the control vector
            controlVec = obj.paramData.GetControlVec();
        end
        
        function time = GetTime(obj)
            %  Returns the time
            time = obj.paramData.GetTime();
        end
        
        function phaseNum = GetPhaseNumber(obj)
            %  Returns the current phase number
            phaseNum = obj.paramData.GetPhaseNum();
        end
        
        function SetAlgFunctions(obj,funcValues)
            %  Sets values of algebraic functions
            obj.funcData.algData.SetFunctions(funcValues)
        end
		
        function funcValues = GetAlgFunctions(obj,funcValues)
            %  Sets values of algebraic functions
            funcValues = obj.funcData.algData.GetFunctionValues();
        end
        
        function SetAlgFuncUpperBounds(obj,upperBound)
            %  Sets upper bound on function values
            obj.funcData.algData.SetUpperBounds(upperBound);
        end
        
        function upperBound = GetAlgFuncUpperBounds(obj)
            %  Sets lower bound on function values
            upperBound = obj.funcData.algData.GetUpperBounds();
        end
		
        function SetAlgFuncLowerBounds(obj,lowerBound)
            %  Sets lower bound on function values
            obj.funcData.algData.SetLowerBounds(lowerBound);
        end
		
        function lowerBound = GetAlgFuncLowerBounds(obj)
            %  Sets lower bound on function values
            lowerBound = obj.funcData.algData.GetLowerBounds();
        end
		
		function SetAlgFuncNames(obj,funcNames) 
			%  Sets string description of function
			obj.funcData.algData.SetFunctionNames(funcNames);
		end
		
		function funcNames = GetAlgFuncNames(obj) 
			%  Sets string description of function
			funcNames = obj.funcData.algData.GetFunctionNames();
		end
        
        function SetAlgStateJac(obj,jac)
            %  Sets algebraic function state Jacobian
            obj.funcData.algData.SetStateJac(jac)
        end
        function SetAlgControlJac(obj,jac)
            %  Sets algebraic function control Jacobian
            obj.funcData.algData.SetControlJac(jac)
        end
        
        function SetAlgTimeJac(obj,jac)
            %  Sets algebraic function time Jacobian
            obj.funcData.algData.SetTimeJac(jac)
        end
        
        function SetDynFunctions(obj,funcValues)
            %  Sets values of Dynamics functions
            obj.funcData.dynData.SetFunctions(funcValues)
        end
        
        function SetDynStateJac(obj,jac)
            %  Sets dynamics function state Jacobian
            obj.funcData.dynData.SetStateJac(jac)
        end
        function SetDynControlJac(obj,jac)
            %  Sets dynamics function control Jacobian
            obj.funcData.dynData.SetControlJac(jac)
        end
        
        function SetDynTimeJac(obj,jac)
            %  Sets dynamics function time Jacobian
            obj.funcData.dynData.SetTimeJac(jac)
        end
        
        function SetCostFunction(obj,funcValues)
            %  Sets values of cost function
            obj.funcData.costData.SetFunctions(funcValues)
        end
        
        function SetCostStateJac(obj,jac)
            %  Sets cost function state Jacobian
            obj.funcData.costData.SetStateJac(jac)
        end
        
        function SetCostControlJac(obj,jac)
            %  Sets cost function control Jacobian
            obj.funcData.costData.SetControlJac(jac)
        end
        
        function SetCostTimeJac(obj,jac)
            %  Sets cost function time Jacobian
            obj.funcData.costData.SetTimeJac(jac)
        end
        
        function isPerturbing = GetIsPerturbing(obj)
            %  Returns isPerturbing flag
            isPerturbing = obj.paramData.GetIsPerturbing();
        end
        
        function isSparsity = GetIsSparsity(obj)
            %  Returns isSparsity flag
            isSparsity = obj.paramData.GetIsSparsity();
        end
		
        function isInitializing = GetIsInitializing(obj)
            %  Returns isSparsity flag
            isInitializing = obj.isInitializing();
        end
		
        function SetIsInitializing(obj,input)
            %  Returns isSparsity flag
            obj.isInitializing = input;
        end
		
		function numControlVars = GetNumControlVars(obj)
			numControlVars = obj.paramData.GetNumControlVars();
		end
    end
    
    methods (Abstract)
        EvaluateFunctions(obj)
        EvaluateJacobians(obj)
    end
end
