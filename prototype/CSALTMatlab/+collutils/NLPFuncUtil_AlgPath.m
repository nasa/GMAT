classdef NLPFuncUtil_AlgPath < collutils.NLPFuncUtil_Path
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = 'protected')
        % NLPFunctionData
        DataFuncUtil
    end
    
    properties (Access = 'private')
        
        % Integer. Number of functions at each discretization point
        numFunctionsPerPoint
        % Integer. Number of points with path functions (lenght of funcData vec)
        numFuncPoints
        % Integer. Number of constraint functions
        numFunctions
        % Bool. Indicates if problem has state variables
        hasStateVars
        % Bool. Indicates if problem has control variables
        hasControlVars
        % Real array. Partial of time at each point w/r/t to initial time
        dTimedTi
        % Real Array. Partial of time at each point w/r/t to final time
        dTimedTf
    end
    
    methods (Access = 'public')
        
        %  Initialize the helper class.
        function Initialize(obj,funcProps,funcData,numDecisionParams,...
                numFuncPoints,dTimedTi,dTimedTf)
            % NOTE.  funcData SHOULD BE A POINTER TO THE DATA IN GMAT.  WE
            % DO NOT WANT TO COPY THAT DATA AS IT CAN BE VERY LARGE.
            
            % Create data helper utility and initialize it
            import collutils.NLPFunctionData
            obj.DataFuncUtil = NLPFunctionData();
            numFuncDependencies = 1;
            
            
            % Set data from intputs
            obj.numFuncPoints = numFuncPoints;
            obj.numFunctionsPerPoint = funcProps.GetNumFunctions();
            obj.numFunctions = obj.numFunctionsPerPoint*obj.numFuncPoints;
            obj.hasStateVars = funcProps.GetHasStateVars();
            obj.hasControlVars = funcProps.GetHasControlVars();
            obj.DataFuncUtil.Initialize(obj.numFunctions,...
                numDecisionParams,numFuncDependencies)
            obj.dTimedTi = dTimedTi;
            obj.dTimedTf = dTimedTf;
            
            %%  Initialize arrays for state partials
            
            % A is all zeros so nothing to do.
            %  Loop over the number of function points
            lowIdx = 1;
            for pointIdx = 1:obj.numFuncPoints
                
                funcIdxs = lowIdx:(lowIdx + obj.numFunctionsPerPoint -1);
                obj.DataFuncUtil.InsertBMatPartition(...
                    eye(obj.numFunctionsPerPoint),funcIdxs,funcIdxs);
                if obj.hasStateVars
                    %  D matrix sub element for initial state Jacobian
                    obj.DataFuncUtil.InsertDMatPartition(...
                        funcProps.GetStateJacPattern(),...
                        funcIdxs,funcData{pointIdx}.GetStateIdxs);
                end
                
                %  Intialize arrays for control partials
                if obj.hasControlVars
                    %  D matrix sub element for initial state Jacobian
                    obj.DataFuncUtil.InsertDMatPartition(...
                        funcProps.GetControlJacPattern(),funcIdxs,...
                        funcData{pointIdx}.GetControlIdxs());
                end
                
                %%  Initialize arrays for time Jacobian partials
                %  D matrix sub element for initial time Jacobian
                obj.DataFuncUtil.InsertDMatPartition(...
                    funcProps.GetTimeJacPattern(),funcIdxs,1);
                % TODO:  Should there be another entry for final time?  Is
                % this a bug? - SPH
                obj.DataFuncUtil.InsertDMatPartition(funcProps.GetTimeJacPattern(),funcIdxs,2);
                
                lowIdx = lowIdx + obj.numFunctionsPerPoint;
            end
            
        end
        
        function [numAZeros,numBZeros,numQZeros] = GetMatrixNumZeros(obj) 
            [numAZeros,numBZeros,numQZeros] = obj.DataFuncUtil.GetMatrixNumZeros();
        end
        
        function [funcValues,jacArray] = ComputeFuncAndJac(obj,funcData)
            %  Computes the function values and Jacobian
            
            %  In GMAT, funcData should be pointer to data because large
            UpdateUserFunArrays(obj,funcData);
            funcValues = ComputeFunctions(obj);
            jacArray = ComputeJacobian(obj);
        end
        
        function jacArray = ComputeSparsity(obj)
            % Compute the sparsity pattern
            jacArray = obj.DataFuncUtil.ComputeSparsityPattern();
        end
        
    end
    
    methods (Access = 'private')
        
        function funcValues = ComputeFunctions(obj)
            %  Compute the function values
            funcValues = obj.DataFuncUtil.ComputeFunctions();
        end
        
        function jacArray = ComputeJacobian(obj)
            % Compute the Jacobian
            jacArray = obj.DataFuncUtil.ComputeJacobian();
        end
        
        function UpdateUserFunArrays(obj,funcData)
            %  Update arrays that are not constant (q, and dq/dz)
            
            lowIdx = 1;
            for pointIdx = 1:obj.numFuncPoints
                funcIdxs = lowIdx:(lowIdx + obj.numFunctionsPerPoint -1);
                meshIdx = funcData{pointIdx}.GetMeshIdx();
                stageIdx = funcData{pointIdx}.GetStageIdx();
                obj.DataFuncUtil.InsertQVecPartition(...
                    funcData{pointIdx}.GetFunctionValues(),funcIdxs);
                if obj.hasStateVars
                    %  D matrix sub element for state Jacobian
                    obj.DataFuncUtil.InsertparQMatPartition(...
                        funcData{pointIdx}.GetStateJac(),funcIdxs,...
                        funcData{pointIdx}.GetStateIdxs())
                end
                
                if obj.hasControlVars
                    %  D matrix sub element for control Jacobian
                    obj.DataFuncUtil.InsertparQMatPartition(...
                        funcData{pointIdx}.GetControlJac(),funcIdxs,...
                        funcData{pointIdx}.GetControlIdxs());
                end
                
                %  D matrix sub element for initial time Jacobian
                obj.DataFuncUtil.InsertparQMatPartition(...
                    funcData{pointIdx}.GetTimeJac()*obj.dTimedTi(pointIdx),funcIdxs,1)
                %  D matrix sub element of final state Jacobian
                obj.DataFuncUtil.InsertparQMatPartition(...
                    funcData{pointIdx}.GetTimeJac()*obj.dTimedTf(pointIdx),funcIdxs,2)
                lowIdx = lowIdx + obj.numFunctionsPerPoint;
            end
            
        end
        
    end
    
end


% %  NEW SPARSE HANDLING
% function jacStruct = ComputeJacobianVectors(obj)
% % Compute the Jacobian
% jacStruct = obj.DataFuncUtil.ComputeJacobianVectors();
% end
% %  NEW SPARSE HANDLING
% function [funcValues,jacStruct] = ComputeFuncAndJacVectors(obj,funcData)
% %  Computes the function values and Jacobian
%
% %  In GMAT, funcData should be pointer to data because large
% UpdateUserFunArrays(obj,funcData);
% funcValues = ComputeFunctions(obj);
% jacStruct = ComputeJacobianVectors(obj);
% end
%
% %  NEW SPARSE HANDLING
% function jacStruct = GetNumAlgPathSparsityNonZeros(obj)
% % Compute the Jacobian
% jacStruct = obj.DataFuncUtil.ComputeJacobianVectors();
% numNonZeros = obj.DataFuncUtil.GetNumNonZeros();
% end

