classdef OrbitTutorialPathFunc < OrbitTestPathFunc
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        function EvaluateFunctions(obj)
            unScaledState = obj.GetUnScaledState();
            totalAccel = zeros(7,1);
            totalAccel(1:6,1) = obj.ComputeTwoBodyAccel(unScaledState);
            totalAccel(7,1) = 0.0;
            totalAccel = obj.ScaleUtility.ScaleStateRate(totalAccel);
            obj.SetDynFunctions(totalAccel);
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end

