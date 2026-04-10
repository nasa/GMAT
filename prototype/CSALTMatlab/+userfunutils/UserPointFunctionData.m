classdef UserPointFunctionData < userfunutils.UserFunctionData
        
    properties (Access = 'private')
        % Bool. Indicates if user provided state Jacobian
        hasUserStateJac
        % Bool. Indicates if user provided control Jacobian
        hasUserControlJac
        % Bool. Indicates if user provided time Jacobian
        hasUserTimeJac
        %  Integer.  Mesh index
        meshIdx
        % Integer.  Stage index
        stageIdx
        % Integer array.  Control indeces in phase decision vector
        controlIdxs
        % Integer array.  State indeces in phase decision vector
        stateIdxs
        % Real matrix.  The Jacobian of the function w/r/t state
        stateJac
        % Real matrix.  The Jacobian of the function w/r/t control
        controlJac
        % Real matrix.  The Jacobian of the function w/r/t time
        timeJac
    end
    
    methods
        
        function obj = UserFunctionData_Path(~)
            %  Constructor
            obj.hasUserStateJac = false();
            obj.hasUserControlJac = false();
            obj.hasUserTimeJac = false();
        end
        
        function SetDecVecData(obj,stateVec,controlVec)
            % Sets state and control data
            obj.stateVec = stateVec;
            obj.controlVec = controlVec;
        end
        
        function SetNLPData(obj,pointType,meshIdx,stageIdx,timeIdxs,...
                stateIdxs,controlIdxs)
            %  Sets indeces into NLP problem and mesh/stage indeces
            obj.meshIdx = meshIdx;
            obj.stageIdx = stageIdx;
            %            obj.timeIdxs = timeIdxs;
            obj.stateIdxs = stateIdxs;
            obj.controlIdxs = controlIdxs;
        end
        
        function SetStateJac(obj,stateJac)
            % Set the state Jacobian
            obj.stateJac = stateJac;
            if obj.isInitializing
                obj.hasUserStateJac = true;
            end
        end
        
        function SetControlJac(obj,controlJac)
            % Set the control Jacobian
            obj.controlJac = controlJac;
            if obj.isInitializing
                obj.hasUserControlJac = true;
            end
        end
        
        function SetTimeJac(obj,timeJac)
            % Set the time Jacobian
            obj.timeJac = timeJac;
            if obj.isInitializing
                obj.hasUserTimeJac = true;
            end
        end
        
        function  stateJac = GetStateJac(obj)
            % Get the state Jacobian
            stateJac = obj.stateJac;
        end
        
        function controlJac = GetControlJac(obj)
            % Get the control Jacobian
            controlJac = obj.controlJac;
        end
        
        function  timeJac = GetTimeJac(obj)
            % Get the time Jacobian
            timeJac = obj.timeJac;
        end
        
        function flag = GetHasUserStateJac(obj)
            % Returns value of hasUserStateJac
            flag = obj.hasUserStateJac;
        end
        
        function flag = GetHasUserControlJac(obj)
            % Returns value of hasUserControlJac
            flag = obj.hasUserControlJac;
        end
        
        function flag = GetHasUserTimeJac(obj)
            % Returns value of hasUserTimeJac
            flag = obj.hasUserTimeJac;
        end
        
        function idxs = GetStateIdxs(obj)
            % Returns indeces of state into NLP
            idxs = obj.stateIdxs;
        end
        
        function idxs = GetControlIdxs(obj)
            % Returns indeces of state into NLP
            idxs = obj.controlIdxs;
        end
        
        function idxs = GetMeshIdx(obj)
            % Returns indeces of mesh point
            idxs = obj.meshIdx;
        end
        
        function idxs = GetStageIdx(obj)
            % Returns indeces of stage point
            idxs = obj.stageIdx;
        end
        
    end
    
end

