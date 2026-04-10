classdef LISAPointObject_RmagConstraint_nhatten < userfunutils.UserPointFunction
    % Boundary functions for the LISA low-thrust orbit transfer problem
    
    properties
        
        % Structure containing scale values
        scaleUtil
       
        % Initial state in unscaled cartesian elements (km, km/s)
        initCartStateConstraintUnscaled
        initCartStateConstraint % scaled version
        
        % initial time in MJDA1
        initTimeConstraintUnscaled
        initTimeConstraint % scaled version
        
        % final time in MJDA1
        finalTimeConstraintUnscaled
        finalTimeConstraint % scaled version
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Evaluate boundary functions
            
            %  Extract the intial and final time and state for
            %  phase 1, (the only phase in the problem)
            % these are all the scaled values
            initState = obj.GetInitialStateVec(1);
            initTime = obj.GetInitialTime(1);
            finalState = obj.GetFinalStateVec(1); % not used
            finalTime = obj.GetFinalTime(1);
            
            % Compute the boundary constraints (scaled)
            % constraint values are already scaled, so don't need to
            % unscale the state/time
            initCartStateConstraintValue = initState - obj.initCartStateConstraint;
            initTimeConstraintValue = initTime - obj.initTimeConstraint;
            finalTimeConstraintValue = finalTime - obj.finalTimeConstraint;
            
            % just want constraints on the initial and final rmag
            rmagConInit = norm(initState(1:3));
            rmagConFinal = norm(finalState(1:3));
            
            % Set the boundary constraints
            obj.SetAlgFunctions([rmagConInit;
                rmagConFinal;
                initTimeConstraintValue; 
                finalTimeConstraintValue]);
            
            % If initializing, set the bounds using the same order
            % as in the call to SetAlgFunctions()
            if obj.IsInitializing
                obj.SetAlgFuncLowerBounds([3;
                    3;
                    0;
                    0]);
                obj.SetAlgFuncUpperBounds([4;
                    4;
                    0;
                    0]);
            end
            
            % Set the cost function
            obj.SetCostFunction(0);
            
        end
        
        function EvaluateJacobians(obj)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
        function SetInitState(obj,initCartState)
            % Sets initial scaled cartesian state
            obj.initCartStateConstraint = initCartState;
        end
        
        
        function SetInitTime(obj,initTime)
            % Set initial scaled time
            obj.initTimeConstraint = initTime;
        end
        
        function SetFinalTime(obj,finalTime)
            % Set final scaled time
            obj.finalTimeConstraint = finalTime;
        end
        
        function SetScaleUtil(obj,scaleUtil)
            % Sets the structure containing scale data
            obj.scaleUtil = scaleUtil;
        end
    end
    
end



