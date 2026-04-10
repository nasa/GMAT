classdef GoddardRocket_ThreePhasePointObject < userfunutils.UserPointFunction
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
					obj.D0 = 5.49153484923381010* 10^(-5);
					obj.B = 1/23800;
					obj.c = 1580.9425279876559;%sqrt(3.264*32.174*23800);
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
			
			stateVec1f = obj.GetFinalStateVec(1);
			stateVec2i = obj.GetInitialStateVec(2);
			stateVec2f = obj.GetFinalStateVec(2);
			stateVec3i = obj.GetInitialStateVec(3);
			stateVec3f = obj.GetFinalStateVec(3);
						
			obj.SetCostFunction(-stateVec3f(1));
			
			t_interface_1_2_error = obj.GetFinalTime(1) - obj.GetInitialTime(2);
			h_interface_1_2_error = stateVec1f(1) - stateVec2i(1);
			v_interface_1_2_error = stateVec1f(2) - stateVec2i(2);
			m_interface_1_2_error = stateVec1f(3) - stateVec2i(3);
			
			t_interface_2_3_error = obj.GetFinalTime(2) - obj.GetInitialTime(3);
			h_interface_2_3_error = stateVec2f(1) - stateVec3i(1);
			v_interface_2_3_error = stateVec2f(2) - stateVec3i(2);
			m_interface_2_3_error = stateVec2f(3) - stateVec3i(3);

			h2f = stateVec2f(1);
			v2f = stateVec2f(2);
			m2f = stateVec2f(3);
			
			[g2f,dg2f_dh2f] = obj.GetGravity(h2f);

			% See Betts 2001 (referenced in the gpops manual) for the correct phase 2 point constraint
			D2f = obj.D0 * v2f * v2f * exp(-obj.B * h2f);
			point_constraint = m2f * g2f - (1 + v2f / obj.c) * D2f;
						
			obj.SetAlgFunctions([obj.GetInitialTime(1);t_interface_1_2_error;t_interface_2_3_error;obj.GetFinalTime(3);obj.GetInitialStateVec(1);h_interface_1_2_error;v_interface_1_2_error;m_interface_1_2_error;h_interface_2_3_error;v_interface_2_3_error;m_interface_2_3_error;obj.GetFinalStateVec(3);point_constraint]);
			
			%                                  t1,0,  t_interface_1_2_error,  t_interface_2_3_error, t3,f, h1,0, v1,0, m1,0, h_interface_1_2_error, v_interface_1_2_error, m_interface_1_2_error, h_interface_2_3_error, v_interface_2_3_error, m_interface_2_3_error, h3,f,  v3,f,  m3,f,  point_constraint 
			switch obj.unitsType
				case 'psopt'
					obj.SetAlgFuncLowerBounds([   0;                      0;                      0;   .1;    1;    0;    1;                     0;                     0;                     0;                     0;                     0;                     0;    1;     0;    .6;  0]);
					obj.SetAlgFuncUpperBounds([   0;                      0;                      0;    1;    1;    0;    1;                     0;                     0;                     0;                     0;                     0;                     0;  1e5;     0;    .6;  0]);
				case 'gpops'
					obj.SetAlgFuncLowerBounds([   0;                      0;                      0;   .1;    0;    0;    3;                     0;                     0;                     0;                     0;                     0;                     0;    1;     0;     1;  0]);
					obj.SetAlgFuncUpperBounds([   0;                      0;                      0; 1e10;    0;    0;    3;                     0;                     0;                     0;                     0;                     0;                     0; 1e10;     0;     1;  0]);
			end
		end
		
        function EvaluateJacobians(~)

        end
	end

end