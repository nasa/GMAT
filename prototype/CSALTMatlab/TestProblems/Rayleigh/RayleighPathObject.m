classdef RayleighPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Define constants
            p = 0.14;
            
            % Get state and control
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            x1 = stateVec(1);
            x2 = stateVec(2);
            u = controlVec(1);
            
            %  Compute the dynamics functions
            obj.SetDynFunctions([x2;
                -x1 + x2*(1.4 - p*x2*x2) + 4*u]);
            
            %  Compute algebraic path constraints for first phase
%             obj.SetAlgFunctions(u + x1/6.0);
%             obj.SetAlgFuncUpperBounds(0);
%             obj.SetAlgFuncLowerBounds(-1e16);
            
            %  Compute the integrand of the cost function
            obj.SetCostFunction(x1*x1 + u*u)
            
        end
        
        function EvaluateJacobians(obj)
            
%             % Get state
%             stateVec = obj.GetStateVec();
%             x1 = stateVec(1);
%             x2 = stateVec(2);
%             
%             %  Set Dynamics Function Jacobians
%             dynFuncStateJac = [0, 1;
%                 -.4*x1*x2, -0.1*(1+ 2*x1^2)];
%             dynFuncControlJac = [0;1];
%             obj.SetDynStateJac(dynFuncStateJac)
%             obj.SetDynControlJac(dynFuncControlJac)
%             
%             %  Set alg path constraint jacobians
%             phaseNum = obj.GetPhaseNumber();
%             if phaseNum == 1
%                 %  The algebraic path constraints
%                 dcondx1 = - 18*(x1 - 1);
%                 dcondx2 = - 2*((x2 - 0.4)/0.3)/0.3;
%                 obj.SetAlgStateJac([dcondx1;dcondx2]);
%             end
%             
        end
    end
    
end



