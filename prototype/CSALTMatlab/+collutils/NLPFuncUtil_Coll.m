classdef NLPFuncUtil_Coll < collutils.NLPFuncUtil
    %NLPFuncUtil Base class for computing NLP functions and Jacobians
    
    % ==== Important Behavior Notes.
    %
    % This class has a two step intialization process.  You must first call
    % Initialize(), which will populate data on the input
    % ProbCharacteristics class.  Then you must evaluate user functions
    % according to the problem characteristics, and send user function info
    % into PrepareToOptimize() to complete initialization.
    
    properties (Access = protected)
        
        % ==== Data defining dimensions of transcription properties
        
        % Real Array. Unscaled discretization points for transcription
        discretizationPoints
        % Integer. Number of points where path constraints are applied
        numPathConstraintPoints
        % Integer. The number of discretization points that have controls
        numControlPoints
        % Integer. Number of mesh points in the transcription
        numMeshPoints
        % Integer. Number stage points that have states
        numStateStagePointsPerMesh;
        % Integer. Number of stages that have control
        numControlStagePointsPerMesh;
        % Integer. The number of discretization points that have states
        numStatePoints
        % Integer. The number of stage points in the transcription
        numStagePoints
        % Integer Array.  Number of stages between mesh points
        numStagePointsPerMesh
        % Real array.  Vector of discretization times.
        timeVector
        % Integer.  Number of points in the time vector
        numTimePoints
        % Real.  The time change from the first time to last time in phase.
        deltaTime
        % Integer array.  An array of flags indicating the type of data at
        % each discretization point.  State, control, or state+control
        timeVectorType
        
        % ==== Helper classes
        
        %  NLPFuncData object.  Utility class to manage defect constraints
        DefectNLPData
        %  NLPFuncData oject.  Utility class to manage cost function quadrature
        CostNLPData
        % Pointer to FunctionInputData object.
        PathFuncInputData
        % Pointer to PathFunctionManager object.
        PathFunctionManager
        %  Pointer to ProblemCharactertistics object.  Contains data used
        %  accross utility classes
        ptrConfig
        
        %==== New for three vector handling of NLP data
        % UserFunProperties Object. Contains info on user functions
        dynFunProps
        isFinalized
        
    end
    
    methods
        
        function obj = Initialize(obj,ptrConfig,type)
            %  Performs the first half of the initialization.
            %  ptrConfig is pointer to ProblemCharacterisics object
            
            %  Set pointer to config object
            obj.ptrConfig = ptrConfig;
            
            %  Computes dimensions of items based on the transcription details
            if nargin == 2
                obj.InitializeTranscription()
            elseif nargin == 3
                obj.InitializeTranscription(type)
            end
            
            %  Initialize utilities used to compute function values and
            %  Jacobians given appropriate sparse matrices in Bett's
            %  formulation
            obj.InitNLPHelpers();
            
        end
        
        function [numAZeros,numBZeros,numQZeros] = GetDefectMatrixNumZeros(obj)
            [numAZeros,numBZeros,numQZeros] = obj.DefectNLPData.GetMatrixNumZeros();
        end
        
        function PrepareToOptimize(obj,dynFunProps,dynFunValues,...
                costFunProps,costFunValues,DecVector)
            %  Initializations that must be done after other subsystems
            %  intialize (but after Initialize() above)
            % dynFunValues and costFunValues are vectors of
            % FunctionOutputData objects
            % DecVector is a DecVecTypeBetts Object
            % dynFuncProps and costFuncProps are UserFunProperties Objects
            
            obj.DefectNLPData.SetDecisionVector(DecVector);
            obj.CostNLPData.SetDecisionVector(DecVector);
            obj.dynFunProps = dynFunProps;
            
            %  Initialize matrices that get re-used
            if obj.ptrConfig.GetHasDefectCons()
                obj.InitializeConstantDefectMatrices(dynFunProps,dynFunValues);
            end
            if obj.ptrConfig.GetHasIntegralCost()
                obj.InitializeConstantCostMatrices(costFunProps,costFunValues);
            end
        end
		
		function numMeshPoints = GetNumMeshPoints(obj)
			numMeshPoints = obj.numMeshPoints;
		end
		        
        function numControlPoints = GetNumControlPoints(obj)
            % Returns number of mesh points that have control
            numControlPoints = obj.numControlPoints;
        end
        
        function numStateStagePointsPerMesh = GetNumStateStagePointsPerMesh(obj)
            % Returns numer of state stage points in a mesh
            numStateStagePointsPerMesh = obj.numStateStagePointsPerMesh;
        end
        
        function numControlStagePointsPerMesh = GetNumControlStagePointsPerMesh(obj)
            % Returns the number of control state points in a mesh
            numControlStagePointsPerMesh = obj.numControlStagePointsPerMesh;
        end
        
        function numStatePoints = GetNumStatePoints(obj)
            % Returns total number of discretization points that have state
            numStatePoints = obj.numStatePoints;
        end
        
        function numPathConstraintPoints = GetNumPathConstraintPoints(obj)
            % Returns total number of discretization points with path contraints
            numPathConstraintPoints = obj.numPathConstraintPoints;
        end
        
        function timeVector = GetTimeVector(obj)
            % Returns array of times in the discrization
            timeVector = obj.timeVector;
        end
        
        function timeVectorType = GetTimeVectorType(obj)
            % Returns vector of bools defining discretization point type
            timeVectorType = obj.timeVectorType;
        end
        
        function numTimePoints = GetNumTimePoints(obj)
            % Returns the number of times in the time vector, integer
            numTimePoints = obj.numTimePoints;
        end
        
        function points = GetDiscretizationPoints(obj)
            % Returns the vector of non-dimensional times
            points = obj.discretizationPoints;
        end
        
        function timeAtMeshPoint = GetTimeAtMeshPoint(obj,meshIdx)
            % Returns time as mesh points given mesh index, real
            if meshIdx > length(obj.timeVector)
                error('meshIx must be <= length(timeVector)')
            end
            timeAtMeshPoint = obj.timeVector(meshIdx);
        end
        
        % Given time, state, and control: calls user function and returns
        % dynamics functions at the point.
        function dynData = GetUserDynFuncData(obj,time,stateVec,controlVec)
            % Prepare path function for eval with the desired data
            % input is time (real), stateVec (array), controlVec (array)
            % output is FunctionOutputData object
            obj.PreparePathFunctionOffMesh(time,stateVec,controlVec);
            % Call the phase and ask it to evaluate the function
            import userfunutils.PathFunctionContainer;
            funcData= PathFunctionContainer();
            funcData.Initialize();
            funcData = obj.PathFunctionManager.EvaluateUserFunction(...
                obj.PathFuncInputData,funcData);
            
            % Now get the data
            dynData = funcData.dynData;
        end
        
        
        
        function PreparePathFunctionOffMesh(obj,time,stateVec,controlVec)
            %  Prepares user path function evaluation at a specific point
            % This function extracts the state, control, time for state
            % data from the decision vector
            % input is time (real), stateVec (array), controlVec (array)
            % output is FunctionOutputData object
            obj.PathFuncInputData.SetStateVec(stateVec);
            obj.PathFuncInputData.SetControlVec(controlVec);
            obj.PathFuncInputData.SetTime(time);
        end
        
        function UpdateCostJacMatrices(obj,A,B,D,Q,funIdxs,varIdxs,stateIdxs)
            %  Compute the constraint Jacobian and insert in to master
            %  Jacobian array
            % NOT READY FOR CONVERSION
            conJac = A + B*Q;
            obj.InsertConJac(conJac,funIdxs,varIdxs);
            %  Update matrices used for sparsity determination
            obj.UpdateConAMatrix(A,funIdxs,varIdxs);
            obj.UpdateConBMatrix(B,funIdxs,stateIdxs);
            obj.UpdateConDMatrix(D,funIdxs,varIdxs);
        end
        
        function sparsityPattern = ComputeDefectSparsityPattern(obj)
            % Returns sparsity pattern of defect constraints
            % NOT READY FOR CONVERSION
            sparsityPattern = obj.DefectNLPData.ComputeSparsityPattern();
        end
        
        function sparsityPattern = ComputeCostSparsityPattern(obj)
            % Returns sparsity pattern of integral cost component
            % NOT READY FOR CONVERSION
            sparsityPattern = obj.CostNLPData.ComputeSparsityPattern();
        end
        
        function [funcValue,jacArray] = ComputeCostFunAndJac(obj,funcData,decisionVector)
            %  Computes the defect function values and Jacobian
            
            obj.CostNLPData.SetDecisionVector(decisionVector);
            if obj.ptrConfig.GetHasIntegralCost()
                obj.FillDynamicCostFuncMatrices(funcData)
                funcValue = obj.CostNLPData.ComputeFunctions();
                jacArray = obj.CostNLPData.ComputeJacobian();
            end
            
        end
        
        function [funcValues,jacArray] = ComputeDefectFunAndJac(obj,funcData,decisionVector)
            %  Computes the defect function values and Jacobian
            
            obj.DefectNLPData.SetDecisionVector(decisionVector);
            %  Compute defect constraints
            if obj.ptrConfig.GetHasDefectCons
                if isa(obj,'collutils.NLPFuncUtil_Radau')
                    obj.FillDynamicDefectConMatrices(funcData(1:end-1));
                end
                if isa(obj,'collutils.NLPFuncUtil_ImplicitRungeKutta')
                    obj.FillDynamicDefectConMatrices(funcData(1:end));
                end
                funcValues = obj.DefectNLPData.ComputeFunctions();
                jacArray = obj.DefectNLPData.ComputeJacobian();
            end
            
        end
        
                %  Compute the cost function
        function InitializeConstantCostMatrices(obj,costFunProps,costFunValues)
            %  Fills in the A, B, and D matrices for cost function
            
            %  If nothing to do return
            if ~obj.ptrConfig.GetHasIntegralCost()
                return
            end
            
            %  For cost function, there is only one function!
            conIdx = 1;
            %  Note that A matrix is all zeros for Radau cost quadrature.
            conDMatRD = sparse(obj.numMeshPoints,obj.ptrConfig.GetNumDecisionVarsNLP());
            conBMatRD = sparse(1,obj.numMeshPoints);
            
            %  Loop over the number of points
            start = 1;
            if obj.quadratureType == 1
                stop = length(costFunValues);
            elseif obj.quadratureType == 2
                stop = length(costFunValues) - 1;
            end
            
            for funcIdx = start:stop
                
                %  Increment counters
                meshIdx = costFunValues{funcIdx}.GetMeshIdx();
                conBMatRD(conIdx,funcIdx) = -obj.quadratureWeights(meshIdx);
                
                %  Fill in state Jacobian elements
                if obj.ptrConfig.GetHasStateVars()
                    conDMatRD(funcIdx,costFunValues{funcIdx}.GetStateIdxs()) = ...
                        costFunProps.GetStateJacPattern();
                end
                
                %  Fill in control Jacobian terms
                if obj.ptrConfig.GetHasControlVars()
                    conDMatRD(funcIdx,costFunValues{funcIdx}.GetControlIdxs()) = ...
                        costFunProps.GetControlJacPattern();
                end
                
                %  Initial time terms
                conDMatRD(funcIdx,1) = ones(1,1);
                
                %  Final time terms
                conDMatRD(funcIdx,2) = ones(1,1);
                
            end
            obj.CostNLPData.SetBMatrix(conBMatRD);
            obj.CostNLPData.SetDMatrix(conDMatRD)
        end
        
    end
    
    methods(Abstract,Access = protected)
        InitStateAndControlInterpolator(obj);
        InitializeConstantDefectMatrices(obj);
        %InitializeConstantCostMatrices(obj);
        FillDynamicDefectConMatrices(obj);
        FillDynamicCostFuncMatrices(obj)
    end
