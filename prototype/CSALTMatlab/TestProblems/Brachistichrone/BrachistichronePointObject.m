classdef BrachistichronePointObject < userfunutils.UserPointFunction
    % Boundary functions for Brachistichrone problem.
    
    methods
        
        function EvaluateFunctions(obj)
           
            %  Extract the intial and final time and state for
            %  phase 1, (the only phase in the problem)
            initTime = obj.GetInitialTime(1);
            finalTime = obj.GetFinalTime(1);
            initState = obj.GetInitialStateVec(1);
            finalState = obj.GetFinalStateVec(1);
            
            % Set the cost function
            obj.SetCostFunction(finalTime);
            
            % Set the boundary functions
            obj.SetAlgFunctions([initTime;
                finalTime;...
                initState;
                finalState]);
            
            % If initializing, set the bounds using the same order
            % as in the call to SetAlgFunctions()
            if obj.IsInitializing
                obj.SetAlgFuncLowerBounds([0 0   0 0  0  1 -10 -10]');
                obj.SetAlgFuncUpperBounds([0 100 0 0  0  1 10 0 ]');
            end
            
        end
        
        function EvaluateJacobians(~)
            
        end
    end
    
end

