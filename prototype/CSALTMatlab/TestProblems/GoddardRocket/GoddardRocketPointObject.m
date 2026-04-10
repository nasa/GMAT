classdef GoddardRocketPointObject < userfunutils.UserPointFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

    properties
		unitsType
    end

	methods
	
		function SetUnitsType(obj,unitsInput)
			obj.unitsType = unitsInput;
		end
	
		function EvaluateFunctions(obj)
			
			stateVec1f = obj.GetFinalStateVec(1);
						
			obj.SetCostFunction(-stateVec1f(1));
						
			obj.SetAlgFunctions([obj.GetInitialTime(1);obj.GetFinalTime(1);obj.GetInitialStateVec(1);stateVec1f]);
			
			%                                  t1,0,  t1,f, h1,0, v1,0, m1,0, h1,f,  v1,f,  m1,f  
			switch obj.unitsType
				case 'psopt'
					obj.SetAlgFuncLowerBounds([   0;   .1;     1;    0;    1;    1;     0;    .6]);
					obj.SetAlgFuncUpperBounds([   0;    1;     1;    0;    1;  1e5;     0;    .6]);
				case 'gpops'
					obj.SetAlgFuncLowerBounds([   0;   .1;     0;    0;    3;    1;     0;     1]);
					obj.SetAlgFuncUpperBounds([   0; 1e10;     0;    0;    3; 1e10;     0;     1]);
			end
		end
		
        function EvaluateJacobians(~)

        end
	end

end