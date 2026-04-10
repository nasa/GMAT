classdef DummyPathFunction2 < userfunutils.UserPathFunction
    %DummyPathFunction2 This is a made up user function used in testing
    %sparsity pattern determination of UserFunctionPathManager.
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            % Extract paramater data
            y = obj.GetStateVec();
            u = obj.GetControlVec();
            time = obj.GetTime();
            
            %  Compute functions for phase 2
            if obj.GetPhaseNumber() == 2
                
                % Test Functions used for dnamics and algebraic funcs
                func = [-y(1)^2*y(3) + u(1)^2*u(2)^3 ;
                    y(2)*y(1)^3 + u(2)^2 + time^2];
                
                %  Test function used for cost
                cost = y(1)*y(3)*u(1)*time;
                
                func2 = [-y(2)^2*y(3) + u(2)^2*u(2)^3 + time^2 ;
                    y(3)*y(1)^3 + u(1)^2 ];
                
                %  Evaluate Algegraic Function
                obj.SetAlgFunctions(func2);
                
                %  Evaluate Cost Function
                obj.SetCostFunction(cost);
                
                % Evaluate dnamics and compute Jacobians
                obj.SetDynFunctions(func);
                
            end
        end
        
        function EvaluateJacobians(obj)
            
            % Extract paramater data
            y = obj.GetStateVec();
            u = obj.GetControlVec();
            time = obj.GetTime();
            
            useAnaltyicJacs = 1;
            
            if  useAnaltyicJacs
                
                func = [-y(1)^2*y(3) + u(1)^2*u(2)^3 ;
                    y(2)*y(1)^3 + u(2)^2 + time^2];
                
                %  State Jacobians
                obj.SetDynStateJac([-2*y(1)*y(3), 0, -y(1)^2;
                                    3*y(2)*y(1)^2, y(1)^3, 0]);               
                obj.SetCostStateJac([ y(3)*u(1)*time,0, y(1)*u(1)*time] )
                obj.SetAlgStateJac([ 0 , -2*y(2)*y(3), -y(2)^2;
                    3*y(3)*y(1)^2, 0 ,y(1)^3]);
                    
                
            end
            
            
        end
    end
    
end