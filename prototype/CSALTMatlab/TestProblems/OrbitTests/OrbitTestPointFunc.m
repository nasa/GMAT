classdef OrbitTestPointFunc < userfunutils.UserPointFunction
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        function state = GetUnScaledState(obj)
            % Returns the scaled state value
            state = obj.GetStateVec()';
            if isrow(state)
                state = state';
            end
            state = obj.ScaleUtility.UnScaleState(state);
        end
    end
    
end

