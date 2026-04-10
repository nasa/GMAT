classdef GoddardRocket_TwoPhasePathObject < userfunutils.UserPathFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

	properties
    
	end

	methods
	
		function EvaluateFunctions(obj)
			D0 = 310.0;
			B = 500.0;
			c = .5;
			
			stateVec = obj.GetStateVec();
			controlVec = obj.GetControlVec();
			
			h = stateVec(1);
			v = stateVec(2);
			m = stateVec(3);
			T = controlVec(1);
			
			D = D0 * v * v * exp(-B * h);
			g = 1 / (h * h);
			
			hdot = v;
			vdot = 1/m * (T - D) - g;
			mdot = -T/c;
			
			obj.SetDynFunctions([hdot;vdot;mdot])
			
		end
		
		
		function EvaluateJacobians(obj)
			D0 = 310.0;
			B = 500.0;
			c = .5;
			
			stateVec = obj.GetStateVec();
			controlVec = obj.GetControlVec();
			
			h = stateVec(1);
			v = stateVec(2);
			m = stateVec(3);
			T = controlVec(1);
			
			D = D0 * v * v * exp(-B * h);
			g = 1 / (h * h);
			
			dhdot_dh = 0;
			dhdot_dv = 1;
			dhdot_dm = 0;
			dvdot_dh = 1 / m * (D0 * v * v * B * exp(-B * h)) + 2 / (h * h * h);
			dvdot_dv = 1 / m * (-2 * D0 * v * exp(-B * h));
			dvdot_dm = - 1 / (m * m) * (T - D0 * v * v * exp(-B * h));
			dmdot_dh = 0;
			dmdot_dv = 0;
			dmdot_dm = 0;
			
			dhdot_dT = 0;
			dvdot_dT = 1 / m;
			dmdot_dT = - 1 / c;
			
			obj.SetDynStateJac([dhdot_dh,dhdot_dv,dhdot_dm; ...
			                    dvdot_dh,dvdot_dv,dvdot_dm; ...
			                    dmdot_dh,dmdot_dv,dmdot_dm])
								
			obj.SetDynControlJac([dhdot_dT;dvdot_dT;dmdot_dT]);
			
			obj.SetDynTimeJac([0;0;0]);
		end
		
	end

end