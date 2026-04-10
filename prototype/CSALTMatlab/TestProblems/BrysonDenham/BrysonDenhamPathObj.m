classdef BrysonDenhamPathObj < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            
            x2 = stateVec(2);
            u = controlVec(1);
            
            obj.SetDynFunctions([x2;
                u;
                0.5*u*u]);
            
        end
        
        function EvaluateJacobians(obj)
            
             controlVec = obj.GetControlVec();
             u = controlVec(1);

            obj.SetDynStateJac([0 1 0;0 0 0;0 0 0]);
            obj.SetDynControlJac([0;1;u]);
            obj.SetDynTimeJac([0;0;0]);
            
        end
    end
    
end



