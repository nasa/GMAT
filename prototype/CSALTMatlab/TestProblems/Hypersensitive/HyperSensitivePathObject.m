classdef HyperSensitivePathObject < userfunutils.UserPathFunction
    %UNTITLED3 Summar of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            %  Computes the user functions.
            
            % Extract paramater data
            y = GetStateVec(obj);
            u = GetControlVec(obj);
            t = GetTime(obj);
            
            %  Evaluate Cost Function
            obj.SetCostFunction(y^2 + u^2);
            
            % Evaluate dnamics and compute Jacobians
            obj.SetDynFunctions(-y^3 + u);
            
        end
        
        function  EvaluateJacobians(obj)
            %  Computes the user Jacobians
            
            y = GetStateVec(obj);
            u = GetControlVec(obj);
            t = GetTime(obj);
            timeFac = 0.0;
            
            %  Cost Function Partials
            obj.SetCostStateJac(2*y);
            obj.SetCostControlJac(2*u);
            obj.SetCostTimeJac(timeFac/100);
            
            %  Dynamics Function Partials
            obj.SetDynStateJac(-3*y^2);
            obj.SetDynControlJac(1);
            obj.SetDynTimeJac(timeFac*3*t^2/100);
            
        end
    end
    
end
