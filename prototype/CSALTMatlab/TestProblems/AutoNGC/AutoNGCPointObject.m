classdef AutoNGCPointObject < userfunutils.OrbitPointFunc
    % Boundary functions for the AutoNGC low-thrust orbit transfer problem
    
    properties
        
        % Initial state in non-dimensional modified equinoctial elements,
        % with mass at the end
        initModEqElemNonDim
        % Final Cartesian state in EME2000 KM, KM/S
        finalCartState

    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Evaluate boundary functions
                       
            %  Extract the initial and final time and state for
            %  phase 1, (the only phase in the problem)
            initState = obj.GetInitialStateVec(1);
            initTime = obj.GetInitialTime(1);
            finalState = obj.GetFinalStateVec(3);
            
            % Compute the boundary constraints
            initConditionConstraint = initState - obj.initModEqElemNonDim';
            
            % Convert final achieved state to Cartesian and compute error
            %finalState = TU = obj.phaseList{1}.orbitScaleUtil.finalState;
            finalState = obj.phaseList{1}.orbitScaleUtil.UnScaleModEqFullState(finalState);
            finalCart = real(Mee2Cart(finalState(1:6),398600.4415));
            [finalStateError,lowBoundFinalState,uppBoundFinalState] = ...
                obj.ComputeFinalBoundaryConstraint(finalCart);
            
            % Compute the linkage constraints
            % TODO: Linkage constraints will eventually be moved to the 
            % OrbitPointFunc class. 
            stateError12 = (obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1));
            stateError23 = (obj.GetInitialStateVec(3) - obj.GetFinalStateVec(2));
            timeError12 = (obj.GetInitialTime(2) - obj.GetFinalTime(1));
            timeError23 = (obj.GetInitialTime(3) - obj.GetFinalTime(2));
            
            % Compute delta Time constraints
            dt1 = obj.GetFinalTime(1)-obj.GetInitialTime(1);
            dt3 = obj.GetFinalTime(3)-obj.GetInitialTime(3);
            
            % Set the boundary constraints
            obj.SetAlgFunctions([initConditionConstraint;initTime;finalStateError;...
                stateError12;stateError23;timeError12;timeError23;dt1;dt3]);
            
            % If initializing, set the bounds using the same order
            % as in the call to SetAlgFunctions()
            if obj.IsInitializing
                TU = obj.phaseList{1}.orbitScaleUtil.TU;
                obj.SetAlgFuncLowerBounds([zeros(7,1);0;lowBoundFinalState;...
                    zeros(8,1);zeros(8,1);.1/TU;.1/TU]);
                obj.SetAlgFuncUpperBounds([zeros(7,1);0;uppBoundFinalState;...
                    zeros(8,1);zeros(8,1);1000*TU;1000*TU]);
            end
            
            % Set the cost function
            finalState = obj.GetFinalStateVec(3);
            obj.SetCostFunction(-finalState(7));
            
        end
        
        function EvaluateJacobians(~)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
        function SetInitialConditions(obj,initModEqElemNonDim)
            % Sets the initial conditions
            obj.initModEqElemNonDim = initModEqElemNonDim;
        end
        
        function SetFinalCartState(obj,finalCartState)
            % Sets the final state in EME 2000, km km/s
            obj.finalCartState = finalCartState;
        end
                        
        function [finalError,lowerBound,upperBound] = ComputeFinalBoundaryConstraint(obj,finalCart)
            
            [finalMee] = Cart2Mee(finalCart,398600.4415); 
            [finalMeeState] = Cart2Mee(obj.finalCartState,398600.4415); 
            finalError = (finalMee(1:5,1) - finalMeeState(1:5));
            DU = obj.phaseList{3}.orbitScaleUtil.DU;
            finalError(1,1) = finalError(1,1)/DU;
            lowerBound = zeros(size(finalError,1),1); 
            upperBound = lowerBound; 

        end
    end
    
end



