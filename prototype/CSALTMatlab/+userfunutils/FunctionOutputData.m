classdef FunctionOutputData < handle
    %FUNCTIONDATA User Function Data Constainer
    %   This class stores user function data for the optimal control
    %   problem.  This data includes the function values and Jacobians.
    %   The data on this class is used to form NLP function values.
    
    properties
        
        %  Bool. Indicates if user has defined data of this type.
        hasUserFunction = false()
        
    end
    
    properties (Access = protected)
        
        %  Bool. Indicates is intializing or not. Note,
        %  some data can optionally be set b7 the user, and if not provided by user the
        %  system must fill it in.  What is/is not provided by user is
        %  determined during intialization and flags are set accordingly at
        %  that time.
        isInitializing = true;
        
        %  Real array.  The function values
        funcValues
        % Integer.  1 = state and control, 2 state only, 3 control only
        %pointType
        %  Integer.  The number of functions
        numFuncs = 0
        % Real array.  Upper bound on function values
        upperBound
        % Real array.  Lower bound on function values
        lowerBound
		% String array. String description of function values
		funcNames
        
    end
    
    properties (Access = 'private')
        % Bool. Indicates if user provided state Jacobian
        hasUserStateJac = false;
        % Bool. Indicates if user provided control Jacobian
        hasUserControlJac  = false;
        % Bool. Indicates if user provided time Jacobian
        hasUserTimeJac  = false;
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
        
	    % Sets the name of functions
		function SetFunctionNames(obj,funcNames)
			obj.funcNames = funcNames;
		end
		
		function funcNames = GetFunctionNames(obj)
			funcNames = obj.funcNames;
		end
		
        %  Sets number of functions and function values
        function SetFunctionValues(obj,numFuncs,funcValues)
            %  Sets the number of functions and array of func values
            obj.numFuncs = numFuncs;
            obj.funcValues = funcValues;
            if obj.isInitializing
                obj.hasUserFunction = true;
            end
        end
        
        %  Sets function values
        function SetFunctions(obj,funcValues)
            %  Sets the number of functions and array of func values
            obj.funcValues = funcValues;
            if ~isempty(funcValues) && obj.isInitializing
%                 disp('functionoutputdata')
%                 disp(funcValues)
                obj.hasUserFunction = true;
                obj.numFuncs = length(funcValues);
            end
        end
        
        %% Basic accessor methods
        function SetIsInitializing(obj,isInitializing)
            %  Sets initialization flag
            obj.isInitializing = isInitializing;
        end
		
        function isInitializing = GetIsInitializing(obj)
            %  Sets initialization flag
            isInitializing = obj.isInitializing;
        end
        
        function funcValues = GetFunctionValues(obj)
            %  Returns array of user function values
            funcValues = obj.funcValues;
        end
        
        function SetNumFunctions(obj,numFunctions)
            %  Sets the number of functions
            obj.numFuncs = numFunctions;
        end
        
        function numFuncs = GetNumFunctions(obj)
            %  Gets the number of functions
            numFuncs = obj.numFuncs;
        end
        
        function SetUpperBounds(obj,upperBound)
            %  Sets upper bound on function values
            obj.upperBound = upperBound;
        end
        
        function upperBound = GetUpperBounds(obj)
            %  Gets upper bound on function values
            upperBound = obj.upperBound;
        end
        
        function SetLowerBounds(obj,lowerBound)
            %  Sets lower bound on function values
            obj.lowerBound = lowerBound;
        end
        
        function lowerBound = GetLowerBounds(obj)
            %  Gets lower bound on function values
            lowerBound = obj.lowerBound;
        end

                function obj = UserFunctionData_Path(~)
            %  Constructor
            obj.hasUserStateJac = false();
            obj.hasUserControlJac = false();
            obj.hasUserTimeJac = false();
        end
                
        function SetNLPData(obj,~,meshIdx,stageIdx,~,...
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

