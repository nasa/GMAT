classdef ImplicitRungeKutta < handle
    %ImplicitRungeKutta Base class (abstract) for IRK based collocation
    %methods
    
    properties (Access = protected)
        % Real array. Part of butcher array
        rhoVec
        % Real array. Part of butcher array
        sigmaMatrix
        % Real array. Part of butcher array
        betaVec
        % Int. The number
        numDefectCons
        % Real array. The non-dimensionalized times of the stages
        stageTimes
        % Real array. The "A" matrix chunk that describes dependency on opt. parameters
        paramDepArray
        % Real array. The "B" matrix that describes dependency on NLP functions
        funcConstArray
        % Int. Number of points that have optimization parameters per step
        numPointsPerStep
        % Integer Array.  Number of stages between mesh points
        numStagePointsPerMesh
        % Integer. Number stage points that have states
        numStateStagePointsPerMesh;
        % Integer. Number of stages that have control
        numControlStagePointsPerMesh;
        % Real array.  The dependency pattern for the A matrix in Betts
        % formulation
        patternAMat
        % Real array.  The dependency pattern for the B matrix in Betts
        % formulation
        patternBMat

    end
    
    methods (Access = public)
        
        function numStagePointsPerMesh = GetNumStagePointsPerMesh(obj)
            % Returns number of stage points in mesh
            numStagePointsPerMesh = obj.numStagePointsPerMesh;
        end
        
        function numStateStagePointsPerMesh = GetNumStateStagePointsPerMesh(obj)
            % Returns number of stage points in mesh that have state params
            numStateStagePointsPerMesh = obj.numStateStagePointsPerMesh;
        end
        
        function numControlStagePointsPerMesh = GetNumControlStagePointsPerMesh(obj)
            % Returns number of stage points in mesh that have control params
            numControlStagePointsPerMesh = obj.numControlStagePointsPerMesh;
        end
        
        function stageTimes = GetStageTimes(obj)
            % Returns real vector of non-dimensionalized stage times
            stageTimes = obj.stageTimes;
        end
        
        function paramDepArray = GetParamDependArray(obj)
            % Returns real array of non-dimensionalized stage times
            paramDepArray = obj.paramDepArray;
        end
        
        function funcConstArray = GetFuncConstArray(obj)
            funcConstArray = obj.funcConstArray;
        end
        
        function numDefectCons = GetNumDefectCons(obj)
            numDefectCons = obj.numDefectCons;
        end
        
        function numPointsPerStep = GetNumPointsPerStep(obj)
            numPointsPerStep = obj.numPointsPerStep;
        end;
        
        function [aChunk,bChunk] = GetDependencyChunk(obj,defectIdx,pointIdx,numVars)
            if defectIdx <= 0 || defectIdx > obj.numDefectCons
                error('Invalid defect constraint index')
            end
            if pointIdx <= 0 || pointIdx > obj.numPointsPerStep
                error('Invalid point index')
            end
            aChunk = obj.patternAMat(defectIdx,pointIdx)*eye(numVars);
            bChunk = obj.patternBMat(defectIdx,pointIdx)*eye(numVars);
        end
        
        function ComputeDependencies(obj)
            
            % Loop over the functions
            obj.patternAMat = zeros(obj.numDefectCons,obj.numPointsPerStep);
            obj.patternBMat = zeros(obj.numDefectCons,obj.numPointsPerStep);
            for funIdx = 1:size(obj.paramDepArray,1)
                % Loop over the variables
                for pointIdx = 1:size(obj.paramDepArray,2)
                    obj.patternAMat(funIdx,pointIdx) = obj.paramDepArray(funIdx,pointIdx);
                    obj.patternBMat(funIdx,pointIdx) = obj.funcConstArray(funIdx,pointIdx);
                end
            end
            
        end
        
        function [aMat,bMat] = ComputeAandB(obj,numVars)
            
            % Loop over the functions
            numRows = size(obj.paramDepArray,1)*numVars;
            numCols = size(obj.paramDepArray,2)*numVars;
            aMat = zeros(numRows,numCols);
            bMat = zeros(numRows,numCols);
            nvM1 = numVars - 1;
            for funIdx = 1:size(obj.paramDepArray,1)
                % Loop over the variables
                rowStartIdx = numVars*funIdx - nvM1;
                for varIdx = 1:size(obj.paramDepArray,2)
                    colStartIdx = numVars*varIdx - nvM1;
                    aMat(rowStartIdx:rowStartIdx+nvM1,colStartIdx:colStartIdx+nvM1) = ...
                        obj.paramDepArray(funIdx,varIdx)*eye(numVars);
                    bMat(rowStartIdx:rowStartIdx+nvM1,colStartIdx:colStartIdx+nvM1) = ...
                        obj.funcConstArray(funIdx,varIdx)*eye(numVars);
                end
            end
            
        end
        
    end
end

