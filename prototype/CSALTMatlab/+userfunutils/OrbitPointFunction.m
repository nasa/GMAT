classdef OrbitPointFunction < userfunutils.UserPointFunction

	properties

		UpperBounds
		LowerBounds
		Functions
		FuncNames
		
	end

	methods
		
		function InitializeBoundsFunctionsAndNames(obj)
			if obj.IsInitializing()
				obj.UpperBounds = [];
				obj.LowerBounds = [];
				obj.FuncNames = {};
				obj.Functions = [];
			else
				obj.Functions = [];
			end
		end
		
		function AddUpperBounds(obj,Input)
			if obj.IsInitializing()
				obj.UpperBounds = [obj.UpperBounds;Input];
			end
		end
		
		function AddLowerBounds(obj,Input)
			if obj.IsInitializing()
		
				obj.LowerBounds = [obj.LowerBounds;Input];
			end	
		end
		
		function AddNames(obj,Input)
			if obj.IsInitializing()
				% Input
				obj.FuncNames = [obj.FuncNames;Input];
			end
		end
		
		function AddFunctions(obj,Input)
		
			obj.Functions = [obj.Functions;Input];
			
		end
	
		function FinalizeBoundsFunctionsAndNames(obj)
			
			if obj.IsInitializing()
				obj.SetAlgFuncLowerBounds(obj.LowerBounds);
				obj.SetAlgFuncUpperBounds(obj.UpperBounds);
				obj.SetAlgFuncNames(obj.FuncNames);
            end
            
			obj.SetAlgFunctions(obj.Functions);
			
		end
	
		function EnforceTimeDirectionality(obj,phaseID)
			
			FinalTime   = obj.GetFinalTime(phaseID);
			InitialTime = obj.GetInitialTime(phaseID);

			if obj.IsInitializing()
				obj.AddUpperBounds(1);
				obj.AddLowerBounds(1e-8);
				obj.AddNames({['Phase ',num2str(phaseID),' Time Directionality']})
			end
	
			obj.AddFunctions(InitialTime/FinalTime);
			
		end
	
		function AddFlightTimeConstraint(obj,PhaseID,MinTime,MaxTime)
			global config
						
			if MaxTime == MinTime
				if obj.IsInitializing()
			
					obj.AddUpperBounds(1);
					obj.AddLowerBounds(1);
					obj.AddNames({['Phase ',num2str(PhaseID),' Flight Time Constraint']})
				
				end
			
				obj.AddFunctions(obj.GetFinalTime(PhaseID)/(MaxTime*86400/config.TU))
			else
				if obj.IsInitializing()
			
					obj.AddUpperBounds(1);
					obj.AddLowerBounds(0);
					obj.AddNames({['Phase ',num2str(PhaseID),' Flight Time Constraint']})
				
				end
			
				obj.AddFunctions((obj.GetFinalTime(PhaseID)-MinTime*86400/config.TU)/((MaxTime-MinTime)*86400/config.TU))
			end
			
			
		end
	
		function AddDeclinationConstraintWithSpice(obj,PhaseID,BodySpiceID,MinDeclination,MaxDeclination)
			
			vInf = obj.CalculateInitialVinf(PhaseID,BodySpiceID);
			
            R = [1.0 0.0 0.0;
                0.0 0.91748206 -0.397777156;
                0.0 0.39777716 0.9174820621]';
				
			vInfECI = R * vInf;
				
			declination = asin(vInfECI(3)/norm(vInfECI));
			if MaxDeclination == MinDeclination 
				if obj.IsInitializing()
			
					obj.AddUpperBounds(1);
					obj.AddLowerBounds(1);
					obj.AddNames({['Phase ',num2str(PhaseID),' Declination Constraint']})
				
				end
			
				obj.AddFunctions((declination)/(MaxDeclination))
			else
				if obj.IsInitializing()
			
					obj.AddUpperBounds(1);
					obj.AddLowerBounds(0);
					obj.AddNames({['Phase ',num2str(PhaseID),' Declination Constraint']})
				
				end
			
				obj.AddFunctions((declination-MinDeclination)/(MaxDeclination-MinDeclination))
			end
		end
	 
		function ConstrainInitialMass(obj,PhaseID,MinMass,MaxMass,Option)
			global config

			InitialState = obj.GetInitialStateVec(PhaseID);
			Mass = InitialState(7);
			MinMass = MinMass/config.MU;
			MaxMass = MaxMass/config.MU;

			if Option == 1

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Initial Mass Constraint']})

				end

				obj.AddFunctions([Mass/MaxMass]);
			elseif Option == 2

				if MaxMass == 0
					if obj.IsInitializing()

						obj.AddUpperBounds([0]);
						obj.AddLowerBounds([0]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Initial Mass Constraint']})

					end

					obj.AddFunctions([Mass]);
				else
					if obj.IsInitializing()

						obj.AddUpperBounds([1]);
						obj.AddLowerBounds([MinMass/MaxMass]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Initial Mass Constraint']})

					end

					obj.AddFunctions([Mass/MaxMass]);
				end
				
			elseif Option == 3

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Initial Mass Constraint']})

				end

				obj.AddFunctions([MinMass/Mass]);
				
			end
		end
		
		function ConstrainFinalMass(obj,PhaseID,MinMass,MaxMass,Option)
			global config

			FinalState = obj.GetFinalStateVec(PhaseID);
			Mass = FinalState(7);
			MinMass = MinMass/config.MU;
			MaxMass = MaxMass/config.MU;

			if Option == 1

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Final Mass Constraint']})

				end

				obj.AddFunctions([Mass/MaxMass]);
			elseif Option == 2

				if MaxMass == 0
					if obj.IsInitializing()

						obj.AddUpperBounds([0]);
						obj.AddLowerBounds([0]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Final Mass Constraint']})

					end

					obj.AddFunctions([Mass]);
				else
					if obj.IsInitializing()

						obj.AddUpperBounds([1]);
						obj.AddLowerBounds([MinMass/MaxMass]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Final Mass Constraint']})

					end

					obj.AddFunctions([Mass/MaxMass]);
				end
				
			elseif Option == 3

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Final Mass Constraint']})

				end

				obj.AddFunctions([MinMass/Mass]);
				
			end
		end
		
		function ConstrainInitialC3WithSpice(obj,PhaseID,BodySpiceID,MinC3,MaxC3,Option)
			global config

			vInf = obj.CalculateInitialVinf(PhaseID,BodySpiceID);

			C3 = vInf(1)*vInf(1)+vInf(2)*vInf(2)+vInf(3)*vInf(3);

			if Option == 1

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Initial C3 Constraint']})

				end

				obj.AddFunctions([C3/MaxC3]);
			elseif Option == 2

				if MaxC3 == 0
					if obj.IsInitializing()

						obj.AddUpperBounds([0]);
						obj.AddLowerBounds([0]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Initial C3 Constraint']})

					end

					obj.AddFunctions([C3]);
				else
					if obj.IsInitializing()

						obj.AddUpperBounds([1]);
						obj.AddLowerBounds([MinC3/MaxC3]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Initial C3 Constraint']})

					end

					obj.AddFunctions([C3/MaxC3]);
				end
			elseif Option == 3

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Initial C3 Constraint']})

				end

				obj.AddFunctions([MinC3/C3]);
				
			end
		end
	
		function ConstrainFinalC3WithSpice(obj,PhaseID,BodySpiceID,MinC3,MaxC3,Option)
			global config

			vInf = obj.CalculateFinalVinf(PhaseID,BodySpiceID);

			C3 = vInf(1)*vInf(1)+vInf(2)*vInf(2)+vInf(3)*vInf(3);

			if Option == 1

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Final C3 Constraint']})

				end

				obj.AddFunctions([C3/MaxC3]);
			elseif Option == 2
				if MaxC3 == 0
					if obj.IsInitializing()

						obj.AddUpperBounds([0]);
						obj.AddLowerBounds([0]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Final C3 Constraint']})

					end

					obj.AddFunctions([C3]);
				else
					if obj.IsInitializing()

						obj.AddUpperBounds([1]);
						obj.AddLowerBounds([MinC3/MaxC3]);
						obj.AddNames({['Phase ',num2str(PhaseID),' Final C3 Constraint']})

					end

					obj.AddFunctions([C3/MaxC3]);
				end
				
			elseif Option == 3

				if obj.IsInitializing()

					obj.AddUpperBounds([1]);
					obj.AddLowerBounds([0]);
					obj.AddNames({['Phase ',num2str(PhaseID),' Final C3 Constraint']})

				end

				obj.AddFunctions([MinC3/C3]);
				
			end
		end
	
		function AddInitialBoundaryConditionWithLaunchVehicleAndSpice(obj,PhaseID,BodySpiceID,LaunchWindow)
			global LV config
			
			if LaunchWindow == 0
				LaunchWindow = 1e-8;
			end
			
			obj.AddInitialBoundaryConditionWithSpice(PhaseID,BodySpiceID,1:3,[0,0,0])
			
			vInf = obj.CalculateInitialVinf(PhaseID,BodySpiceID);
			
			C3 = vInf(1)*vInf(1)+vInf(2)*vInf(2)+vInf(3)*vInf(3);

			MaxInitialMass = LV.GetMassToEscapeC3(C3) / config.MU;
			
			InitialState = obj.GetInitialStateVec(PhaseID);
			
			InitialTime = obj.GetInitialTime(PhaseID);
			
			if obj.IsInitializing()
			
				obj.AddUpperBounds([1;1]);
				obj.AddLowerBounds([0;0]);
				obj.AddNames({['Phase ',num2str(PhaseID),' Launch Vehicle Mass Constraint'];['Phase ',num2str(PhaseID),' Launch Window']})
				
			end
			
			obj.AddFunctions([InitialState(7)/MaxInitialMass;InitialTime/(LaunchWindow*86400/config.TU)]);
			
		end
	
		function AddInitialBoundaryConditionWithSpice(obj,PhaseID,BodySpiceID,StatesToConstrain,DeltaVector)
			
			RelState = obj.CalculateInitialStateRelativeToSpice(PhaseID,BodySpiceID);
			local_functions = zeros(length(StatesToConstrain),1);
			for i = 1:length(StatesToConstrain)
				if obj.IsInitializing()
					obj.AddUpperBounds(0);
					obj.AddLowerBounds(0);
					obj.AddNames({['Phase ',num2str(PhaseID),' State ',num2str(StatesToConstrain(i)),' Initial Boundary Condition relative to ',num2str(BodySpiceID)]})
				end
				local_functions(i) = RelState(StatesToConstrain(i))-DeltaVector(i);
			end
			
			obj.AddFunctions(local_functions);
		end
		
		function AddFinalBoundaryConditionWithSpice(obj,PhaseID,BodySpiceID,StatesToConstrain,DeltaVector)
			
			RelState = obj.CalculateFinalStateRelativeToSpice(PhaseID,BodySpiceID);

			local_functions = zeros(length(StatesToConstrain),1);
			for i = 1:length(StatesToConstrain)
				if obj.IsInitializing()
					obj.AddUpperBounds(0);
					obj.AddLowerBounds(0);
					obj.AddNames({['Phase ',num2str(PhaseID),' State ',num2str(StatesToConstrain(i)),' Final Boundary Condition relative to ',num2str(BodySpiceID)]})
				end

				local_functions(i) = RelState(StatesToConstrain(i))-DeltaVector(i);
			end
			obj.AddFunctions(local_functions);
		end
		
		function ConstrainStatesBetweenPhases(obj,EndingPhase,StartingPhase,StatesToConstrain,DeltaVector)
			
			FinalState   = obj.GetFinalStateVec(EndingPhase);
			InitialState = obj.GetInitialStateVec(StartingPhase);
			RelState = InitialState - FinalState;

			local_functions = zeros(length(StatesToConstrain),1);
			for i = 1:length(StatesToConstrain)
				if obj.IsInitializing()
					obj.AddUpperBounds(0);
					obj.AddLowerBounds(0);
					obj.AddNames({['Phase ',num2str(EndingPhase),' and Phase ',num2str(StartingPhase),' State ',num2str(StatesToConstrain(i)),' Link']})
				end
				local_functions(i) = RelState(StatesToConstrain(i))-DeltaVector(i);
			end
			obj.AddFunctions(local_functions);			
		end
		
		function ConstrainTimeBetweenPhases(obj,EndingPhase,StartingPhase,MinDeltaTime,MaxDeltaTime,Option)
			global config
			
			FinalTime   = obj.GetFinalTime(EndingPhase);
			InitialTime = obj.GetInitialTime(StartingPhase);
			RelTime = InitialTime - FinalTime;

			if Option == 1
				if MaxDeltaTime == 0
					if obj.IsInitializing()
						obj.AddUpperBounds(0);
						obj.AddLowerBounds(0);
						obj.AddNames({['Phase ',num2str(EndingPhase),' and Phase ',num2str(StartingPhase),' Time Link']})
					end
			
					obj.AddFunctions(RelTime);
				else
					if obj.IsInitializing()
						obj.AddUpperBounds(1);
						obj.AddLowerBounds(0);
						obj.AddNames({['Phase ',num2str(EndingPhase),' and Phase ',num2str(StartingPhase),' Time Link']})
					end
			
					obj.AddFunctions(RelTime/MaxDeltaTime);
				end
			elseif Option == 2
				if MaxDeltaTime == 0
					if MinDeltaTime ~= 0 
						disp('Bad time range options! Fix your point function!')
					end
					if obj.IsInitializing()
						obj.AddUpperBounds(0);
						obj.AddLowerBounds(0);
						obj.AddNames({['Phase ',num2str(EndingPhase),' and Phase ',num2str(StartingPhase),' Time Link']})
					end
			
					obj.AddFunctions(RelTime);
				else
					if obj.IsInitializing()
						obj.AddUpperBounds(1);
						obj.AddLowerBounds(MinDeltaTime/MaxDeltaTime);
						obj.AddNames({['Phase ',num2str(EndingPhase),' and Phase ',num2str(StartingPhase),' Time Link']})
					end
			
					obj.AddFunctions(RelTime/(MaxDeltaTime*86400/config.TU)-1);
				end
			elseif Option == 3
				if MinDeltaTime == 0 
					disp('Bad time range options! Fix your point function!')
				end
				if obj.IsInitializing()
					obj.AddUpperBounds(1e100);
					obj.AddLowerBounds(1);
					obj.AddNames({['Phase ',num2str(EndingPhase),' and Phase ',num2str(StartingPhase),' Time Link']})
				end
			
				obj.AddFunctions(RelTime/(MinDeltaTime*86400/config.TU));
			end
			
		end
	
		function Add2BodyFlybyBetweenPhases(obj,EndingPhase,StartingPhase,BodySpiceID,BodyMu,MinPeriapseRadius)
			
			obj.ConstrainStatesBetweenPhases(EndingPhase,StartingPhase,[1:3,7],[0;0;0;0]);
			obj.ConstrainTimeBetweenPhases(EndingPhase,StartingPhase,0,0,2);
			
			obj.AddInitialBoundaryConditionWithSpice(StartingPhase,BodySpiceID,1:3,[0;0;0])

			ApproachVInf = obj.CalculateFinalVinf(EndingPhase,BodySpiceID);
			DepartureVInf = obj.CalculateInitialVinf(StartingPhase,BodySpiceID);
			
			FlybyEccentricity = 1 + (MinPeriapseRadius*norm(ApproachVInf)*norm(DepartureVInf)/BodyMu);
			
			NeededTurnAngle = acos(dot(ApproachVInf,DepartureVInf)/(norm(ApproachVInf)*norm(DepartureVInf)));
			
			MaxTurnAngle = 2*asin(1/FlybyEccentricity);
			
			if obj.IsInitializing()
				obj.AddUpperBounds(1);
				obj.AddLowerBounds(0);
				obj.AddNames({['Phase ',num2str(EndingPhase),' 2 Body Approximated Flyby Turn Angle']})
			end
			
			obj.AddFunctions(NeededTurnAngle/MaxTurnAngle);
			
		end
	
		function RelState = CalculateInitialStateRelativeToSpice(obj,PhaseID,BodySpiceID)
			global config
		
			mjd = config.TimeOffSet + obj.GetInitialTime(PhaseID) * config.TU / 86400;
		
			StateBound = config.StateScaleMat(1:6,1:6)*cspice_spkezr(BodySpiceID,(mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');

			SpaceCraftState = obj.GetInitialStateVec(PhaseID);
		
			RelState = SpaceCraftState(1:6) - StateBound;
		end
		
		function RelState = CalculateFinalStateRelativeToSpice(obj,PhaseID,BodySpiceID)
			global config
			
			mjd = config.TimeOffSet + obj.GetFinalTime(PhaseID) * config.TU / 86400;
			
			StateBound = config.StateScaleMat(1:6,1:6)*cspice_spkezr(BodySpiceID,(mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');

			SpaceCraftState = obj.GetFinalStateVec(PhaseID);
			
			RelState = SpaceCraftState(1:6) - StateBound;
		end
	
		function vInf = CalculateInitialVinf(obj,PhaseID,BodySpiceID)
			global config
			
			RelState = obj.CalculateInitialStateRelativeToSpice(PhaseID,BodySpiceID);
			
			vInf = (RelState(4:6))*config.VU;
			
		end
		
		function vInf = CalculateFinalVinf(obj,PhaseID,BodySpiceID)
			global config
			
			RelState = obj.CalculateFinalStateRelativeToSpice(PhaseID,BodySpiceID);
			
			vInf = (RelState(4:6))*config.VU;
			
		end
	
	end

end