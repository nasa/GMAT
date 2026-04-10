classdef HyperSensitiveMultiPhasePathObject < userfunutils.UserPathFunction
    %UNTITLED3 Summar of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            %  Computes the user functions.
            
            % Extract paramater data
            y = obj.GetStateVec();
            u = obj.GetControlVec();
            t = obj.GetTime();
            timeFac = 0.0;
            
            %  Evaluate Cost Function
            obj.SetCostFunction(0.5*(y^2 + u^2) + timeFac*t/100);
            
            % Evaluate dnamics and compute Jacobians
            obj.SetDynFunctions(-y^3 + u + timeFac*t^3/100);
            
        end
        
        function  EvaluateJacobians(obj)
            %  Computes the user Jacobians
            
            y = obj.GetStateVec();
            u = obj.GetControlVec();
            t = obj.GetTime();
            timeFac = 0.0;
            
            %  Cost Function Partials
            obj.SetCostStateJac(y);
            obj.SetCostControlJac(u);
            obj.SetCostTimeJac(timeFac/100);
            
            %  Dynamics Function Partials
            obj.SetDynStateJac(-3*y^2);
            obj.SetDynControlJac(1);
            obj.SetDynTimeJac(timeFac*3*t^2/100);
            
        end
    end
    
end
