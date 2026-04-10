classdef NLPFuncUtil_Point < collutils.NLPFuncUtil
    %NLPFuncUtil_Point Computes function values, Jacobian, and sparsity for user
    %boundary functions
    %   Detailed explanation goes here
    
    properties (Access = 'private')
        % NLPFunctionData
        DataFuncUtil
        % Integer.  Number of functions
        numFunctions
        % Integer.  Number of variables
        numVars
        % Pointer to Decision Vector Helper Utility
        ptrDecVector
        % Bool. Indicates if problem has state variables
        hasStateVars
    end
    
    methods
        
        %  Initialize the helper class.
        function Initialize(obj,funcProps,funcData,DecVector)
            
            %% Create data helper utility and initialize it
            import collutils.NLPFunctionData
            obj.DataFuncUtil = NLPFunctionData();
            numFuncDependencies = 1;
            obj.DataFuncUtil.Initialize(funcProps.numFunctions,...
                DecVector.numDecisionParams,numFuncDependencies)
            
            %%  Extract data needed later
            obj.numFunctions = funcProps.numFunctions;
            obj.numVars = DecVector.numDecisionParams;
            obj.hasStateVars = funcProps.hasStateVars;
            
            %%  Initialize arrays for state partials
            funcIdxs = 1:funcProps.numFunctions;
            % A is all zeros so nothing to do.
            obj.DataFuncUtil.InsertBMatPartition(eye(obj.numFunctions),funcIdxs,funcIdxs)
            if obj.hasStateVars
                %  D matrix sub element for initial state Jacobian
                obj.DataFuncUtil.InsertDMatPartition(...
                    funcProps.initStateJacPattern,funcIdxs,funcData.initialStateIdxs)
                %  D matrix sub element of final state Jacobian
                obj.DataFuncUtil.InsertDMatPartition(...
                    funcProps.finalStateJacPattern,funcIdxs,funcData.finalStateIdxs)
            end
            
            %%  Initialize arrays for time Jacobian partials
            %  D matrix sub element for initial time Jacobian
            obj.DataFuncUtil.InsertDMatPartition(...
                funcProps.initTimeJacPattern,funcIdxs,1);
            %  D matrix sub element of final time Jacobian
            obj.DataFuncUtil.InsertDMatPartition(...
                funcProps.finalTimeJacPattern,funcIdxs,2);
            
        end
        
        function UpdateUserFunArrays(obj,funcData)
            %  Update arrays that are not constant (q, and dq/dz)
            
            % A is all zeros so nothing to do.
            funcIdxs = 1:obj.numFunctions;
            obj.DataFuncUtil.InsertQVecPartition(funcData.GetFunctionValues(),funcIdxs);
            if obj.hasStateVars
                %  D matrix sub element for initial state Jacobian
                obj.DataFuncUtil.InsertparQMatPartition(...
                    funcData.initStateJac,funcIdxs,funcData.initialStateIdxs)
                %  D matrix sub element of final state Jacobian
                obj.DataFuncUtil.InsertparQMatPartition(...
                    funcData.finalStateJac,funcIdxs,funcData.finalStateIdxs)
            end
            
            %%  Initialize arrays for time Jacobian partials
            %  D matrix sub element for initial time Jacobian
            obj.DataFuncUtil.InsertparQMatPartition(...
                funcData.initTimeJac,funcIdxs,1)
            %  D matrix sub element of final state Jacobian
            obj.DataFuncUtil.InsertparQMatPartition(...
                funcData.finalTimeJac,funcIdxs,2)
            
        end        
        
        function funcValues = ComputeFunctions(obj)
            %  Returns the user function values
            funcValues = obj.DataFuncUtil.ComputeFunctions();
        end      
        
        function jacArray = ComputeJacobian(obj)
            % Returns th user function Jacobian
            jacArray = obj.DataFuncUtil.ComputeJacobian();
        end     
        
        function jacArray = ComputeSparsity(obj)
            % Returns the sparsity pattern of the user functions
            jacArray = obj.DataFuncUtil.ComputeSparsityPattern();
        end
        
    end
    
end

