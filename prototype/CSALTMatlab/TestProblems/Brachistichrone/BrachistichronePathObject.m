classdef BrachistichronePathObject < userfunutils.UserPathFunction
    % Path functions for Brachistichrone problem. 
    
    properties
        gravity = -32.174
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Get the state and control
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            v = stateVec(3);
            u = controlVec(1);
            
            %  Set the dynamics functions
            obj.SetDynFunctions([v*sin(u);
                v*cos(u);
                obj.gravity*cos(u)]);
            
        end
        
        function EvaluateJacobians(obj)
                        
            %  Get the state and control
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            v = stateVec(3);
            u = controlVec(1);
            
            %  Set the Jacobians of the dynamics functions
            obj.SetDynStateJac([0 0 sin(u);
                0 0 cos(u);
                0 0 0]);
            
            obj.SetDynControlJac([v*cos(u);
                -v*sin(u);
                -obj.gravity*sin(u)]);
            
            obj.SetDynTimeJac([0;0;0]);
            
        end
    end
    
end



