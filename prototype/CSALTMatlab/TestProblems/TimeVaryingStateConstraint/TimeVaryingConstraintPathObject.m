classdef TimeVaryingConstraintPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
                        
            % Get state and control
            stateVec = obj.GetStateVec();
            t = obj.GetTime();
            u = obj.GetControlVec();
            x1 = stateVec(1);
            x2 = stateVec(2);
            
            %  Compute the dynamics functions
            obj.SetDynFunctions([x2;
                -x2 + u]);
            
            %  Compute algebraic path constraints for first phase
           obj.SetAlgFunctions(-x2 + 8*(t - 0.5)^2 - 0.5);
           obj.SetAlgFuncUpperBounds(100);
           obj.SetAlgFuncLowerBounds(0);
            
            %  Compute the integrand of the cost function
            obj.SetCostFunction(x1*x1 + x2*x2 + 0.005*u*u)
            
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



