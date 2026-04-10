classdef HyperSensitivePointObject < userfunutils.UserPointFunction
    %UNTITLED3 Summar of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods

        function EvaluateFunctions(obj)
            
            obj.SetAlgFunctions([obj.GetInitialTime(1);obj.GetFinalTime(1);...
                obj.GetInitialStateVec(1);obj.GetFinalStateVec(1)]);

            obj.SetAlgFuncLowerBounds([0 10000 1 1.5]');
            obj.SetAlgFuncUpperBounds([0 10000 1 1.5]');
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end
