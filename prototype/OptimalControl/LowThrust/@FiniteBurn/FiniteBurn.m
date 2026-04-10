classdef FiniteBurn < handle
    
    %FiniteBurn.  This class groups thrusters in a set, handles throttle
    %logic, which distributes power across thrusters, and computes the
    %total thrust vector and mass flow summing across all thrusters.
    %
    %  Author: S. Hughes, steven.p.hughes@nasa.gov
    
    properties
        Thrusters     = {}
        % Options: ByOrder, MaxPower,MaxThrust, MinimumFlowRate
        ThrottleLogic = 'MaxNumberOfThrusters';
        Spacecraft
        numThrusters  = 0;
        debugMath = false();
    end
    
    methods
        
        %%  Set and configure attached thrusters
        function SetThrusters(obj,Thrusters)
            % input is a cell array of Thruster objects
            obj.numThrusters = length(Thrusters);
            for idx = 1:obj.numThrusters
                obj.Thrusters{idx} = Thrusters{idx};
            end
        end
        
        %%  Use throttle logic to divide power across thrusters, then
        %   compute sum total thrust and and mass flow rate
        function [thrust,massFlowRate] = ...
                GetThrustAndMassFlowRate(obj,powerAvailable)
            
            %  Use throttle logic to distribute power to thrusters
            ComputeThrottleLogic(obj,powerAvailable);
            
            %  Compute the total thrust vector and mass flow rate
            massFlowTotal  = 0;
            thrust =  zeros(3,1);
            for i = 1:obj.numThrusters
                [thrustVector, massFlowRate] = ...
                    obj.Thrusters{i}.GetThrustVectorMassFlowRate();
                thrust  = thrust + thrustVector;
                massFlowTotal = massFlowTotal + massFlowRate;
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
                ThrustLogic_MaxNumberOfThrusters(obj,powerAvailable)
            end
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
            
            %  Divide up power across thrusters that should fire
            for i = 1:numToFire
                obj.Thrusters{i}.SetPowerAllocated(powerPerThruster);
            end
            %  Set power to zero for thrusters that should not fire
            for i = numToFire+1:obj.numThrusters
                obj.Thrusters{i}.SetPowerAllocated(0)
            end
            
        end
    end
end



