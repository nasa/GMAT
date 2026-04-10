classdef FiniteBurn < handle
    
    %FiniteBurn.  This class groups thrusters in a set, handles throttle
    %logic, which distributes power across thrusters, and computes the
    %total thrust vector and mass flow summing across all thrusters.
    %
    %  Author: S. Hughes, steven.p.hughes@nasa.gov
	%  Modified: J. Knittel, jeremy.m.knittel@nasa.gov 3/2/2016
    
    properties
        Thrusters     = {}
        % Options: ByOrder, MaxPower,MaxThrust, MinimumFlowRate
        ThrottleLogic = 'MaxNumberOfThrusters';
        Spacecraft
        numThrusters  = 0;
		numToFire = 0;
        debugMath = false();
		Heaviside = 0;
		
		throttleHeavisideSharpness = 1000;
    end
    
    methods
        
        %%  Set and configure attached thrusters
        function SetThrusters(obj,Thrusters)
            % input is a cell array of Thruster objects
            obj.numThrusters = length(Thrusters);
			obj.Heaviside = ones(1,obj.numThrusters);
            for idx = 1:obj.numThrusters
                obj.Thrusters{idx} = Thrusters{idx};
            end
        end
        
        %%  Use throttle logic to divide power across thrusters, then
        %   compute sum total thrust and and mass flow rate
        function [thrust,massFlowTotal] = ...
                GetThrustAndMassFlowRate(obj,powerAvailable)
            
            %  Use throttle logic to distribute power to thrusters
            ComputeThrottleLogic(obj,powerAvailable);
            
            %  Compute the total thrust vector and mass flow rate
            massFlowTotal  = 0;
            thrust =  zeros(3,1);
            for i = 1:obj.numThrusters
                [thrustVector, massFlowRate] = ...
                    obj.Thrusters{i}.GetThrustVectorMassFlowRate();
                thrust  = thrust + thrustVector * obj.Heaviside(i);
                massFlowTotal = massFlowTotal + massFlowRate * obj.Heaviside(i);
            end
			
            if obj.debugMath
                disp(['======== Finite Burn Debug Data ========']);
                disp(['Power Available : ' num2str(powerAvailable,16)]);
                disp(['Total Mass Flow : ' num2str(massFlowTotal,16)]);
                disp(['Total Thrust  X : ' num2str(thrust(1),16)]);
                disp(['Total Thrust  Y : ' num2str(thrust(2),16)]);
                disp(['Total Thrust  Z : ' num2str(thrust(3),16)]);
            end
        end
        
        %% Compute how power is distributed across thrusters
        function ComputeThrottleLogic(obj,powerAvailable)
            if strcmp(obj.ThrottleLogic,'MaxNumberOfThrusters')
                ThrustLogic_MaxNumberOfThrusters(obj,powerAvailable);
			elseif strcmp(obj.ThrottleLogic,'HeavisideMaxNumberOfThrusters')
                ThrustLogic_Heaviside_MaxNumberOfThrusters(obj,powerAvailable);
			elseif strcmp(obj.ThrottleLogic,'HeavisideMinNumberOfThrusters')
                ThrustLogic_Heaviside_MinNumberOfThrusters(obj,powerAvailable);
			elseif strcmp(obj.ThrottleLogic,'PreAllocatedPower')
				ThrustLogic_PreAllocatedPower(obj);
            end
        end
        
		function ThrustLogic_PreAllocatedPower(obj)
			obj.numToFire = obj.numThrusters;
		end
		
        %%  Throttle logic for Max Number of Thrusters case.  This function
        % computes how power should be divided across thrusters and sets
        % the power on the thrusters.
        function ThrustLogic_MaxNumberOfThrusters(obj,powerAvailable)
            
            %  Try firing all thrusters, then drop one thruster each pass
            %  through the loop, (thrusters are dropped off the end of the
            %  list), until we find a feasbible subset.
            for numToFire = obj.numThrusters:-1:1
                powerPerThruster = powerAvailable/numToFire;
                %  Compute mean min usable power, if we were to fire
                %  numToFire thrusters
                meanMinUsablePower = 0;
                for thustIdx = 1:numToFire
                    meanMinUsablePower = meanMinUsablePower + ...
                        obj.Thrusters{thustIdx}.MinimumUsablePower;
                end
                meanMinUsablePower = meanMinUsablePower/numToFire;
                %  If power can be distributed across "numToFire" thrusters
                %  break out, we're done.
                if powerPerThruster > meanMinUsablePower
                    break
                end
                %  Handle the special case when there is not enough power to
                %  fire any thrusters.
                if numToFire == 1 && powerPerThruster < meanMinUsablePower
                    numToFire = 0;
                    break
                end
            end
            obj.numToFire = numToFire;
            %  Divide up power across thrusters that should fire
            for i = 1:numToFire
                obj.Thrusters{i}.SetPowerAllocated(powerPerThruster);
            end
            %  Set power to zero for thrusters that should not fire
            for i = numToFire+1:obj.numThrusters
                obj.Thrusters{i}.SetPowerAllocated(0)
            end
            
        end
		
		%% Throttle logic for Min Number of Thrusters using heaviside functions.
		% Use of heaviside functions are so that the number of thrusters firing, 
		% thrust and mass flow rate are all continuous. This function decides 
		% how to divide up the power available and sends it to each thruster
        function ThrustLogic_Heaviside_MinNumberOfThrusters(obj,powerAvailable)

			% Initialize variables    
			maxP = zeros(1,obj.numThrusters);
			minP = zeros(1,obj.numThrusters);
			obj.Heaviside = zeros(1,obj.numThrusters);
			cumulative_power = 0;
			obj.numToFire = obj.numThrusters;
			ifFoundNumToFire = 0;
			extra_H = zeros(1,obj.numThrusters);
			extra = zeros(1,obj.numThrusters);
			powerToAllocate = zeros(1,obj.numThrusters);
			minP_times_H = 0;
			
			% Get the minimum and maximum usable power for each thruster and
			% then sort the thrusters by minimum usable power. The thrusters
			% will only be allowed to turn on in this order. This makes sense
			% because we are trying to fire as many as possible. However, 
			% it could result in some power being wasted if there is not
			% enough power to turn on a second thruster, but the power available
			% is more than the first thruster can handle.
			for i = 1:obj.numThrusters
				minP(i) = obj.Thrusters{i}.MinimumUsablePower;
				maxP(i) = obj.Thrusters{i}.MaximumUsablePower;
			end
			[maxP,maxIdx] = sort(maxP,2,'descend');
			minP = minP(maxIdx);
			
			% Loop through the thrusters and determine how much power would be
			% required to turn each one on
            for i = 1:obj.numThrusters
				
				if i == 1
					Pcrit = minP(i);
				elseif i == 2
					Pcrit = maxP(i-1)-minP(i-1);
				else
					Pcrit = maxP(i-1);
				end
								
				% Cumulative power required to turn on the i-th thruster
				cumulative_power = cumulative_power + Pcrit;
				
				% Check if the number of engines to fire has been found yet
				if ifFoundNumToFire == 0
					% It has not. 
					
					% Check if this is the last thruster that can be turned on
					if powerAvailable == cumulative_power
						% It is.
						obj.numToFire = i;
						ifFoundNumToFire = 1;
						
					% Check if this thruster can be turned on.
					elseif powerAvailable < cumulative_power
						% It cannot
						obj.numToFire = i-1;
						ifFoundNumToFire = 1;
					end
				end
				
				% Compute the heaviside function for this thruster
				expfun = exp(-obj.throttleHeavisideSharpness*(powerAvailable-cumulative_power));
				obj.Heaviside(maxIdx(i)) = 1/(1+expfun);	
						
				% Compute the cumulative amount of power just to turn on the currently firing engines. 
				% Note that in order to smooth things out, the heaviside function is used. So,
				% when a thruster is close to the PowerAvailable at which it gets turned on, 
				% the thruster is "partially" turned on. For a very small range of PowerAvailable
				% it is between 0 and 100% turned on, and for this range it eats up a percentage of
				% the available power and generates a percentage of the overall thrust.
				minP_times_H = minP_times_H + minP(i) * obj.Heaviside(maxIdx(i));
						
			end
			
			% Now we need to allocate the available power to each thruster. We are going to loop until we find a 
			% valid solution. 
			while true
				
				% At the beginning of each iteration assume that all thrusters will be allocated power in their range
				ifInRange = 1;
				
				% Get a temporary holder of power allocation to each thruster
				temp_powerToAllocate = zeros(1,obj.numThrusters);
				
				% Loop through each thruster
				for i = 1:obj.numThrusters
                    
                    if powerToAllocate(i) == 0
						% It has not.
						
						% Compute the amount of power to allocate. The thruster always gets its minimum power, minP. Add
						% to that, a share of the excess power. The excess power is the total power available - the minimum
						% power to turn on all of the "firing" thrusters (minP_times_H) - the power already allocated (sum(extra)).
						% The excess power is divided by all of the "firing" thrusters (sum(obj.Heaviside))- the number of thrusters
						% with their power already allocated (sum(extra_H)) 
						powerVal = minP(i) + (powerAvailable - sum(extra.*extra_H) - minP_times_H) / (sum(obj.Heaviside)-sum(extra_H));

						% If the first engine is not fully on yet, then give it either its minimum power or all of the available power
						% whichever is greater
						if sum(obj.Heaviside) < 1 && i == 1
							if powerAvailable > minP(1)
								powerToAllocate(1) = powerAvailable;
							else
								powerToAllocate(1) = minP(1);
							end
						
						% If the first engine is not fully on yet, and i is not the first engine, then just give the engine it's minimum power 
						elseif sum(obj.Heaviside) < 1
							powerToAllocate(i) = minP(1);
							
						% Below here, at least one engine is fully on. We are going to give power to all engines as if they are fully on
						% even if they aren't. The thrust and mdot for them will be calculated as if they are on, but then when
						% the forces and mass flow rates are summed, the heaviside functions will eliminate the non-existant thrust and mdot.
						% They are given power as if they are on though, because we don't know apriori when they are going to start transitioning
						% on. And if they transition on from fully off there is a discontinuity in the net thrust. Instead, we transition them from 
						% a high power setting, to a lower power setting. That way the thrusters that are turning on compensate perfectly for the
						% lost thrust from lowering the power of the actually firing thrusters to a lower power setting. They need to transition from
						% a high power setting to a lower power setting because new engines are turning on, so the originally firing thruster has to 
						% start sharing power.
						
						% Check if the power allocated is less than the minimum for this thruster and if so, give it its minimum. The heaviside
						% function will make sure it doesn't produce thrust from imaginary power
						elseif powerVal < minP(i)
							powerToAllocate(i) = minP(i)*obj.Heaviside(maxIdx(i));
							
						% Check if the power allocated is more than this thruster can handle 
						elseif powerVal > maxP(i)
							% It is.
							
							% Give this thruster it's maximum power, and put the extra power into 'extra' so that the other thrusters can share it
							extra(i) = maxP(i)-minP(i);
							extra_H(i) = obj.Heaviside(maxIdx(i));
							powerToAllocate(i) = maxP(i)*obj.Heaviside(maxIdx(i));
							
							% This thruster was not in range, so we need to recalculate the power allocation for the other thrusters in a new iteration
							ifInRange = 0;
						else
							
							% This thruster's power allocation was in an acceptable range
							temp_powerToAllocate(i) = powerVal;%*obj.Heaviside(maxIdx(i));
						end	
					end
				end
				
				% Check if all thruster's power has been allocated or was in an acceptable range
				if ifInRange
					% It was!
					
					% Loop through the thrusters and store the power to each
					for i = 1:obj.numThrusters
						if powerToAllocate(i) == 0
				    		obj.Thrusters{maxIdx(i)}.SetPowerAllocated(temp_powerToAllocate(i));	
						else
				    		obj.Thrusters{maxIdx(i)}.SetPowerAllocated(powerToAllocate(i));	
						end
					end
					
					% Break out of the while loop
					break;
				end
            end
            
			if ifFoundNumToFire == 0
				obj.numToFire = obj.numThrusters;
			end			
        end
		
		
		%% Throttle logic for Max Number of Thrusters using heaviside functions.
		% Use of heaviside functions are so that the number of thrusters firing, 
		% thrust and mass flow rate are all continuous. This function decides 
		% how to divide up the power available and sends it to each thruster
        function ThrustLogic_Heaviside_MaxNumberOfThrusters(obj,powerAvailable)

			% Initialize variables    
			maxP = zeros(1,obj.numThrusters);
			minP = zeros(1,obj.numThrusters);
			obj.Heaviside = zeros(1,obj.numThrusters);
			cumulative_power = 0;
			obj.numToFire = obj.numThrusters;
			ifFoundNumToFire = 0;
			extra_H = zeros(1,obj.numThrusters);
			extra = zeros(1,obj.numThrusters);
			powerToAllocate = zeros(1,obj.numThrusters);
			minP_times_H = 0;
			
			% Get the minimum and maximum usable power for each thruster and
			% then sort the thrusters by minimum usable power. The thrusters
			% will only be allowed to turn on in this order. This makes sense
			% because we are trying to fire as many as possible. However, 
			% it could result in some power being wasted if there is not
			% enough power to turn on a second thruster, but the power available
			% is more than the first thruster can handle.
			for i = 1:obj.numThrusters
				minP(i) = obj.Thrusters{i}.MinimumUsablePower;
				maxP(i) = obj.Thrusters{i}.MaximumUsablePower;
			end
			[minP,minIdx] = sort(minP);
			maxP = maxP(minIdx);
			
			% Loop through the thrusters and determine how much power would be
			% required to turn each one on
            for i = 1:obj.numThrusters
				% Cumulative power required to turn on the i-th thruster
				cumulative_power = cumulative_power + minP(i);
				
				% Check if the number of engines to fire has been found yet
				if ifFoundNumToFire == 0
					% It has not. 
					
					% Check if this is the last thruster that can be turned on
					if powerAvailable == cumulative_power
						% It is.
						obj.numToFire = i;
						ifFoundNumToFire = 1;
						
					% Check if this thruster can be turned on.
					elseif powerAvailable < cumulative_power
						% It cannot
						obj.numToFire = i-1;
						ifFoundNumToFire = 1;
					end
				end
				
				% Compute the heaviside function for this thruster
				expfun = exp(-obj.throttleHeavisideSharpness*(powerAvailable-cumulative_power));
				obj.Heaviside(minIdx(i)) = 1/(1+expfun);	
				
				% Compute the cumulative amount of power just to turn on the currently firing engines. 
				% Note that in order to smooth things out, the heaviside function is used. So,
				% when a thruster is close to the PowerAvailable at which it gets turned on, 
				% the thruster is "partially" turned on. For a very small range of PowerAvailable
				% it is between 0 and 100% turned on, and for this range it eats up a percentage of
				% the available power and generates a percentage of the overall thrust.
				minP_times_H = minP_times_H + minP(i) * obj.Heaviside(minIdx(i));
					
			end

			% Now we need to allocate the available power to each thruster. We are going to loop until we find a 
			% valid solution. 
			while true
				
				% At the beginning of each iteration assume that all thrusters will be allocated power in their range
				ifInRange = 1;
				
				% Get a temporary holder of power allocation to each thruster
				temp_powerToAllocate = zeros(1,obj.numThrusters);
				
				% Loop through each thruster
				for i = 1:obj.numThrusters
					
					% Check if power has already been allocated to this thruster
					if powerToAllocate(i) == 0
						% It has not.
						
						% Compute the amount of power to allocate. The thruster always gets its minimum power, minP. Add
						% to that, a share of the excess power. The excess power is the total power available - the minimum
						% power to turn on all of the "firing" thrusters (minP_times_H) - the power already allocated (sum(extra)).
						% The excess power is divided by all of the "firing" thrusters (sum(obj.Heaviside))- the number of thrusters
						% with their power already allocated (sum(extra_H)) 
						powerVal = minP(i) + (powerAvailable - sum(extra.*extra_H) - minP_times_H) / (sum(obj.Heaviside)-sum(extra_H));

						% If the first engine is not fully on yet, then give it either its minimum power or all of the available power
						% whichever is greater
						if sum(obj.Heaviside) < 1 && i == 1
							if powerAvailable > minP(1)
								powerToAllocate(1) = powerAvailable;
							else
								powerToAllocate(1) = minP(1);
							end
						
						% If the first engine is not fully on yet, and i is not the first engine, then just give the engine it's minimum power 
						elseif sum(obj.Heaviside) < 1
							powerToAllocate(i) = minP(1);
							
						% Below here, at least one engine is fully on. We are going to give power to all engines as if they are fully on
						% even if they aren't. The thrust and mdot for them will be calculated as if they are on, but then when
						% the forces and mass flow rates are summed, the heaviside functions will eliminate the non-existant thrust and mdot.
						% They are given power as if they are on though, because we don't know apriori when they are going to start transitioning
						% on. And if they transition on from fully off there is a discontinuity in the net thrust. Instead, we transition them from 
						% a high power setting, to a lower power setting. That way the thrusters that are turning on compensate perfectly for the
						% lost thrust from lowering the power of the actually firing thrusters to a lower power setting. They need to transition from
						% a high power setting to a lower power setting because new engines are turning on, so the originally firing thruster has to 
						% start sharing power.
						
						% Check if the power allocated is less than the minimum for this thruster and if so, give it its minimum. The heaviside
						% function will make sure it doesn't produce thrust from imaginary power
						elseif powerVal < minP(i)
							powerToAllocate(i) = minP(i);
							
						% Check if the power allocated is more than this thruster can handle 
						elseif powerVal > maxP(i)
							% It is.
							
							% Give this thruster it's maximum power, and put the extra power into 'extra' so that the other thrusters can share it
							extra(i) = maxP(i)-minP(i);
							extra_H(i) = obj.Heaviside(minIdx(i));
							powerToAllocate(i) = maxP(i);
							
							% This thruster was not in range, so we need to recalculate the power allocation for the other thrusters in a new iteration
							ifInRange = 0;
						else
							
							% This thruster's power allocation was in an acceptable range
							temp_powerToAllocate(i) = powerVal;
						end	
					end
				end
				
				% Check if all thruster's power has been allocated or was in an acceptable range
				if ifInRange
					% It was!
					
					% Loop through the thrusters and store the power to each
					for i = 1:obj.numThrusters
						if powerToAllocate(i) == 0
				    		obj.Thrusters{minIdx(i)}.SetPowerAllocated(temp_powerToAllocate(i));	
						else
				    		obj.Thrusters{minIdx(i)}.SetPowerAllocated(powerToAllocate(i));	
						end
					end
					
					% Break out of the while loop
					break;
				end
			end
			
			% Set the class variables
			if ifFoundNumToFire == 0
				obj.numToFire = obj.numThrusters;
			end				
        end
    end
end



