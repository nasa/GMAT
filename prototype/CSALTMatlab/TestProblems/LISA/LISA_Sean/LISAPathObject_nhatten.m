classdef LISAPathObject_nhatten < userfunutils.UserPathFunction
    % Path functions for the LISA low-thrust orbit transfer problem
    
    properties
                        
        % Structure containing scale values
        scaleUtil 
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Get the state
            % for 2-body propagation, EOMs are autonomous, so time doesn't
            % matter, and there is no control, so that doesn't matter,
            % either
            stateVec = obj.GetStateVec();
            
            % there are 3 spacecraft, each described by 6 states
            nSpacecraft = 3;
            statesPerBody = 6;
            
            % do all in one for loop
            dynVec = zeros(size(stateVec));
            for i = 1:nSpacecraft
                stateOneBody = stateVec(1 + (i-1)*statesPerBody:i*statesPerBody); % get the state for just body i
                stateOneBodyUnscaled = obj.scaleUtil.earthScaling.StateUnscaleMat * stateOneBody; % unscale the state
                dynOneBodyUnscaled = obj.EOM2body(stateOneBodyUnscaled, obj.scaleUtil.earthScaling.gravParam); % get the dynamics for body i
                dynOneBody = obj.scaleUtil.earthScaling.DynScaleMat * dynOneBodyUnscaled; % scale the dynamics for body i
                dynVec(1 + (i-1)*statesPerBody:i*statesPerBody) = dynOneBody; % put dynamics for body i into dynamics array
            end
            
            %  Set the dynamics functions
            obj.SetDynFunctions(dynVec);
            
        end
        
        function EvaluateJacobians(obj)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
        function SetScaleUtil(obj,scaleUtil)
            
            % Set structure containing scale factors for this problem
            obj.scaleUtil = scaleUtil;
            
        end
        
        function [dx] = EOM2body(obj, x, gravParam)
            % given Cartesian state and gravitational parameter, return time
            % derivative of state, assuming 2-body motion
            rMag = norm(x(1:3));
            dv = -gravParam / rMag^3 * x(1:3);
            dx = [x(4:6); dv];
        end
    end
    
end



