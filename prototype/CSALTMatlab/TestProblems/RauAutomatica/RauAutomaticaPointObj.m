classdef RauAutomaticaPointObj < userfunutils.UserPointFunction
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
            
            obj.SetCostFunction (-finalStateVec(1));
            
            obj.SetAlgFunctions([
                initialTime;
                finalTime;
                initialStateVec(1)
                ]);

            obj.SetAlgFuncLowerBounds([0 2 1]');
            obj.SetAlgFuncUpperBounds( [0 2 1]');

        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

