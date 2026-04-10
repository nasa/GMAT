classdef Hull_Problem9_5PointObject < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
                
            
            obj.SetAlgFunctions([obj.GetInitialTime(1);obj.GetFinalTime(1);...
                                 obj.GetInitialStateVec(1)]);
            %
            obj.SetAlgFuncLowerBounds([0 1 0]');
            obj.SetAlgFuncUpperBounds([0 1 0]');
            
        end
        
        function EvaluateJacobians(~)

        end
    end
    
end

