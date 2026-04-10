classdef DecVecTypeBetts < collutils.DecisionVector
    %DecVecTypeBetts DecisionVector organized similar to that used by
    %Bett's.
    % Z = [t_o t_f y_10 u_10 y_11 u_11 ... y_nm u_nm s_1 .. s_o w_1..w_p]
    
    properties
        % Integer. The number of optimization params in one mesh step
        numParamsPerMesh
        % Integer. Number of state vars+control vars in opt. control prob
        numStateAndControlVars
        % Integer.  Number of stage points
        numStagePoints
        % Bool.  Has control at last mesh point (HS does, Radau does not)
        hasControlAtFinalMesh
    end
    
    methods
        
        function SetChunkIndeces(obj)
            %  Compute the start and stop indeces of chunks
            
            
            obj.numStatePoints = (obj.numStateMeshPoints-1)*...
                (1+obj.numStateStagePoints)+1;
            if obj.numStateMeshPoints == obj.numControlMeshPoints
                obj.hasControlAtFinalMesh = true;
            else
                obj.hasControlAtFinalMesh = false;
            end
            
            if obj.hasControlAtFinalMesh
                obj.numControlPoints = (obj.numControlMeshPoints-1)*(1 +...
                    obj.numControlStagePoints) + 1;
            else
                obj.numControlPoints = (obj.numControlMeshPoints)*(1 +...
                    obj.numControlStagePoints);
            end
            
            
            % TODO:  Add error checking.  numControlMeshPoints must be
            % either
            obj.numStatePointsPerMesh = 1 + obj.numStateStagePoints;
            obj.numControlPointsPerMesh = 1 + obj.numControlStagePoints;
            obj.numStateParams    = obj.numStateVars*obj.numStatePoints;
            obj.numControlParams  = obj.numControlVars*obj.numControlPoints;
            obj.numDecisionParams   = obj.numStateParams...
                + obj.numControlVars*obj.numControlPoints + ...
                obj.numIntegralParams + obj.numStaticParams + 2;
            
            obj.timeStartIdx     = 1;
            obj.timeStopIdx      = obj.timeStartIdx +  1;
            %
            obj.staticStartIdx   = 2 + obj.numStateParams + ...
                obj.numControlParams + 1;
            obj.staticStopIdx    = obj.staticStartIdx + ...
                obj.numStaticParams - 1;
            
            obj.integralStartIdx = obj.staticStopIdx + 1;
            obj.integralStopIdx  = obj.integralStartIdx + ...
                obj.numIntegralParams - 1;
            
            obj.numParamsPerMesh = ...
                (obj.numStateMeshPoints +obj.numStateStagePoints)*...
                obj.numStateVars+...
                (obj.numControlMeshPoints + obj.numControlStagePoints)*...
                obj.numControlVars;
            obj.numStateAndControlVars = obj.numStateVars + ...
                obj.numControlVars;
            obj.numStagePoints = obj.numStateStagePoints;
            if obj.numStateStagePoints ~= obj.numControlStagePoints
                errorMsg = ['numStateStagePoints must be equal to '...
                    'numControlStagePoints'];
                errorLoc  = 'DecVecTypeBetts:SetChunkIndeces';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        function idx = GetStateIdxsAtMeshPoint(obj,meshIdx,stageIdx)
            %  Extract state param indeces at mesh point and stage point
            obj.ValidateMeshStageIndeces(meshIdx,stageIdx)
            startIdx = obj.timeStopIdx + ...
                (meshIdx - 1)*(obj.numStagePoints+1)*...
                obj.numStateAndControlVars + ...
                (stageIdx)*obj.numStateAndControlVars+1;
            stopIdx = startIdx + (obj.numStateVars-1);
            idx = startIdx:stopIdx;
        end
        
        function SetStateVector(obj,meshIdx,stageIdx,stateVec)
            %  Given mesh, state and stage, insert into decision vector
            stateIdxs = obj.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
            obj.decisionVector(stateIdxs) = stateVec;
        end
        
        function stateVec = GetStateVector(obj,meshIdx,stageIdx)
            %  Given state and stage indeces, extract state from dec. vec.
            stateIdxs = obj.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
            stateVec = obj.decisionVector(stateIdxs);
        end
        
        function SetStateArray(obj,stateArray)
            %  Given a state array, insert states in the decision vector
            
            [row,col] = size(stateArray);
            if  row ~= obj.numStatePoints || col ~= obj.numStateVars
                errorMsg = 'State Array is not valid dimension';
                errorLoc  = 'DecisionVector:SetStateArray';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Loop through the matrix setting and insert into dec. vec.
            rowIdx = 0;
            for meshIdx = 1:obj.numStateMeshPoints - 1
                for stageIdx = 0:obj.numStateStagePoints
                    rowIdx = rowIdx + 1;
                    stateVector = stateArray(rowIdx,:);
                    idxs = obj.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
                    obj.decisionVector(idxs) = stateVector;
                end
            end
            %  Set the last mesh point
            stateVector = stateArray(rowIdx + 1,:);
            idxs = obj.GetStateIdxsAtMeshPoint(obj.numStateMeshPoints,0);
            obj.decisionVector(idxs) = stateVector;
            
        end
        
        function stateArray = GetStateArray(obj)
            %  Get the array of state params for all mesh/stage points
            
            %  stateArray is numStatePoints x numStateVars
            %
            %  stateArray = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
            %               [X2 Y2 Z2 VX2 VY2 VZ2 M2]
            %               [XN YN ZN VXN VYN VZN M1]
            
            %  Loop through the mesh and stages and populate the array
            %  stateArray = zeros(obj.numStatePoints,obj.numStateVars);
            %  Note:  all isa gradient and isGrad stuff is to work around
            %  ugliness with intlab.  Not necessary for GMAT.
            stateArray = [];
            rowIdx = 0;
            vecIdx = 1;
            for meshIdx = 1:obj.numStateMeshPoints - 1
                for stageIdx = 0:obj.numStateStagePoints
                    idxs = obj.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
                    rowIdx = rowIdx + 1;
                    for stateIdx = 1:obj.numStateVars
                        colIdx = idxs(stateIdx);
                        if isempty(stateArray)
                            stateArray = obj.decisionVector(idxs)';
                        else
                            stateArray(rowIdx,stateIdx) = obj.decisionVector(colIdx);
                        end
                    end
                end
            end
            %  Set the last mesh point
            idxs = obj.GetStateIdxsAtMeshPoint(obj.numStateMeshPoints,0);
            rowIdx = rowIdx + 1;
            for stateIdx = 1:obj.numStateVars
                colIdx = idxs(stateIdx);
                stateArray(rowIdx,stateIdx) = obj.decisionVector(colIdx);
            end
        end
        
        function idx = GetControlIdxsAtMeshPoint(obj,meshIdx,stageIdx)
            %  Extract control param indeces at mesh point and stage point
            obj.ValidateMeshStageIndeces(meshIdx,stageIdx)
            startIdx = obj.timeStopIdx + ...
                (meshIdx - 1)*(obj.numStagePoints+1)*...
                obj.numStateAndControlVars + ...
                (stageIdx)*obj.numStateAndControlVars+ obj.numStateVars+1;
            stopIdx = startIdx + (obj.numControlVars-1);
            idx = startIdx:stopIdx;
        end
        
        function SetControlVector(obj,meshIdx,stageIdx,controlVec)
            %  Given mesh, state and stage, insert into decision vector
            controlIdxs = obj.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
            obj.decisionVector(controlIdxs) = controlVec;
        end
        
        function controlVec = GetControlVector(obj,meshIdx,stageIdx)
            %  Given state and stage indeces, extract state from dec. vec.
            controlIdxs = obj.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
            controlVec = obj.decisionVector(controlIdxs);
        end
        
        function SetControlArray(obj,controlArray)
            %  Given a control array, insert Controls in the decision vector
            
            if obj.numControlVars == 0
                return
            end

            [row,col] = size(controlArray);
            if  row ~= obj.numControlPoints || col ~= obj.numControlVars
                errorMsg = 'Control Array is not valid dimension';
                errorLoc = 'DecisionVector:SetControlArray';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Loop through the matrix setting and insert into dec. vec.
            rowIdx = 0;
            if obj.hasControlAtFinalMesh
                numMeshLoops = obj.numControlMeshPoints - 1;
            else
                numMeshLoops = obj.numControlMeshPoints;
            end
            for meshIdx = 1:numMeshLoops
                for stageIdx = 0:obj.numControlStagePoints
                    rowIdx = rowIdx + 1;
                    controlVector = controlArray(rowIdx,:);
                    idxs = obj.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
                    obj.decisionVector(idxs) = controlVector;
                end
            end
            %  Set the last mesh point
            if obj.hasControlAtFinalMesh
                controlVector = controlArray(rowIdx + 1,:);
                idxs = obj.GetControlIdxsAtMeshPoint(...
                    obj.numControlMeshPoints,0);
                obj.decisionVector(idxs) = controlVector;
            end
            
        end
        
        function [idxs] = GetFinalStateIdxs(obj)
            idxs = obj.GetStateIdxsAtMeshPoint(obj.numStateMeshPoints,0);
        end
        
        function [idxs] = GetInitialStateIdxs(obj)
            idxs = obj.GetStateIdxsAtMeshPoint(1,0);
        end
        
        function [idxs] = GetFinalTimeIdx(obj);
            idxs = 2;
        end
        
        function [idxs] = GetInitialTimeIdx(obj);
            idxs = 1;
        end
        
        function controlArray = GetControlArray(obj)
            %  Get the array of Control params for all mesh/stage points
            
            %  ControlArray is numControlPoints x numControlVars
            %
            %
            %  Build a dummy W matrix in Eq. 41.
            %  ControlArray = [Ux1 Uy1 Uz1]
            %                 [Ux2 Uy2 Uz2]
            %                 [UxN UyN UzN]
            
            %  Loop through the mesh and stages and populate the array
            if obj.hasControlAtFinalMesh
                numMeshLoops = obj.numControlMeshPoints - 1;
            else
                numMeshLoops = obj.numControlMeshPoints;
            end
            controlArray = zeros(obj.numControlPoints,obj.numControlVars);
            rowIdx = 0;
            controlArray = [];   %  Workaround for Intlab
            for meshIdx = 1:numMeshLoops
                for stageIdx = 0:obj.numControlStagePoints
                    idxs = obj.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
                    rowIdx = rowIdx + 1;
                    for controlIdx = 1:obj.numControlVars
                        colIdx = idxs(controlIdx);
                        if isempty(controlArray)
                            controlArray = obj.decisionVector(idxs);
                        else
                            controlArray(rowIdx,controlIdx) = obj.decisionVector(colIdx);
                        end
                    end
                end
            end
            %  Set the last mesh point
            if obj.hasControlAtFinalMesh
                idxs = obj.GetControlIdxsAtMeshPoint(...
                    obj.numControlMeshPoints,0);
                rowIdx = rowIdx + 1;
                for controlIdx = 1:obj.numControlVars
                    colIdx = idxs(controlIdx);
                    controlArray(rowIdx,controlIdx) = obj.decisionVector(colIdx);
                end
            end
            
        end
        
        function ValidateMeshStageIndeces(obj,meshIdx,stageIdx)
            if stageIdx  > obj.numStagePoints + 1
                errorMsg = ['stageIdx must be < ' ...
                    'numStagePoints + 1'];
                errorLoc  = 'DecVecTypeBetts:GetStateIdxsAtMeshPoint';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if meshIdx  > obj.numStateMeshPoints
                errorMsg = ['meshIdx must be < ' ...
                    'numStateMeshPoints -1'];
                errorLoc  = 'DecVecTypeBetts:GetStateIdxsAtMeshPoint';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
        end
        
    end
    
end

