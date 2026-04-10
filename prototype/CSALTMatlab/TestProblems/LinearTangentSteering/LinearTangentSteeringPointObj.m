classdef LinearTangentSteeringPointObj < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            
            %%  Extract state and control components
            initialTime = obj.GetInitialTime(1);
            finalTime = obj.GetFinalTime(1);
            initialStateVec = obj.GetInitialStateVec(1);
            finalStateVec = obj.GetFinalStateVec(1);
            
            obj.SetCostFunction(finalTime);
            
            x2f = finalStateVec(2);
            x3f = finalStateVec(3);
            x4f = finalStateVec(4);
            obj.SetAlgFunctions([initialTime;...
                initialStateVec;
                x2f;
                x3f;
                x4f]);
            %
            obj.SetAlgFuncLowerBounds([0 0 0 0 0 45 5 0]');
            obj.SetAlgFuncUpperBounds([0 0 0 0 0 45 5 0]');
            
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

