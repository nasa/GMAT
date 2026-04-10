classdef DummyPathFunction < userfunutils.UserPathFunction
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
            time = obj.GetTime();

            if obj.GetPhaseNumber() == 1
                
                % Test Functions used for dnamics and algebraic funcs
                func = [-y(1)^2*y(2)*y(3)^3 + ...
                    -u(1)^2*u(2)^3 - time^3;
                    -y(3)^2*y(2)*y(1)^3 + ...
                    -u(2)^2*u(1)^3 + time^2];
                
                %  Test function used for cost
                cost = y(1)*y(2)*y(3)*u(1)*u(2)*time;
                
                %  Evaluate Algebraic Function
                fac = pi;
                obj.SetAlgFunctions(fac*func);
                
                %  Evaluate Cost Function
                obj.SetCostFunction(cost);
                
                % Evaluate dnamics and compute Jacobians
                obj.SetDynFunctions(func);
                
            end
            
        end
        
        function  EvaluateJacobians(obj)
            %  Computes the user Jacobians
            
            y = obj.GetStateVec();
            u = obj.GetControlVec();
            time = obj.GetTime();
            useAnalticPartials = false();
            
            funcStateJac = [-2*y(1)*y(2)*y(3)^3,...
                -y(1)^2*y(3)^3,...
                -3*y(1)^2*y(2)*y(3)^2;...
                -3*y(3)^2*y(2)*y(1)^2,...
                -y(3)^2*y(1)^3,...
                -2*y(3)*y(2)*y(1)^3];
            
            funcControlJac = [-2*u(1)*u(2)^3,...
                -3*u(1)^2*u(2)^2;...
                -3*u(2)^2*u(1)^2,...
                -2*u(2)*u(1)^3];
            
            funcTimeJac = [-3*time^2;...
                2*time];
            
            costStateJac = [y(2)*y(3)*u(1)*u(2)*time ...
                y(1)*y(3)*u(1)*u(2)*time ...
                y(1)*y(2)*u(1)*u(2)*time];
            
            costControlJac = [y(1)*y(2)*y(3)*u(2)*time ...
                y(1)*y(2)*y(3)*u(1)*time];
            
            costTimeJac = y(1)*y(2)*y(3)*u(1)*u(2);
            
            %  Evaluate Algebraic Function
            fac = pi;
            if useAnalticPartials
                obj.SetAlgStateJac(fac*funcStateJac);
                obj.SetAlgControlJac(fac*funcControlJac);
                obj.SetAlgTimeJac(fac*funcTimeJac);
            end
            
            %  Evaluate Cost Function
            if useAnalticPartials
                obj.SetCostStateJac(costStateJac);
                obj.SetCostControlJac(costControlJac);
                obj.SetCostTimeJac(costTimeJac);
            end
            
            % Evaluate dnamics and compute Jacobians
            if useAnalticPartials
                obj.SetDynStateJac(funcStateJac);
                obj.SetDynControlJac(funcControlJac);
                obj.SetDynTimeJac(funcTimeJac);
            end
            
        end
    end
end
