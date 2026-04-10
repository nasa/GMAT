classdef AutoNGCPathObject < userfunutils.OrbitPathFunc
    % Path functions for the AutoNGC low-thrust orbit transfer problem
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            % Compute and set the dynamics
            obj.SetDynamics();
            
            % Compute and set the control path constraint
            obj.SetControlPathConstraint();
            
        end
        
        function EvaluateJacobians(~)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
    end
    
end



