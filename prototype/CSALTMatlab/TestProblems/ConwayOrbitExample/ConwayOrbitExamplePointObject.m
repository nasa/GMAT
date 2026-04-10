classdef ConwayOrbitExamplePointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            finalStateVec = obj.GetFinalStateVec(1);
            
            r  = finalStateVec(1);
            vr = finalStateVec(3);
            vt = finalStateVec(4);
            
            obj.SetCostFunction(-(0.5*(vr^2 + vt^2) - 1/r));
                  
            obj.SetAlgFunctions([obj.GetInitialTime(1);
                obj.GetFinalTime(1);
                obj.GetInitialStateVec(1);
                ]);
            
            obj.SetAlgFuncLowerBounds([0 50 1.1 0 0 1/sqrt(1.1)]');
            obj.SetAlgFuncUpperBounds([0 50 1.1 0 0 1/sqrt(1.1)]');

        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



