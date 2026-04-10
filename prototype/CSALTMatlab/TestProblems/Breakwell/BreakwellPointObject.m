classdef BreakwellPointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
                     
            %  Linkage constraints
            link1 = [obj.GetInitialTime(2) - obj.GetFinalTime(1); ...
                obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1)];
            link1LowerBound = zeros(3,1);
            link1UpperBound = zeros(3,1);
            
            link2 = [obj.GetInitialTime(3) - obj.GetFinalTime(2); ...
                obj.GetInitialStateVec(3) - obj.GetFinalStateVec(2)];
            link2LowerBound = zeros(3,1);
            link2UpperBound = zeros(3,1);
            
            obj.SetAlgFunctions([obj.GetInitialTime(1);
                obj.GetInitialStateVec(1);
                obj.GetFinalTime(3);
                obj.GetFinalStateVec(3);
                link1;
                link2]);
            
            obj.SetAlgFuncLowerBounds([0  0 1 1  0 -1 link1LowerBound' link2LowerBound']');
            obj.SetAlgFuncUpperBounds([0  0 1 1  0 -1 link1UpperBound' link2UpperBound']');
            
            
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



