classdef RadauPhase < Phase
    %UNTITLED5 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % Row vector that contains the number of control points for
        % each mesh interval
        meshIntervalNumPoints
        %  Row vector that the time fraction each mesh makes of the whole
        %  phase.  If meshFraction = [1 1 1], then there are three mesh
        %  intervals, and each one is one third of the phase duration
        meshIntervalFractions
        numStateStagePoints;
        numControlStagePoints;
    end
    
    properties (SetAccess = 'protected')
       
        %  Mesh point and radau transcription details
        radauPoints
        radauWeights
        radauDiffMatrix
    end
    
    methods
        
        %  Compute Radau points, weights, and differentiation matrix
        function obj = InitTranscription(obj)
            
            %  Compute Radau points, weights, and differentiation matrix
            [obj.radauPoints,obj.radauWeights,obj.radauDiffMatrix]...
                = lgrPS(obj.meshIntervalFractions, ...
                obj.meshIntervalNumPoints);
            obj.numCollocationPoints = length(obj.radauWeights);
            
            %  Compute the number of mesh points
            obj.numMeshPoints        = obj.numCollocationPoints;
            obj.numStatePoints       = obj.numCollocationPoints + 1;
            obj.numControlPoints     = obj.numCollocationPoints;
            obj.numPathConstraintPoints = obj.numCollocationPoints;
            obj.numTimeParams        = 2;
            obj.meshPoints           = obj.radauPoints/2 + 1/2;
            obj.numDefectConstraints = obj.numStates*...
                obj.numCollocationPoints;
            obj.SetStageProperties();
            
            %  Compute bookkeeping properties of the discretization
            obj.numStateParams   = obj.numStates*obj.numStatePoints;
            obj.numControlParams = obj.numControls*obj.numControlPoints;
            obj.numDecisionParams = obj.numStateParams + ...
                obj.numControlParams + obj.numStaticParams + ...
                obj.numTimeParams;
            
            %  Set the point types for each of the mesh points and the mesh
            %  and stage index
            %  1 means state and control
            %  2 means state only
            %  3 means control only
            obj.timeVectorType = ones(obj.numStatePoints,1);
            obj.timeVectorType(obj.numStatePoints,1) = 2;
        end
        
        %  Configure the time vector
        function ComputeTimeVector(obj)
            obj.ComputeTimeParameters();
            obj.timeVector = (obj.finalTime - obj.initialTime)*...
                (obj.radauPoints + 1)/2 + obj.initialTime;
        end
        
        %  Compute the defect constraints
        function ComputeQuadratures(obj,rhsMatrix,costVec)
            %  Compute defect constraints
            if obj.PathFunction.hasDynFunctions
                obj.ComputeDefectConstraints(rhsMatrix)
            end
            %  Compute cost function
            if obj.PathFunction.hasCostFunction
               obj.ComputeCostFunctionIntegral(costVec)
            end
        end
                
        %  Compute the defect constraints
        function ComputeDefectConstraints(obj,rhsMatrix)
            if obj.PathFunction.hasDynFunctions
                stateMat = obj.DecVector.GetStateArray();
                defectConstraintsLHS = obj.radauDiffMatrix*stateMat;
                obj.ComputeTimeParameters();
                defectConstraints = defectConstraintsLHS - ...
                    0.5*(obj.finalTime -obj.initialTime)*rhsMatrix;
                defectConstraints = reshape(defectConstraints,...
                    obj.numDefectConstraints,1);
                obj.defectConstraintVec = defectConstraints;
            end
        end
        
        %  Compute the cost function
        function ComputeCostFunctionIntegral(obj,costVec)
            if obj.PathFunction.hasCostFunction
                obj.costFuncIntegral = costVec'*obj.radauWeights;
                obj.ComputeTimeParameters();
                obj.costFuncIntegral = 0.5*(obj.finalTime -...
                    obj.initialTime)*obj.costFuncIntegral;
            end
        end
        
        %  Configure time parameters
        function ComputeTimeParameters(obj)
            GetInitialFinalTime(obj);
            obj.finalTime   = obj.DecVector.GetLastTime();
            obj.initialTime = obj.DecVector.GetFirstTime();
        end
              
        %  Function to set stage properties for the phase type
        function SetStageProperties(obj)
            obj.numStateStagePoints   = 0;
            obj.numControlStagePoints = 0;
            obj.numStagePoints        = 0;
        end
        
        %  Get the mesh index given the point index
        function meshIdx = GetMeshIndex(obj,pointIdx)
             meshIdx = pointIdx;
        end
        
        %  Get the stage index given the point index
        function stageIdx = GetStageIndex(~,~)
            stageIdx = 0;
        end
    end
end
