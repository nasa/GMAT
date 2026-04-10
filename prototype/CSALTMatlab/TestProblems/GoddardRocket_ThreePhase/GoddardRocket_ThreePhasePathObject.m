classdef GoddardRocket_ThreePhasePathObject < userfunutils.UserPathFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

    properties
		unitsType
        D0
        B
        c
    end

	methods
	
		function InitializeConstants(obj)
            switch obj.unitsType
                case 'gpops'
					obj.D0 = 5.49153484923381010 * 10^(-5);
					obj.B = 1/23800;
					obj.c = sqrt(3.264*32.174*23800);
                case 'psopt'
                    obj.D0 = 310.0;
                    obj.B = 1/500.0;
                    obj.c = .5;
            end
	    end
		
		function SetUnitsType(obj,unitsInput)
			obj.unitsType = unitsInput;
		end
		
		function [g,dg_dh] = GetGravity(obj,h) 
            switch obj.unitsType
                case 'gpops'
					g = 32.174;
					dg_dh = 0;
                case 'psopt'
                    g = (1/(h*h));
					dg_dh = 2/(h*h*h);
            end
		end
	
		function EvaluateFunctions(obj)
			
			stateVec = obj.GetStateVec();
			controlVec = obj.GetControlVec();
			
			h = stateVec(1);
			v = stateVec(2);
			m = stateVec(3);
			T = controlVec(1);
			
			D = obj.D0 * v * v * exp(-obj.B * h);
			[g,dg_dh] = obj.GetGravity(h);
			
			hdot = v;
			vdot = 1/m * (T - D) - g;
			mdot = -T/obj.c;
			
			obj.SetDynFunctions([hdot;vdot;mdot])
			
			if obj.GetPhaseNumber() == 2
				% See Betts 2001 (referenced in the gpops manual) for the correct phase 2 constraint
				voverc = v/obj.c;
				xmg = m * g;
				term1 = (obj.c^2).*(1+voverc)/(g/obj.B)-1.0-2/voverc;
				term2 = xmg/(1+4/voverc+2/(voverc^2));
				path_constraint = T-D-xmg-term1*term2;
				obj.SetAlgFunctions([path_constraint]);
				obj.SetAlgFuncUpperBounds([0]);
				obj.SetAlgFuncLowerBounds([0]);
			end
			
		end
		
		
		function EvaluateJacobians(obj)
			
			stateVec = obj.GetStateVec();
			controlVec = obj.GetControlVec();
			
			h = stateVec(1);
			v = stateVec(2);
			m = stateVec(3);
			T = controlVec(1);
			
			D = obj.D0 * v * v * exp(-obj.B * h);
			[g,dg_dh] = obj.GetGravity(h);
			
			dhdot_dh = 0;
			dhdot_dv = 1;
			dhdot_dm = 0;
			dvdot_dh = 1 / m * (obj.D0 * v * v * obj.B * exp(-obj.B * h)) + dg_dh;
			dvdot_dv = 1 / m * (-2 * obj.D0 * v * exp(-obj.B * h));
			dvdot_dm = - 1 / (m * m) * (T - obj.D0 * v * v * exp(-obj.B * h));
			dmdot_dh = 0;
			dmdot_dv = 0;
			dmdot_dm = 0;
			
			dhdot_dT = 0;
			dvdot_dT = 1 / m;
			dmdot_dT = - 1 / obj.c;
			
			obj.SetDynStateJac([dhdot_dh,dhdot_dv,dhdot_dm; ...
			                    dvdot_dh,dvdot_dv,dvdot_dm; ...
			                    dmdot_dh,dmdot_dv,dmdot_dm])
								
			obj.SetDynControlJac([dhdot_dT;dvdot_dT;dmdot_dT]);
			
			obj.SetDynTimeJac([0;0;0]);
		end
		
	end

end