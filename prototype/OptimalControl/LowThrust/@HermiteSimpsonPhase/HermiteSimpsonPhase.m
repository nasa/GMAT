classdef HermiteSimpsonPhase < Phase
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
        %  Number of stages in a mesh interval for state variables
        numStateStagePoints;
        %  Number of stages in a mesh interval for control variables
        numControlStagePoints;
        %  Vector of real: Step size for stages for each mesh interval 
        stageStepSizes
        %  Vector of int:  Number of stages steps per mesh interval
        numStageSteps
    end
    
    properties (SetAccess = 'protected')
        
        %  Time parameters
        totalNumPoints
        %  Vector: mesh step size for each mesh interval
        meshStepSizes
        %  Vector: number of mesh steps for each mesh interval
        numMeshSteps
        
    end
    
    methods
        
        %  Compute Radau points, weights, and differentiation matrix
        function obj = InitTranscription(obj)
            
            %  Set stage properties (i.e. how many stages are there?)
            obj.SetStageProperties()
            obj.ComputeInitialTimeParameters()
            
            %  Compute constant step size chunks for each mesh interval
            obj.totalNumPoints = 1;
            obj.numMeshIntervals = length(obj.meshIntervalFractions)-1;
            obj.meshStepSizes = zeros(obj.numMeshIntervals,1);
            obj.numMeshPoints = 0;
            
            %  Loop over number of mesh intervals and compute step and
            %  point properties for each mesh interval and number of points
            %  in the whole phase
            for i = 1:obj.numMeshIntervals
                %  Compute the number of points in each mesh interval
                %  and the step size in the mesh interval
                obj.numMeshSteps(i) = obj.meshIntervalNumPoints(i)-1;
                obj.numMeshPoints   = obj.numMeshPoints +...
                    obj.numMeshSteps(i);
                obj.meshStepSizes(i) =(obj.meshIntervalFractions(i+1)-...
                    obj.meshIntervalFractions(i))/obj.numMeshSteps(i)*...
                    1;%(obj.finalTime - obj.initialTime) ;
                
                %  Compute state step sizes and number of state steps
                obj.stageStepSizes(i) = obj.meshStepSizes(i)/2;
                obj.numStageSteps(i) = obj.numMeshSteps(i)*2;
                
                %  Compute the total number of points in the phase
                obj.totalNumPoints = obj.totalNumPoints + ...
                    obj.numMeshSteps(i)*(1 + obj.numStateStagePoints);
            end
            obj.numMeshPoints = obj.numMeshPoints + 1;
            
            %  Compute the time vector including all mesh intervals/stages
            %  Loop over mesh intervals
            obj.meshPoints = zeros(obj.totalNumPoints,1);
            cnt     = 0;
            for meshIdx = 1:obj.numMeshIntervals
                %  Loop through the step sizes
                cnt = cnt + 1;
                obj.meshPoints(cnt) = obj.meshIntervalFractions(meshIdx)...
                    *1;%*(obj.finalTime - obj.initialTime);
                for pointIdx = 1:obj.numStageSteps(meshIdx) - 1
                    cnt = cnt + 1;
                    obj.meshPoints(cnt) = obj.meshPoints(cnt-1) + ...
                        obj.stageStepSizes(meshIdx);
                end
            end
            obj.meshPoints(obj.totalNumPoints) = 1;%obj.finalTime;
            
            %  Compute the number of mesh points
            obj.numStatePoints       = obj.numMeshPoints;
            obj.numControlPoints     = obj.numMeshPoints;
            obj.numTimeParams        = 2;
            obj.numCollocationPoints = obj.totalNumPoints-1;
            obj.numDefectConstraints = obj.numStates*...
                (obj.totalNumPoints - 1);
            obj.numPathConstraintPoints = obj.totalNumPoints;

            %  Compute bookkeeping properties of the discretization
            obj.numStateParams   = obj.numStates*...
                obj.numCollocationPoints;
            obj.numControlParams = obj.numControls*...
                obj.numCollocationPoints;
            obj.numStateStagePoints = 1;
            obj.numControlStagePoints = 1;
                        
            obj.numStateParams    = obj.numStates*(obj.numMeshPoints*...
                (1 + obj.numStateStagePoints));
            obj.numControlParams  = obj.numControls*(obj.numMeshPoints* ...
                ( 1+ obj.numControlStagePoints));
            
            obj.numStateParams    = obj.numStates*(obj.totalNumPoints);
            obj.numControlParams  = obj.numControls*(obj.totalNumPoints);
                      
            obj.numDecisionParams = obj.numStateParams + ...
                obj.numControlParams + obj.numStaticParams + ...
                obj.numTimeParams;
            
            %  Set the point types for each of the mesh points and the mesh
            %  and stage index
            %  1 means state and control
            %  2 means state only
            %  3 means control only
            %  For HS, all points are type 1
            obj.timeVectorType = ones(obj.totalNumPoints,1);
            
        end
             
        %  Compute the defect constraints
        function ComputeQuadratures(obj,rhsMatrix,costVec)
            
            if ~obj.PathFunction.hasDynFunctions && ...
                    ~obj.PathFunction.hasCostFunction
                return
            end
            
            %  Loop over number of meshes intervals and compute defect
            %  constraints and cost function
            rhsCount = 1;
            lengthCon = obj.DecVector.numStates;
            conIdx = 1;
            meshCount = 0;
            obj.costFuncIntegral = 0;
            obj.ComputeTimeParameters();
            timeChange = obj.finalTime - obj.initialTime;
            for meshIdx = 1:obj.numMeshIntervals
                
                %  loop over the mesh steps in the current interval
                for stepIdx = 1:obj.numMeshSteps(meshIdx)
                    meshCount = meshCount + 1;
                    
                    if obj.PathFunction.hasCostFunction
                        
                        %  Perform quadrature for cost function
                        if obj.PathFunction.hasCostFunction
                            jk          = costVec(rhsCount,:);
                            jkbarkplus1 = costVec(rhsCount+1,:);
                            jkplus1     = costVec(rhsCount+2,:);
                            obj.costFuncIntegral = ...
                                obj.costFuncIntegral + ...
                               timeChange*obj.meshStepSizes(meshIdx)/6*...
                                (jkplus1 + 4*jkbarkplus1 + jk);
                        end
                        
                    end
                    
                    if obj.PathFunction.hasDynFunctions
                        
                        %  Get state at the mesh and stages
                        yk          = ...
                            obj.DecVector.GetStateAtMeshPoint(meshCount,0)';
                        ykbarkplus1 = ...
                            obj.DecVector.GetStateAtMeshPoint(meshCount,1)';
                        ykplus1     = ...
                            obj.DecVector.GetStateAtMeshPoint(meshCount+1,0)';
                        
                        %  Get the dynamics (ODEs) at mesh and stages
                        fk          = rhsMatrix(rhsCount,:);
                        fkbarkplus1 = rhsMatrix(rhsCount+1,:);
                        fkplus1     = rhsMatrix(rhsCount+2,:);
                        
                        %  Compute the Hermite interpolant constraint
                        hermiteCon = ykbarkplus1 - 0.5*(ykplus1 +  yk)...
                            - timeChange*0.125*...
                            obj.meshStepSizes(meshIdx)*(fk - fkplus1);
                        
                        %  Compute the Hermite quadrature constraint
                        simsonQuad = ykplus1 - yk - ...
                            timeChange*obj.meshStepSizes(meshIdx)/6*...
                            (fkplus1 + 4*fkbarkplus1 + fk );
                        
                        %  Concatenate constraints and update counters
                        conVec(conIdx:conIdx+lengthCon-1,:) = hermiteCon';
                        conIdx = conIdx+lengthCon;
                        conVec(conIdx:conIdx+lengthCon-1,:) = simsonQuad';
                        
                        conIdx = conIdx+lengthCon;
                        
                    end
                    rhsCount = rhsCount + 2;
                end 
            end
            obj.defectConstraintVec = conVec;
        end
        
        %  Configure Time Parameters
        function ComputeTimeParameters(obj)
            GetInitialFinalTime(obj);
            obj.finalTime   = obj.DecVector.GetLastTime();
            obj.initialTime = obj.DecVector.GetFirstTime();
        end
        
        %  Function to set stage properties for the phase type
        function SetStageProperties(obj)
            obj.numStateStagePoints   = 1;
            obj.numControlStagePoints = 1;
            obj.numStagePoints        = 1;
        end
        
        %  Configure the time vector
        function ComputeTimeVector(obj)
            obj.ComputeTimeParameters();
            obj.timeVector = (obj.finalTime - obj.initialTime)*...
                obj.meshPoints + obj.initialTime;
        end
        
        %  Get the mesh index given the point index
        function meshIdx = GetMeshIndex(~,pointIdx)          
            if even(pointIdx)
                meshIdx = pointIdx/2;
            else
                meshIdx = 1 + floor(pointIdx/2);
            end
        end
        
        %  Get the stage index given the point index
        function stageIdx = GetStageIndex(~,pointIdx)
            if even(pointIdx)
                stageIdx = 1;
            else
                stageIdx = 0;
            end
        end
        
    end
end

