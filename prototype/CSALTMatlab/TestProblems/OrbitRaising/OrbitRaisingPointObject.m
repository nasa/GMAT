classdef OrbitRaisingPointObject < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            %%  Define constants
            mu   = 1;
            
            %%  Extract state and control components
            finalStateVec = obj.GetFinalStateVec(1);
            r     = finalStateVec(1,1);
            theta = finalStateVec(2,1);
            dr_dt = finalStateVec(3,1);
            dtheta_dt = finalStateVec(4,1);
            orbitRateError = sqrt(mu/r) - dtheta_dt;
            obj.SetAlgFunctions([orbitRateError;
                obj.GetInitialTime(1);
                obj.GetFinalTime(1);
                obj.GetInitialStateVec(1);
                finalStateVec]);
            obj.SetCostFunction(-r);
            obj.SetAlgFuncLowerBounds([0 0 3.32 1 0 0 1 1 -1 -pi 0 -10 0]');
            obj.SetAlgFuncUpperBounds( [0 0 3.32 1 0 0 1 1 10  pi 0  10 1]');
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

