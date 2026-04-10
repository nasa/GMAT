classdef InteriorPointPointObject < userfunutils.UserPointFunction
	%UNTITLED Summary of this class goes here
	%   Detailed explanation goes here

	properties
    
	end

	methods
	
		function EvaluateFunctions(obj)
			
			%stateVec = obj.GetFinalStateVec(1);
			
			%obj.SetCostFunction(1.0);
			
			obj.SetAlgFunctions([obj.GetInitialTime(1);obj.GetFinalTime(1);obj.GetInitialTime(2);obj.GetFinalTime(2);...
			                     obj.GetInitialStateVec(1);obj.GetFinalStateVec(1);obj.GetInitialStateVec(2);obj.GetFinalStateVec(2)]);
			
			%                         t1,0, t1,f, t1,0, t1,f, x1,0, x1,f, x2,0, x2,f
			obj.SetAlgFuncLowerBounds([ 0;   .75;  .75;    1;    1;   .9;  .9;  .75]);
			obj.SetAlgFuncUpperBounds([ 0;   .75;  .75;    1;    1;   .9;  .9;  .75]);
		
		end
        function EvaluateJacobians(~)

        end
	end

end