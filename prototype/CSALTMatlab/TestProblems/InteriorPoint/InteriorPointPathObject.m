classdef InteriorPointPathObject < userfunutils.UserPathFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

	properties
    
	end

	methods
	
		function EvaluateFunctions(obj)
			
			stateVec = obj.GetStateVec();
			controlVec = obj.GetControlVec();
			
			x = stateVec(1);
			u = controlVec(1);
			
			xdot = controlVec;
			
			obj.SetDynFunctions([xdot])
			
			obj.SetCostFunction([x*x+u*u])
			
		end
		
		
		function EvaluateJacobians(obj)
												
			dxdot_dx = 0;
			
			dxdot_du = 1;
			
			obj.SetDynStateJac([dxdot_dx]);
								
			obj.SetDynControlJac([dxdot_du]);
			
			obj.SetDynTimeJac([0]);
		end
		
	end

end