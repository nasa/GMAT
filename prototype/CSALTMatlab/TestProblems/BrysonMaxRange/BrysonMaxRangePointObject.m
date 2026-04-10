classdef BrysonMaxRangePointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            finalStateVec = obj.GetFinalStateVec(1);
            initialStateVec = obj.GetInitialStateVec(1);
            finalTime = obj.GetFinalTime(1);
            
            obj.SetCostFunction(-finalStateVec(1));
            
            obj.SetAlgFunctions([initialStateVec;finalStateVec(2);finalTime]);
            %
            obj.SetAlgFuncLowerBounds([0 0 0 0.1 2]');
            obj.SetAlgFuncUpperBounds([0 0 0 0.1 2]');
            
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



