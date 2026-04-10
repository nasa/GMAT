classdef AutoNGCLOSKPointObject < userfunutils.OrbitPointFunc
    % Boundary functions for the AutoNGC lunar orbit station keeping problem
    
    properties
        
        % Initial state in non-dimensional modified equinoctial elements,
        % with mass at the end
        initModEqElemNonDim
        
        % Final Cartesian state in lunar equatorial inertial KM, KM/S
        finalCartState
        
        % Final state in non-dimensional modified equinoctial elements,
        % with mass at the end
        finalModEqElemNonDim

    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Evaluate boundary functions
                       
            %  Extract the initial and final time and state
            initState = obj.GetInitialStateVec(1);
            initTime = obj.GetInitialTime(1);
            finalState = obj.GetFinalStateVec(1);
            
            % Compute the boundary constraints
            initConditionConstraint = initState - obj.initModEqElemNonDim'; % initial state fully fixed
            finalConditionConstraint = finalState(1:5) - obj.finalModEqElemNonDim(1:5)'; % not constraining final anomaly or final mass
                      
            % Set the boundary constraints
            obj.SetAlgFunctions([initConditionConstraint;
                initTime;
                finalConditionConstraint]);
            
            % If initializing, set the bounds using the same order
            % as in the call to SetAlgFunctions()
            if obj.IsInitializing
                obj.SetAlgFuncLowerBounds([zeros(7,1);
                    0;
                    zeros(5,1)]);
                obj.SetAlgFuncUpperBounds([zeros(7,1);
                    0;
                    zeros(5,1)]);
            end
            
            % Set the cost function: maximize final mass
            obj.SetCostFunction(-finalState(7));
            %disp([initConditionConstraint; initTime; finalConditionConstraint; finalState(7)])
            %disp(obj.GetFinalTime(1))
            
        end
        
        function EvaluateJacobians(~)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
        function SetInitialConditions(obj,initModEqElemNonDim)
            % Sets the initial conditions (scaled)
            obj.initModEqElemNonDim = initModEqElemNonDim;
        end
        
        function SetFinalConditions(obj,finalModEqElemNonDim)
            % Sets the final conditions (scaled)
            obj.finalModEqElemNonDim = finalModEqElemNonDim;
        end
                        
    end
    
end



