classdef DecVecTypeOne < collutils.DecisionVector
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    %  The decsion vector is organized according to Eq. 41 of GPOPS II
    %  journal article.
    %  Z = [V' W' Q' t' s']
    %  where
    %  v = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
    %      [X2 Y2 Z2 VX2 VY2 VZ2 M2]
    %      [XN YN ZN VXN VYN VZN M1]
    %  and
    %  w = [Ux1 Uy1 Uz1]
    %      [Ux2 Uy2 Uz2]
    %      [UxN UyN UzN]
    %
    
    properties
        % Integer.  The index for the start of the state dec. vec. chunk
        stateStartIdx
        % Integer.  The index for the end of the state dec. vec. chunk
        stateStopIdx
        % Integer.  The index for the start of the control dec. vec. chunk
        controlStartIdx
        % Integer.  The index for the end of the control dec. vec. chunk
        controlStopIdx
    end
    
    methods
        
        function SetChunkIndeces(obj)
            
            if obj.numStateStagePoints == 0
                obj.numStatePoints = obj.numStateMeshPoints;
            else
                obj.numStatePoints = obj.numStateMeshPoints*(1 +...
                    obj.numStateStagePoints) - 1;
            end
            if obj.numControlStagePoints == 0
                obj.numControlPoints = obj.numControlMeshPoints;
            else
                obj.numControlPoints = obj.numControlMeshPoints*(1 +...
                    obj.numControlStagePoints) - 1;
            end
            obj.numStatePointsPerMesh = 1 + obj.numStateStagePoints;
            obj.numControlPointsPerMesh = 1 + obj.numControlStagePoints;
            obj.numStateParams    = obj.numStateVars*obj.numStatePoints;
            obj.numControlParams  = obj.numControlVars*obj.numControlPoints;
            obj.numDecisionParams   = obj.numStateParams...
                + obj.numControlVars*obj.numControlPoints + ...
                obj.numIntegralParams + obj.numStaticParams + 2;
            
            %  Compute the start and stop indeces of chunks
            obj.stateStartIdx    = 1;
            obj.stateStopIdx     = obj.numStateParams;
            obj.controlStartIdx  = obj.numStateParams + 1;
            obj.controlStopIdx   = obj.controlStartIdx + ...
                obj.numControlParams - 1;
            obj.integralStartIdx = obj.controlStopIdx + 1;
            obj.integralStopIdx  = obj.integralStartIdx + ...
                obj.numIntegralParams - 1;
            obj.timeStartIdx     = obj.integralStopIdx + 1;
            obj.timeStopIdx      = obj.timeStartIdx +  1;
            obj.staticStartIdx   = obj.timeStopIdx + 1;
            obj.staticStopIdx    = obj.staticStartIdx + ...
                obj.numStaticParams - 1;
            

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
            
            stateVector = reshape(stateArray,...
                obj.numStateParams,1);
            obj.decisionVector(obj.stateStartIdx:obj.stateStopIdx,1)...
                = stateVector;
            
        end
        
        function SetControlArray(obj,controlArray)
            %  Given a control array, insert controls in decision vector
            
            [row,col] = size(controlArray);
            if  row ~= obj.numControlPoints || col ~= obj.numControlVars
                errorMsg = 'Control Array is not valid dimension';
                errorLoc  = 'DecisionVector:SetControlArray';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            controlVector = reshape(controlArray,obj.numControlParams,1);
            obj.decisionVector(obj.controlStartIdx:obj.controlStopIdx,1)...
                = controlVector;
            
        end
        
        function stateArray = GetStateArray(obj)
            %  Get the array of state params for all mesh/stage points
            
            %  stateArray is numStateParams x numStateVars
            %
            %  stateArray = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
            %               [X2 Y2 Z2 VX2 VY2 VZ2 M2]
            %               [XN YN ZN VXN VYN VZN M1]
            
            stateArray = reshape(obj.decisionVector(obj.stateStartIdx:...
                obj.stateStopIdx),obj.numStatePoints,obj.numStateVars);
            
        end
        
        function controlArray = GetControlArray(obj)
            %  Get the array of control params for all mesh/stage points
            
            %  controlArray is numControlMeshPoints x numControlVars
            %
            %  Build a dummy W matrix in Eq. 41.
            %  W = [Ux1 Uy1 Uz1]
            %      [Ux2 Uy2 Uz2]
            %      [UxN UyN UzN]
            
            controlArray = reshape(obj.decisionVector...
                (obj.controlStartIdx:obj.controlStopIdx),...
                obj.numControlPoints,obj.numControlVars);
            
        end
        
        function idx = GetStateIdxsAtMeshPoint(obj,meshIdx,stageIdx)
            %  Extract state param indeces at mesh point and stage point
            startIdx = (obj.stateStartIdx - 1) + ...
                (meshIdx-1)*obj.numStatePointsPerMesh + ...
                1 + stageIdx;
            step     = obj.numStatePoints;
            stopIdx = startIdx + step*(obj.numStateVars-1);
            idx = startIdx:step:stopIdx;
        end
        
        function vecIndeces = GetControlIdxsAtMeshPoint(obj,meshIdx,stageIdx)
            %  Extract control param indeces at mesh point and stage point         
            startIdx = (obj.controlStartIdx - 1) + ...
                (meshIdx-1)*obj.numControlPointsPerMesh + ...
                1 + stageIdx;
            step     = obj.numControlPoints;
            stopIdx = startIdx + step*(obj.numControlVars-1);
            vecIndeces = startIdx:step:stopIdx;
        end
        
    end
    
end

