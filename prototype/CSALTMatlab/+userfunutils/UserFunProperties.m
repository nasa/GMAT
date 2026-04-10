classdef UserFunProperties < handle
    %UserFunProperties Contains properties of a user's function and the
    %user function's dependencies

    
    properties (Access = 'private')
        % Real array.  Jacobian pattern for state derivatives
        stateJacPattern
        % Real array.  Jacobian pattern for time derivatives
        timeJacPattern
        % Real array.  Jacobian pattern for control derivatives
        controlJacPattern
        % Int. The number of user functions
        numFunctions
        % Bool. True if user problem has state variables
        hasStateVars
        % Bool. True if user problem has control variables
        hasControlVars
    end
    
    methods
        
        %% Set methods
        function SetStateJacPattern(obj,stateJacPattern)
            % Sets the Jacobian pattern for state derivatives
            obj.stateJacPattern = stateJacPattern;
        end
        function SetTimeJacPattern(obj,timeJacPattern)
            % Sets the Jacobian pattern for time derivatives
            obj.timeJacPattern = timeJacPattern;
        end
        function SetControlJacPattern(obj,controlJacPattern)
            % Sets the Jacobian pattern for control derivatives
            obj.controlJacPattern = controlJacPattern;
        end
        function SetNumFunctions(obj,numFunctions)
            % Sets the number of user functions
            obj.numFunctions = numFunctions;
        end
        function SetHasStateVars(obj,hasStateVars)
            % Sets flag indicatiing if problem has state variables
            obj.hasStateVars = hasStateVars;
        end
        function SetHasControlVars(obj,hasControlVars)
            % Sets flag indicating if problem has control variables
            obj.hasControlVars = hasControlVars;
        end
        
        %% Get Methods
        function stateJacPattern = GetStateJacPattern(obj)
            % Returns the Jacobian pattern for state derivatives
            stateJacPattern = obj.stateJacPattern;
        end
        function timeJacPattern = GetTimeJacPattern(obj)
            % Returns the Jacobian pattern for time derivatives
            timeJacPattern = obj.timeJacPattern;
        end
        function controlJacPattern = GetControlJacPattern(obj)
            % Returns the Jacobian pattern for control derivatives
            controlJacPattern = obj.controlJacPattern;
        end
        function numFunctions = GetNumFunctions(obj)
            % Returns the number of user functions
            numFunctions = obj.numFunctions;
        end
        function hasStateVars = GetHasStateVars(obj)
            % Returns flag indicatiing if problem has state variables
            hasStateVars = obj.hasStateVars;
        end
        function hasControlVars = GetHasControlVars(obj)
            % Returns flag indicatiing if problem has control variables
            hasControlVars = obj.hasControlVars;
        end
    end
    
end

