classdef MoonLanderPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Define constants
            g = 1.0;
            E = 2.349;
            
            % Get state and control
            stateVec = obj.GetStateVec();
            T = obj.GetControlVec();
            h = stateVec(1);
            v = stateVec(2);
            m = stateVec(3);
            
            %  Compute the dynamics functions
            obj.SetDynFunctions([v;
                -g + T/m;
                -T/E]);
            
            %  Compute algebraic path constraints for first phase
           obj.SetAlgFunctions([ T h v m]');
           obj.SetAlgFuncUpperBounds([1.227 20 20 1]');
           obj.SetAlgFuncLowerBounds([0 -20 -20 0.01]');
            
            %  Compute the integrand of the cost function
            obj.SetCostFunction(T)
            
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



