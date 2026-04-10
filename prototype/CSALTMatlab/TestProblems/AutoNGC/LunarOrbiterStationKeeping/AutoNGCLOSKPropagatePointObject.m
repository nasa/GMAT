classdef AutoNGCLOSKPropagatePointObject < userfunutils.OrbitPointFunc
    % Boundary functions for the AutoNGC lunar orbit station keeping problem
    % just a propagator: fix initial time, final time, and initial state
    
    properties
        
        % Initial state in non-dimensional modified equinoctial elements,
        % with mass at the end
        initModEqElemNonDim
        finalTimeNonDim = 100

    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Evaluate boundary functions
                       
            %  Extract the initial and final time and state
            initState = obj.GetInitialStateVec(1);
            initTime = obj.GetInitialTime(1);
            finalState = obj.GetFinalStateVec(1);
            finalTime = obj.GetFinalTime(1);
            
            % Compute the boundary constraints
            initConditionConstraint = initState - obj.initModEqElemNonDim'; % initial state fully fixed
            finalTimeConstraint = finalTime - obj.finalTimeNonDim; % final time fixed
                      
            % Set the boundary constraints
            obj.SetAlgFunctions([initConditionConstraint;
                initTime;
                finalTimeConstraint]);
            
            % If initializing, set the bounds using the same order
            % as in the call to SetAlgFunctions()
            if obj.IsInitializing
                obj.SetAlgFuncLowerBounds([zeros(7,1);
                    0;
                    0]);
                obj.SetAlgFuncUpperBounds([zeros(7,1);
                    0;
                    0]);
            end
            
            % Set the cost function: nothing
            obj.SetCostFunction(0);
            
        end
        
        function EvaluateJacobians(~)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
        function SetInitialConditions(obj,initModEqElemNonDim)
            % Sets the initial conditions (scaled)
            obj.initModEqElemNonDim = initModEqElemNonDim;
        end
        
        function SetFinalTime(obj,finalTimeNonDim)
            % Sets the final conditions (scaled)
            obj.finalTimeNonDim = finalTimeNonDim;
        end
                        
    end
    
end



