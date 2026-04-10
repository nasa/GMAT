classdef HyperSensitiveMultiPhasePointObject < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods

        function EvaluateFunctions(obj)
            
            %  Linkage constraints
            link1 = [obj.GetInitialTime(2) - obj.GetFinalTime(1); ...
                     obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1)];
            link1LowerBound = zeros(2,1);
            link1UpperBound = zeros(2,1);
            
            obj.SetAlgFunctions([obj.GetInitialTime(1);obj.GetFinalTime(1);...
                obj.GetInitialStateVec(1);obj.GetFinalStateVec(1);
                obj.GetInitialTime(2);obj.GetFinalTime(2);...
                obj.GetInitialStateVec(2);obj.GetFinalStateVec(2); link1]);

            %  
            obj.SetAlgFuncLowerBounds([0 30 1 -1 20 50 -0.2 1 link1LowerBound']');
            obj.SetAlgFuncUpperBounds([0 30 1 1 30 50 0.2 1.0 link1UpperBound']');

        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

