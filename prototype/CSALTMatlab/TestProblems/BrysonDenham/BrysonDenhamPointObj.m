classdef BrysonDenhamPointObj < userfunutils.UserPointFunction
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
            
            obj.SetCostFunction(finalStateVec(3));
            
            obj.SetAlgFunctions([initialStateVec;
                finalStateVec(1);
                finalStateVec(2);
                initialTime
                finalTime]);
            %
            obj.SetAlgFuncLowerBounds([0 1 0 0 -1 0 1]');
            obj.SetAlgFuncUpperBounds( [0 1 0 0 -1 0 1]');
            
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

