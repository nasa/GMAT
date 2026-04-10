classdef LinearTangentSteeringPathObj < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            
            %x1 = obj.stateVec(1);
            x2 = stateVec(2);
            %x3 = obj.stateVec(3);
            x4 = stateVec(4);
            u = controlVec(1);
            
            a = 100;
            obj.SetDynFunctions([x2;
                a*cos(u);
                x4;
                a*sin(u)]);
            
        end
        
        function EvaluateJacobians(obj)
            
            controlVec = obj.GetControlVec();
            u = controlVec(1);
            a = 100;
            obj.SetDynStateJac([
                0 1 0 0;
                0 0 0 0;
                0 0 0 1;
                0 0 0 0]);
            
            obj.SetDynControlJac([0;
                -a*sin(u);
                0;
                a*cos(u)]);
            
            obj.SetDynTimeJac([0 0 0 0]');
            
        end
    end
    
end



