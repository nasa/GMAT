classdef OrbitPathFunction < userfunutils.UserPathFunction

	properties
	
		time 
		stateVec
		controlVec
		ifSparsity
		Xdot
		
		UpperBounds
		LowerBounds
		Functions
		Names
		
	end

	methods

		function InitializeBoundsFunctionsAndNames(obj)
			if obj.GetIsInitializing()
				obj.UpperBounds = [];
				obj.LowerBounds = [];
				obj.Names = {};
				obj.Functions = [];
			else
				obj.Functions = [];
			end

			obj.Xdot = zeros(7,1);
		end
		
		function AddUpperBounds(obj,Input)
			if obj.GetIsInitializing()
				obj.UpperBounds = [obj.UpperBounds;Input];
			end
		end
		
		function AddLowerBounds(obj,Input)
			if obj.GetIsInitializing()
				obj.LowerBounds = [obj.LowerBounds;Input];
			end	
		end
		
		function AddNames(obj,Input)
			if obj.GetIsInitializing()
				obj.Names = [obj.UpperBounds;Input];
			end
		end
		
		function AddFunctions(obj,Input)
			obj.Functions = [obj.Functions;Input];
		end

		function FinalizeBoundsFunctionsAndNames(obj)
			
			if obj.GetIsInitializing()
				obj.SetAlgFuncLowerBounds(obj.LowerBounds);
				obj.SetAlgFuncUpperBounds(obj.UpperBounds);
				obj.SetAlgFuncNames(obj.Names);
			end
			obj.SetAlgFunctions(obj.Functions);
			
		end
	
		function ComputeSphericalControlOrbitalDynamics(obj,SEPForces)
			obj.InitializeBoundsFunctionsAndNames();
		
			if obj.GetIsInitializing()
				obj.UpdateSphericalControlAlgFunctionBounds();
			end
		
			obj.UpdateStateControlandTime();
			obj.UnScaleStateandTime();
			obj.UpdateIfSparsity();
			obj.CalculateAccelerations_SphericalControl(SEPForces);
			obj.ScaleAccelerations();
			obj.SetDynFunctions(obj.Xdot)
			obj.FinalizeBoundsFunctionsAndNames();
		end
	
		function ComputeCartesianControlOrbitalDynamics(obj,SEPForces)

			obj.InitializeBoundsFunctionsAndNames();
			
			if obj.GetIsInitializing()
				obj.UpdateCartesianControlAlgFunctionBounds();
			end
		
			obj.UpdateStateControlandTime();
			obj.UnScaleStateandTime();
			obj.UpdateIfSparsity();
			obj.CalculateAccelerations_CartesianControl(SEPForces);
			obj.UpdateCartesianControlAlgFunctions();
			obj.ScaleAccelerations();
			obj.SetDynFunctions(obj.Xdot)
			obj.FinalizeBoundsFunctionsAndNames();
		end
  	
		function UpdateCartesianControlAlgFunctionBounds(obj)
			
			if obj.GetNumControlVars()
				newLowerBound = 0;
				newUpperBound = 1;
				newNames = {'Control Magnitude'};
			else		
				newLowerBound = [];
				newUpperBound = [];
				newNames = {};
			end
			
			obj.AddLowerBounds(newLowerBound)
			obj.AddUpperBounds(newUpperBound)
			obj.AddNames(newNames)
		end
  
		function UpdateSphericalControlAlgFunctionBounds(obj)
		
			newLowerBound = [];
			newUpperBound = [];
			newNames = {};
		
			obj.AddLowerBounds(newLowerBound)
			obj.AddUpperBounds(newUpperBound)
			obj.AddNames(newNames)
		end
  
		function UpdateCartesianControlAlgFunctions(obj)

			if obj.GetNumControlVars()
				% newFunction = obj.controlVec'*obj.controlVec;
				newFunction = norm(obj.controlVec);
				obj.AddFunctions(newFunction);
			end

		end
	
	  	function UpdateStateControlandTime(obj)
			obj.time = obj.GetTime();
			obj.stateVec = obj.GetStateVec();
			obj.controlVec = obj.GetControlVec();
		end
	
		function UnScaleStateandTime(obj)
			global config
			obj.time = obj.time * config.TU;
			obj.stateVec = config.StateUnScaleMat * obj.stateVec;
		end
  
	  	function ScaleStateandTime(obj)
			obj.time = obj.time / config.TU;
			obj.stateVec = config.StateScaleMat*obj.stateVec;
		end

		function ScaleAccelerations(obj)
			global config
			obj.Xdot = config.StateAccScaleMat*obj.Xdot;
		end
	
		function UpdateIfSparsity(obj)	
			obj.ifSparsity = obj.GetIsSparsity();
		end
	
		function CalculateAccelerations_CartesianControl(obj,SEPForces)
		
	        if obj.ifSparsity
	            sharp0 = obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness;
	            obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness = 10;
	        end
			if sum(isnan(obj.controlVec))
				controlVec = zeros(3,1);
			elseif obj.GetNumControlVars()
				controlVec = obj.controlVec;
			else
				controlVec = zeros(3,1);
			end
			
	        [obj.Xdot,A]= obj.SEPForces.ComputeForcesGivenControl(obj.time,obj.stateVec,controlVec);
			if obj.ifSparsity
	            obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness = sharp0;
	        end
        end
        
        function CalculateAccelerations_SphericalControl(obj,SEPForces)
		
	        if obj.ifSparsity
	            sharp0 = obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness;
	            obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness = 10;
	        end
			if sum(isnan(obj.controlVec))
				controlVec = zeros(3,1);
			elseif obj.GetNumControlVars()
				controlVec = obj.controlVec;
			else
				controlVec = zeros(3,1);
			end
			
	        [obj.Xdot,A]= obj.SEPForces.ComputeForcesGivenSphericalControl(obj.time,obj.stateVec,controlVec);
			if obj.ifSparsity
	            obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness = sharp0;
	        end
		end
	
	end
  
end