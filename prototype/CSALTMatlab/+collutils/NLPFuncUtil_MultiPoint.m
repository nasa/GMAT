classdef NLPFuncUtil_MultiPoint < collutils.NLPFuncUtil
    %NLPFuncUtil_MultiPoint Computes function values, Jacobian, and sparsity for user
    %boundary functions
    %   Detailed explanation goes here
    
    properties (Access = 'private')
        % NLPFunctionData
        DataFuncUtil
        % Integer.  Number of functions
        numFunctions
        % Pointer to Decision Vector Helper Utility
        ptrDecVector
        % Pointer to manager with the Jacobian data
        ptrFuncManager
        % Integer.  The function offset in the total NLP problem
        funcOffSet
        % Integer.  Number of phases
        numPhases
        % Vector of pointers to phases.
        phaseList
    end
    
    methods
        
        %  Initialize the helper class.
        function Initialize(obj,phaseList,funcData,jacData,numVars)
            
            %% Create data helper utility and initialize it
            import collutils.NLPFunctionData
            obj.DataFuncUtil = NLPFunctionData();
            obj.phaseList = phaseList;
            obj.numFunctions = funcData.GetNumFunctions;
            obj.numPhases = length(phaseList);
            numFuncDependencies = 1;
            obj.DataFuncUtil.Initialize(funcData.GetNumFunctions(),numVars,numFuncDependencies)
            
            %% Initialize A and B matrices.  A is zero so nothing to do.
            funcIdxs = 1:obj.numFunctions;
            obj.DataFuncUtil.InsertBMatPartition(eye(obj.numFunctions),funcIdxs,funcIdxs)
            
            %%  Initialize D Matrix
            for phaseIdx = 1:obj.numPhases
                if obj.phaseList{phaseIdx}.Config.GetHasStateVars()
                    %  D matrix sub element for initial state Jacobian
                    obj.DataFuncUtil.InsertDMatPartition(...
                        jacData.initStateJacPattern{phaseIdx},funcIdxs,jacData.initialStateIdxs{phaseIdx})
                    %  D matrix sub element of final state Jacobian
                    obj.DataFuncUtil.InsertDMatPartition(...
                        jacData.finalStateJacPattern{phaseIdx},funcIdxs,jacData.finalStateIdxs{phaseIdx})
                end
                
                %%  Initialize arrays for time Jacobian partials
                %  D matrix sub element for initial time Jacobian
                obj.DataFuncUtil.InsertDMatPartition(...
                    jacData.initTimeJacPattern{phaseIdx},funcIdxs,jacData.initialTimeIdxs{phaseIdx});
                %  D matrix sub element of final time Jacobian
                obj.DataFuncUtil.InsertDMatPartition(...
                    jacData.finalTimeJacPattern{phaseIdx},funcIdxs,jacData.finalTimeIdxs{phaseIdx});
            end
            
        end
        
        function UpdateUserFunArrays(obj,funcData,jacData)
            %  Update arrays that are not constant (q, and dq/dz)
            
            % A is all zeros so nothing to do.
            funcIdxs = 1:obj.numFunctions;
            if obj.numFunctions > 0
               obj.DataFuncUtil.InsertQVecPartition(funcData.GetFunctionValues(),funcIdxs);
            end
            
            %%  Initialize Q Matrix
            for phaseIdx = 1:obj.numPhases
                if obj.phaseList{phaseIdx}.Config.GetHasStateVars()
                    %  Q matrix sub element for initial state Jacobian
                    obj.DataFuncUtil.InsertparQMatPartition(...
                        jacData.initStateJac{phaseIdx},funcIdxs,jacData.initialStateIdxs{phaseIdx})
                    %  Q matrix sub element of final state Jacobian
                    obj.DataFuncUtil.InsertparQMatPartition(...
                        jacData.finalStateJac{phaseIdx},funcIdxs,jacData.finalStateIdxs{phaseIdx})
                end
                
                %%  Initialize arrays for time Jacobian partials
                %  Q matrix sub element for initial time Jacobian
                obj.DataFuncUtil.InsertparQMatPartition(...
                    jacData.initTimeJac{phaseIdx},funcIdxs,jacData.initialTimeIdxs{phaseIdx});
                %  Q matrix sub element of final time Jacobian
                obj.DataFuncUtil.InsertparQMatPartition(...
                    jacData.finalTimeJac{phaseIdx},funcIdxs,jacData.finalTimeIdxs{phaseIdx});
            end
            
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



