classdef GoddardRocket_TwoPhasePointObject < userfunutils.UserPointFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

	properties
    
	end

	methods
	
		function EvaluateFunctions(obj)
			
			stateVec1f = obj.GetFinalStateVec(1);
			stateVec2i = obj.GetInitialStateVec(2);
			stateVec2f = obj.GetFinalStateVec(2);
						
			obj.SetCostFunction(-stateVec2f(1));
			
			t_interface_error = obj.GetFinalTime(1) - obj.GetInitialTime(2);
			h_interface_error = stateVec1f(1) - stateVec2i(1);
			v_interface_error = stateVec1f(2) - stateVec2i(2);
			m_interface_error = stateVec1f(3) - stateVec2i(3);
			
			obj.SetAlgFunctions([obj.GetInitialTime(1);t_interface_error;obj.GetFinalTime(2);obj.GetInitialStateVec(1);h_interface_error;v_interface_error;m_interface_error;obj.GetFinalStateVec(2)]);
						
			%                          t1,0,  t_interface_error, t2,f, h1,0, v1,0, m1,0, h_interface_error, v_interface_error, m_interface_error, h2,f,  v2,f,  m2,f
			obj.SetAlgFuncLowerBounds([   0;                  0;   .1;    1;    0;    1;                 0;                 0;                 0;    1;     0;    .6]);
			obj.SetAlgFuncUpperBounds([   0;                  0;    1;    1;    0;    1;                 0;                 0;                 0;  1e5;     0;    .6]);
		
		end
        function EvaluateJacobians(~)

        end
	end

end