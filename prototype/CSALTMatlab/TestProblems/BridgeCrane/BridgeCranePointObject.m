classdef BridgeCranePointObject < userfunutils.UserPointFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

	properties
    
	end

	methods
	
		function EvaluateFunctions(obj)
					
			obj.SetAlgFunctions([obj.GetInitialTime(1);...
                obj.GetInitialStateVec(1);obj.GetFinalStateVec(1)]);

			obj.SetAlgFuncLowerBounds([ 0; 0; 0; 0; 0; 0; 0; 0; 0]);
			obj.SetAlgFuncUpperBounds([ 0; 0; 0; 0; 0; 15; 0; 0; 0])
            
            obj.SetCostFunction(obj.GetFinalTime(1))
        end
        
        function EvaluateJacobians(~)

        end
	end

end