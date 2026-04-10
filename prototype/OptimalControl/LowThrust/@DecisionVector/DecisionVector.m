classdef DecisionVector < handle
    
    %  Author: S. Hughes.  steven.p.hughes@nasa.gov
    %
    %  This class manages parts of a decision vector allowing you to create
    %   a decision vector, extract parts, or set parts of the vector.
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
    
    properties  (SetAccess = 'protected')
        
        %  Properties for continuous optimal control problem
        numStates
        numControls
        numIntegralParams
        numStaticParams
        
        %  Properties of the discretization of the state and controls
        numStateMeshPoints
        numControlMeshPoints
        numStateStagePoints;
        numControlStagePoints;
        numStatePoints
        numControlPoints
        numStatePointsPerMesh
        numControlPointsPerMesh
        
        %  Properties of the complete decision variable vector
        lengthDecVector
        decisionVector
        numStateParams
        numControlParams
        
        %  Vector Chunking Ideces
        stateStartIdx
        stateStopIdx
        controlStartIdx
        controlStopIdx
        integralStartIdx
        integralStopIdx
        timeStartIdx
        timeStopIdx
        staticStartIdx
        staticStopIdx
        
    end
    
    methods
        
        %% Initialize the decision vector. Set properties. Validate values.
        function Initialize(obj,numStates, ...
                numControls,numIntegrals,numStaticParams,...
                numStateMeshPoints,numControlMeshPoints,...
                numStateStagePoints,numControlStagePoints)
            
            %  Check that inputs are valid
            if  numStates <= 0
                errorMsg = 'numStates must be > 0';
                errorLoc  = 'DecisionVector:ConfigureDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            %  if  numControls <= 0
            %     errorMsg = 'numControls must be > 0';
            %     errorLoc  = 'DecisionVector:ConfigureDecisionVector';
            %     ME = MException(errorLoc,errorMsg);
            %     throw(ME);
            %  end
            if  numStateMeshPoints <= 0
                errorMsg = 'numStateMeshPoints must be > 0';
                errorLoc  = 'DecisionVector:ConfigureDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if  numControlMeshPoints <= 0
                errorMsg = 'numControlMeshPoints must be > 0';
                errorLoc  = 'DecisionVector:ConfigureDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Set optimal control problem sizes
            obj.numStates         = numStates;
            obj.numControls       = numControls;
            obj.numIntegralParams = numIntegrals;
            obj.numStaticParams   = numStaticParams;
            
            %  Set discretization properites
            obj.numStateMeshPoints    = numStateMeshPoints;
            obj.numControlMeshPoints  = numControlMeshPoints;
            obj.numStateStagePoints   = numStateStagePoints;
            obj.numControlStagePoints = numControlStagePoints;
            
            if obj.numStateStagePoints == 0
                obj.numStatePoints = numStateMeshPoints;
            else
                obj.numStatePoints        = numStateMeshPoints*(1 +...
                    numStateStagePoints) - 1;
            end
            if obj.numControlStagePoints == 0
                obj.numControlPoints = numControlMeshPoints;
            else
            obj.numControlPoints        = numControlMeshPoints*(1 +...
                numControlStagePoints) - 1;
            end
            obj.numStatePointsPerMesh = 1 + numStateStagePoints;
            obj.numControlPointsPerMesh = 1 + numControlStagePoints;
            obj.numStateParams    = numStates*obj.numStatePoints;
            obj.numControlParams  = numControls*obj.numControlPoints;
            obj.lengthDecVector   = obj.numStates*obj.numStatePoints...
                + obj.numControls*obj.numControlPoints + ...
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
            %
        end
        
        function SetDecisionVector(obj,decisionVector)
            
            %  Check that decision vector is the required length
            [rowSize,colSize] = size(decisionVector);
            if  rowSize ~= obj.lengthDecVector || colSize ~=1
                errorMsg = 'State vector is not valid length';
                errorLoc  = 'DecisionVector:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Set the decision vector
            obj.decisionVector    = decisionVector;
            
        end
        
        %  Given a state array, insert it in the decision vector
        function SetStateArray(obj,stateArray)
            
            [row,col] = size(stateArray);
            if  row ~= obj.numStatePoints || col ~= obj.numStates
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
        
        %  Given a state vector, insert it in the decision vector
        function SetStateVector(obj,stateVector)
            
            [row,col] = size(stateVector);
            if  row ~= obj.numStateParams || col ~= 1
                errorMsg = 'State Vector is not valid dimension';
                errorLoc  = 'DecisionVector:SetStateVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            obj.decisionVector(obj.stateStartIdx:obj.stateStopIdx,1)...
                = stateVector;
            
        end
        
        %  Given a control array, insert it in the decision vector
        function SetControlArray(obj,controlArray)
            
            [row,col] = size(controlArray);
            if  row ~= obj.numControlPoints || col ~= obj.numControls
                errorMsg = 'Control Array is not valid dimension';
                errorLoc  = 'DecisionVector:SetControlArray';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            controlVector = reshape(controlArray,obj.numControlParams,1);
            obj.decisionVector(obj.controlStartIdx:obj.controlStopIdx,1)...
                = controlVector;
            
        end
        
        %  Given a control vector, insert it in the decision vector
        function SetControlVector(obj,controlVector)
            
            [row,col] = size(controlVector);
            if  row ~= obj.numControlParams || col ~= 1
                errorMsg = 'Control Vector is not valid dimension';
                errorLoc = 'DecisionVector:SetControlVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            obj.decisionVector(obj.controlStartIdx:obj.controlStopIdx,1)...
                = controlVector;
            
        end
        
        %  Given a time vector, insert it in the decision vector
        function SetTimeVector(obj,timeVector)
            
            [row,col] = size(timeVector);
            if  row ~= 2 || col ~= 1
                errorMsg = 'Time Vector is not valid dimension';
                errorLoc = 'DecisionVector:SetTimeVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            obj.decisionVector(obj.timeStartIdx:obj.timeStopIdx,1)...
                = timeVector;
            
        end
        
        %  Given a static vector, insert it in the decision vector
        function SetStaticVector(obj,staticVector)
            
            [row,col] = size(staticVector);
            if  row ~= obj.numStaticParams || col ~= 1
                errorMsg = 'Static Vector is not valid dimension';
                errorLoc = 'DecisionVector:SetStaticVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            obj.decisionVector(obj.staticStartIdx:obj.staticStopIdx,1)...
                = staticVector;
            
        end
        
        %  Given an integral vector, insert it in the decision vector
        function SetIntegralVector(obj,integralVector)
            
            [row,col] = size(integralVector);
            if  row ~= obj.numIntegralParams || col ~= 1
                errorMsg = 'Integral Vector is not valid dimension';
                errorLoc = 'DecisionVector:SetIntegralVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            obj.decisionVector(obj.integralStartIdx:...
                obj.integralStopIdx,1)= integralVector;
            
        end
        
        %  Extract part of decision vector that contains states
        function stateSubVector = GetStateSubVector(obj)
            stateSubVector = obj.decisionVector(1:obj.stateStopIdx);
        end
        
        %  Extract part of decision vector that contains controls
        function controlSubVector = GetControlSubVector(obj)
            controlSubVector = obj.decisionVector(obj.controlStartIdx: ...
                obj.controlStopIdx);
        end
        
        %  Extract part of decision vector that contains integrals
        function integralSubVector = GetIntegralSubVector(obj)
            integralSubVector = obj.decisionVector(...
                obj.integralStartIdx:obj.integralStopIdx);
        end
        
        %  Extract part of decision vector that contains static params
        function staticSubVector = GetStaticSubVector(obj)
            staticSubVector = obj.decisionVector(...
                obj.staticStartIdx:obj.staticStopIdx);
        end
        
        %  Extract part of decision vector that contains times
        function timeSubVector = GetTimeSubVector(obj)
            timeSubVector = obj.decisionVector(...
                obj.timeStartIdx:obj.timeStopIdx);
        end
        
        %  Extract the state array
        function stateArray = GetStateArray(obj)
            
            %  stateArray is numStateParams x numStates
            %
            %  stateArray = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
            %               [X2 Y2 Z2 VX2 VY2 VZ2 M2]
            %               [XN YN ZN VXN VYN VZN M1]
            
            stateArray = reshape(obj.decisionVector(obj.stateStartIdx:...
                obj.stateStopIdx),obj.numStatePoints,obj.numStates);
            
        end
        
        %  Extract the state array
        function controlArray = GetControlArray(obj)
            
            %  controlArray is numControlMeshPoints x numControls
            %
            %  Build a dummy W matrix in Eq. 41.
            %  W = [Ux1 Uy1 Uz1]
            %      [Ux2 Uy2 Uz2]
            %      [UxN UyN UzN]
            
            controlArray = reshape(obj.decisionVector...
                (obj.controlStartIdx:obj.controlStopIdx),...
                obj.numControlPoints,obj.numControls);
            
        end
        
        %  Get the first state vector
        function stateVec = GetFirstStateVector(obj)
            stateVec = GetStateAtMeshPoint(obj,1);
        end
        
        %  Get the last state vector
        function stateVec = GetLastStateVector(obj)
            stateVec = GetStateAtMeshPoint(obj,obj.numStateMeshPoints);
        end
        
        %  Get the first control vector
        function stateVec = GetFirstControlVector(obj)
            stateVec = GetControlAtMeshPoint(obj,1);
        end
        
        %  Get the last control vector
        function stateVec = GetLastControlVector(obj)
            stateVec = GetControlAtMeshPoint(obj,obj.numControlPoints);
        end
        
        %  Get first time
        function time = GetFirstTime(obj)
            time = obj.decisionVector(obj.timeStartIdx);
        end
        
        %  Get last time
        function time = GetLastTime(obj)
            time = obj.decisionVector(obj.timeStopIdx);
        end
        
        %  Extract state vector at given mesh point
        function stateVec = GetStateAtMeshPoint(obj,meshIdx,stageIdx)
            % Check that state vector contains the mesh point
            if  meshIdx <= 0 || meshIdx > obj.numStatePoints
                errorMsg = ['meshIdx must be >= 0 and <= ' ...
                    'numStateMeshPoints'];
                errorLoc  = 'DecisionVector:GetStateAtMeshPoint';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Set state to zero if this is a a mesh point
            if nargin == 2
                stageIdx = 0;
            end
            
            %  Compute indeces of state vector components the extract the
            %  state vector
            startIdx = (obj.stateStartIdx - 1) + ...
                (meshIdx-1)*obj.numStatePointsPerMesh + ...
                1 + stageIdx;
            step     = obj.numStatePoints;
            stopIdx = startIdx + step*(obj.numStates-1);
            vecIndeces = startIdx:step:stopIdx;
            stateVec = obj.decisionVector(vecIndeces);
        end
        
        %  Extract control vector at given mesh point
        function controlVec = GetControlAtMeshPoint(obj,meshIdx,stageIdx)
            % Check that state vector contains the mesh point
            if  meshIdx <= 0 || meshIdx > obj.numControlPoints
                errorMsg = ['meshIdx must be >= 0 and <= ' ...
                    ' numControlMeshPoints'];
                errorLoc  = 'DecisionVector:GetControlAtMeshPoint';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Set state to zero if this is a mesh point
            if nargin < 3
                stageIdx = 0;
            end
            
            %  Compute indeces of control vector components the extract the
            %  control vector
            startIdx = (obj.controlStartIdx - 1) + ...
                (meshIdx-1)*obj.numControlPointsPerMesh + ...
                1 + stageIdx;
            step     = obj.numControlPoints;
            stopIdx = startIdx + step*(obj.numControls-1);
            vecIndeces = startIdx:step:stopIdx;
            controlVec = obj.decisionVector(vecIndeces); 
        end
        
    end
    
end