end

% %  NEW SPARSE HANDLING
% function [funcValues,jacStruct] = ComputeDefectFunAndJacVectors(obj,funcData,decisionVector)
% %  Computes the defect function values and Jacobian
%
% obj.DefectNLPData.SetDecisionVector(decisionVector);
% %  Compute defect constraints
% if obj.ptrConfig.GetHasDefectCons
%     obj.FillDynamicDefectConMatrices(funcData(1:end-1));
%     funcValues = obj.DefectNLPData.ComputeFunctions();
%     jacStruct = obj.DefectNLPData.ComputeJacobianVectors();
% end
%
% end
%
% 
% %  NEW SPARSE HANDLING
% function [funcValue,jacStruct] = ComputeCostFunAndJacVectors(obj,funcData,decisionVector)
% %  Computes the defect function values and Jacobian
% 
% obj.CostNLPData.SetDecisionVector(decisionVector);
% if obj.ptrConfig.GetHasIntegralCost()
%     obj.FillDynamicCostFuncMatrices(funcData)
%     funcValue = obj.CostNLPData.ComputeFunctions();
%     jacStruct = obj.CostNLPData.ComputeJacobianVectors();
% end
% 
% end
% 
% %  NEW SPARSE HANDLING
% function numNonZeros = GetNumDefectSparsityNonZeros(obj)
% numNonZeros = obj.DefectNLPData.GetNumNonZeros()
% end
% 
% %  NEW SPARSE HANDLING
% function numNonZeros = GetNumCostSparsityNonZeros(obj)
% funcValue = obj.CostNLPData.ComputeFunctions();
% numNonZeros = obj.CostNLPData.GetNumNonZeros();
% end


