classdef RauAutomaticaPathObj < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            y = stateVec(1);
            u = controlVec(1);
            
            obj.SetDynFunctions(5/2*(-y +y*u -u^2));

        end
        
        function EvaluateJacobians(obj)
            
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            u = controlVec(1);
            y = stateVec(1);
            obj.SetDynStateJac(5/2*(-1 + u));
            obj.SetDynControlJac(5/2*(y-2*u));
            obj.SetDynTimeJac(0);

        end
    end
    
end



