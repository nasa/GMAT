classdef BreakwellPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
                       
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            time = obj.GetTime();
            
            x = stateVec(1);
            v = stateVec(2);
            u = controlVec(1);
            
            obj.SetCostFunction(0.5*u*u);
            obj.SetDynFunctions([v;u]);
            
            obj.SetAlgFunctions(x);
            obj.SetAlgFuncUpperBounds(0.1);
            obj.SetAlgFuncLowerBounds(-10);
            
        end
        
        function EvaluateJacobians(obj)
            
            controlVec = obj.GetControlVec();
            u = controlVec(1);
            
            obj.SetCostStateJac([0 0]);
            obj.SetCostControlJac(u)
            obj.SetCostTimeJac(0)
            
            obj.SetAlgStateJac([1 0]);
            obj.SetAlgControlJac(0);
            obj.SetAlgTimeJac(0);
            
            obj.SetDynStateJac([0 1;0 0]);
            obj.SetDynControlJac([0;1]);
            obj.SetDynTimeJac([0;0]);
            
        end
    end
    
end



