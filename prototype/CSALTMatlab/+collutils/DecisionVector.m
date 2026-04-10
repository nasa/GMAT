classdef DecisionVector < handle
    
    %  Author: S. Hughes.  steven.p.hughes@nasa.gov
    %
    %  This class manages parts of a decision vector allowing you to create
    %  a decision vector, extract parts, or set parts of the vector.

    
    properties  (SetAccess = 'protected')
        
        % Integer. Number of states in the optimal control problem.
        numStateVars
        % Integer. Number of controls in the optimal control problem.
        numControlVars

        % Integer. Number of mesh points that have state variables
        numStateMeshPoints
        % Integer. Number of mesh points that have control variables
        numControlMeshPoints
        % Integer. Number of stage points that have state variables
        numStateStagePoints;
        % Integer. Number of stage points that have control variables
        numControlStagePoints;
        % Integer. The number of points (mesh+stage) that have state vars
        numStatePoints
        % Integer. The number of points (mesh+stage) that have control vars
        numControlPoints
        % Integer. The number of state points in each mesh interval
        numStatePointsPerMesh
        % Integer. The number of control points in each mesh interval
        numControlPointsPerMesh
        
        % Integer. The number of optimization variables
        numDecisionParams
        % Real Array.  The decision vector
        decisionVector
        % Integer. The number of state optimization variables in NLP
        numStateParams
        % Integer.  The number of control optimization variables in NLP
        numControlParams
        % Integer. Number of integral params in the opt. control problem
        numIntegralParams
        % Integer. Number of static params in the optimal control problem.
        numStaticParams
        
        % Integer.  The index for the start of the integral dec. vec. chunk
        integralStartIdx
        % Integer.  The index for the end of the integral dec. vec. chunk
        integralStopIdx
        % Integer.  The index for the start of the time dec. vec. chunk
        timeStartIdx
        % Integer.  The index for the end of the time dec. vec. chunk
        timeStopIdx
        % Integer.  The index for the start of the static dec. vec. chunk
        staticStartIdx
        % Integer.  The index for the end of the static dec. vec. chunk
        staticStopIdx
        
    end
    
    methods
        
        
        function Initialize(obj,numStateVars, ...
                numControlVars,numIntegralParams,numStaticParams,...
                numStateMeshPoints,numControlMeshPoints,...
                numStateStagePoints,numControlStagePoints)
            % Initialize the decision vector
            
            %  Check that inputs are valid
            if  numStateVars <= 0
                errorMsg = 'numStateVars must be > 0';
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
            obj.numStateVars = numStateVars;
            obj.numControlVars = numControlVars;
            obj.numIntegralParams = numIntegralParams;
            obj.numStaticParams = numStaticParams;
            
            %  Set discretization properites
            obj.numStateMeshPoints    = numStateMeshPoints;
            obj.numControlMeshPoints  = numControlMeshPoints;
            obj.numStateStagePoints   = numStateStagePoints;
            obj.numControlStagePoints = numControlStagePoints;

            obj.SetChunkIndeces()
            obj.decisionVector = zeros(obj.numDecisionParams,1);
            %
        end
        
        function SetDecisionVector(obj,decisionVector)
            %  Set the entire decision vector
            
            %  Check that decision vector is the required length
            [rowSize,colSize] = size(decisionVector);
            if  rowSize ~= obj.numDecisionParams || colSize ~=1
                errorMsg = 'State vector is not valid length';
                errorLoc  = 'DecisionVector:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Set the decision vector
            obj.decisionVector    = decisionVector;
            
        end
        
        function decisionVector = GetDecisionVector(obj)
            %  Get the decision entire vector
            decisionVector = obj.decisionVector;
        end
        
        function SetTimeVector(obj,timeVector)
            %  Given a time vector, insert it in the decision vector
            
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
        
        function timeSubVector = GetTimeVector(obj)
            %  Extract part of decision vector that contains times
            timeSubVector = obj.decisionVector(...
                obj.timeStartIdx:obj.timeStopIdx);
        end
        
        function SetStaticVector(obj,staticVector)
            %  Given a static vector, insert it in the decision vector
            
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
        
        function staticSubVector = GetStaticVector(obj)
            %  Extract part of decision vector that contains static params
            staticSubVector = obj.decisionVector(...
                obj.staticStartIdx:obj.staticStopIdx);
        end
        
        function SetIntegralVector(obj,integralVector)
            %  Given an integral vector, insert it in the decision vector
            
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
        
        function integralSubVector = GetIntegralVector(obj)
            %  Extract part of decision vector that contains integrals
            integralSubVector = obj.decisionVector(...
                obj.integralStartIdx:obj.integralStopIdx);
        end

        function stateVec = GetFirstStateVector(obj)
            %  Get the first state vector
            stateVec = GetStateAtMeshPoint(obj,1);
        end
        
        function stateVec = GetLastStateVector(obj)
            %  Get the last state vector
            stateVec = GetStateAtMeshPoint(obj,obj.numStateMeshPoints);
        end
               
        function time = GetFirstTime(obj)
            %  Get epoch of first point from the decision vector
            time = obj.decisionVector(obj.timeStartIdx);
        end
        
        function time = GetLastTime(obj)
            %  Get epoch of last point from the decision vector
            time = obj.decisionVector(obj.timeStopIdx);
        end
        
        function [timeStartIdx,timeStopIdx] = GetTimeIdxs(obj)
            %  Get indeces of time parameters
            timeStartIdx = obj.timeStartIdx;
            timeStopIdx = obj.timeStopIdx;
        end
        
        function stateVec = GetStateAtMeshPoint(obj,meshIdx,stageIdx)
            %  Extract state vector at given mesh point and stage point
            
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
            vecIndeces = obj.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
            stateVec = obj.decisionVector(vecIndeces);
        end
        
        function controlVec = GetControlAtMeshPoint(obj,meshIdx,stageIdx)
            %  Extract control vector at given mesh point and stage point
            
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
            vecIndeces = obj.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
            controlVec = obj.decisionVector(vecIndeces);
        end
        
        function stateVec = GetInterpolatedStateVector(obj,time)
            % NOTE THIS IS A DUMMY IMPLEMENTATION THAT CURRENTLY ONLY TESTS
            % THAT THE INTERFACES ARE WORKING
            warning('Phase.GetInterpolatedStateVector is not implemented')
            stateVec = rand(obj.numStateVars,1);
        end
        
        function controlVec = GetInterpolatedControlVector(obj,time)
            % NOTE THIS IS A DUMMY IMPLEMENTATION THAT CURRENTLY ONLY TESTS
            % THAT THE INTERFACES ARE WORKING
            warning('Phase.GetInterpolatedControlVector is not implemented')
            controlVec = rand(obj.numControlVars,1);
        end
               
    end
    
    methods (Abstract)
        GetStateIdxsAtMeshPoint(obj)
        GetControlArray(obj)
        GetStateArray(obj)
        SetControlArray(obj)
        SetStateArray(obj)
    end
    
end