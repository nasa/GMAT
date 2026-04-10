classdef ObstacleAvoidancePathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Define constants
            V = 2.138;
            
            % Get state and control
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            x = stateVec(1);
            y = stateVec(2);
            theta = controlVec(1);
            
            %  Compute the dynamics functions
            xdot = V*cos(theta);
            ydot = V*sin(theta);
            obj.SetDynFunctions([xdot;ydot]);
            
            %  Compute algebraic path constraints for first phase
            con1 = (x - 0.4)^2 + (y - 0.5)^2;
            con2 = (x - 0.8)^2 + (y - 1.5)^2;
            obj.SetAlgFunctions([con1;con2]);
            obj.SetAlgFuncUpperBounds([100;100]);
            obj.SetAlgFuncLowerBounds([0.1;0.1]);
            
            %  Compute the integrand of the cost function
            obj.SetCostFunction(xdot*xdot + ydot*ydot)
            
        end
        
        function EvaluateJacobians(obj)
            
%             %  Define constants
%             V = 2.138;
%             
%             % Get state and control
%             stateVec = obj.GetStateVec();
%             controlVec = obj.GetControlVec();
%             x = stateVec(1);
%             y = stateVec(2);
%             theta = controlVec(1);
%             
%             %  Compute the dynamics functions
%             xdot = V*cos(theta);
%             ydot = V*sin(theta);
%             obj.SetDynFunctions([xdot;ydot]);
%             
%             obj.SetDynStateJac(zeros(2,2));
%             obj.SetDynControlJac([-V*sin(theta);V*cos(theta)]);
%             obj.SetDynTimeJac([0 0]');
%             
%             obj.SetCostStateJac([0 0 ]');
%             obj.SetCostTimeJac(0);
                         
        end
    end
    
end



