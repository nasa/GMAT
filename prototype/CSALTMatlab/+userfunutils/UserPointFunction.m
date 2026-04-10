classdef UserPointFunction < userfunutils.UserFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = 'private')
        
		isInitializing = false();
		
        % Integer.  The number of algebraic functions
        numAlgFunctions = 0;
        
        % Bool. Indicates if there is a path function portion to cost
        hasCostFunction = false();
        %  Bool.  Indicates if user function has algebraic functions
        hasAlgFunctions = false();

        %  Pointer to PathFunction Container
        funcData
        
        %  Vector of pointers to FunctionInputData objects
        initialInputData
        finalInputData
        
        %  Integer.  Number of phases
        numPhases
    end
    
    methods
        
		function isInitializing = IsInitializing(obj)
		
			isInitializing = obj.isInitializing;
		
		end
		
        function Initialize(obj,initialInputData,finalInputData)
            %  Intializes the user function object
            
			obj.isInitializing = true();
			
            %  Update pointers to data objects
            obj.initialInputData = initialInputData;
            obj.finalInputData = finalInputData;
            obj.numPhases = length(initialInputData);
            
            %  Create data containers  
            import userfunutils.UserPointFunctionData;
            import userfunutils.PointFunctionContainer;
            obj.funcData = userfunutils.PointFunctionContainer();
            obj.funcData.Initialize()

            %  Call evaluate on the user's function to populate data
            obj.EvaluateFunctions();

            obj.EvaluateJacobians();
            
            obj.funcData.costData.SetIsInitializing(false());
            obj.funcData.algData.SetIsInitializing(false());
            
            %  Set flags according to user configuration
            obj.hasCostFunction = obj.funcData.costData.hasUserFunction;
            obj.hasAlgFunctions = obj.funcData.algData.hasUserFunction;
            obj.numAlgFunctions = obj.funcData.algData.GetNumFunctions();
			
            
			obj.isInitializing = false();
            
        end
        
        function funcData = EvaluateUserFunction(obj)
            %  Calls EvaluateFunctions() on the user function object
                        
            %  Call the user's Evaluate() method
%             try
                obj.EvaluateFunctions();
%             catch
%                 error('User Function Failed to Evaluate')
%             end
            
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
        
        function ValidatePhaseIndex(obj,phaseIdx)
            if phaseIdx < 1 || phaseIdx > obj.numPhases
                error('Invalid phase index')
            end
        end
                
        function SetFuncData(obj,funcData)
            %  Sets pointer to function data object
            obj.funcData = funcData;
        end
        
        function funcData = GetFuncData(obj)
            %  Sets pointer to function data object
            funcData = obj.funcData;
        end
        
        function stateVec = GetFinalStateVec(obj,phaseIdx)
            % Returns the state vector
            ValidatePhaseIndex(obj,phaseIdx)
            stateVec = obj.finalInputData{phaseIdx}.GetStateVec();
        end
        
        function stateVec = GetInitialStateVec(obj,phaseIdx)
            % Returns the state vector for a given phase Idx
            ValidatePhaseIndex(obj,phaseIdx)
            stateVec = obj.initialInputData{phaseIdx}.GetStateVec();
        end

        function time = GetFinalTime(obj,phaseIdx)
            %  Returns the time
            obj.ValidatePhaseIndex(phaseIdx)
            time = obj.finalInputData{phaseIdx}.GetTime();
        end
        
        function time = GetInitialTime(obj,phaseIdx)
            %  Returns the time
            ValidatePhaseIndex(obj,phaseIdx)
            time = obj.initialInputData{phaseIdx}.GetTime();
        end
               
        function SetAlgFunctions(obj,funcValues)
            %  Sets values of algebraic functions
            obj.funcData.algData.SetFunctions(funcValues)
        end
        
        function SetAlgFuncUpperBounds(obj,upperBound)
            %  Sets upper bound on function values
            obj.funcData.algData.SetUpperBounds(upperBound);
        end
        
        function SetAlgFuncLowerBounds(obj,lowerBound)
            %  Sets lower bound on function values
            obj.funcData.algData.SetLowerBounds(lowerBound);
        end
		
		function SetAlgFuncNames(obj,funcNames) 
			%  Sets string description of function
			obj.funcData.algData.SetFunctionNames(funcNames);
		end
		        
        function SetAlgStateJac(obj,jac)
            %  Sets algebraic function state Jacobian
            obj.funcData.algData.SetStateJac(jac)
        end
        
        function SetAlgTimeJac(obj,jac)
            %  Sets algebraic function time Jacobian
            obj.funcData.algData.SetTimeJac(jac)
        end
                                
        function SetCostFunction(obj,funcValues)
            %  Sets values of cost function
            obj.funcData.costData.SetFunctions(funcValues)
        end
        
        function SetCostStateJac(obj,jac)
            %  Sets cost function state Jacobian
            obj.funcData.costData.SetStateJac(jac)
        end

        function SetCostTimeJac(obj,jac)
            %  Sets cost function time Jacobian
            obj.funcData.costData.SetTimeJac(jac)
        end
                  
    end
    
%     methods (Abstract)
%         EvaluateFunctions(obj)
%         EvaluateJacobians(obj)
%     end
end
