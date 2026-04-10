classdef BridgeCranePathObject < userfunutils.UserPathFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

	properties
    
	end

	methods
	
		function EvaluateFunctions(obj)
		
            %  Get current state and control
			stateVec = obj.GetStateVec();
			control = obj.GetControlVec();
            x2 = stateVec(2);
            x3 = stateVec(3);
            x4 = stateVec(4);
			u = control(1);
						
			obj.SetDynFunctions([x2 u x4 (-0.98*x3 + 0.1*u)]')
	
		end
		
		
		function EvaluateJacobians(obj)
												
		end
		
	end

end