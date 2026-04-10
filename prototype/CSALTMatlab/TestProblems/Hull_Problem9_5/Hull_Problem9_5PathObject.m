classdef Hull_Problem9_5PathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        gravity = -32.174
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Get the state and control
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            x = stateVec(1);
            u = controlVec(1);
            
            %  Set the dynamics functions
            obj.SetDynFunctions(u);
            obj.SetCostFunction(u^2 - x);
        end
        
        function EvaluateJacobians(obj)
            
            obj.SetDynStateJac(0);
            obj.SetDynControlJac(1);
            u = obj.GetControlVec();
            obj.SetCostStateJac(-1)
            obj.SetCostControlJac(2*u)
                                    
        end
    end
    
end



