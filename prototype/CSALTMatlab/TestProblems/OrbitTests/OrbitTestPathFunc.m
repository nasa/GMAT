classdef OrbitTestPathFunc < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        ScaleUtility
        gravParam
    end
    
    methods
        function state = GetUnScaledState(obj)
           % Returns the scaled state value
           state = obj.GetStateVec();
           if isrow(state)
               state = state';
           end
           state = obj.ScaleUtility.UnScaleState(state);
        end
        
        function orbitAcc = ComputeTwoBodyAccel(obj,state)
            % Computes two-body accel, assumes input state is dimensional
            orbitAcc(1:3,1) = state(4:6,1);
            orbitAcc(4:6,1) = -obj.gravParam*state(1:3,1)/norm(state(1:3,1))^3;
        end

    end
    
end

