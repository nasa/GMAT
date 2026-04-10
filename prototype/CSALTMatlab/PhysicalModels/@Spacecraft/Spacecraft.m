classdef Spacecraft < handle
    
    %SPACECRAFT.  This is a prototype spacecraft class for GMAT design and
    %testing.  Written by S. Hughes.  steven.p.hughes@nasa.gov
    
    properties  (SetAccess = public)
        
        %%  Epoch, OrbitState, and ballistic properties
        Epoch       =  21545;
        OrbitState  = [7100 0 1300 0 7.35 1]';
        CentralBody = 3;
        Cd          = 2.2;
        Cr          = 1.8;
        DragArea    = 20;
        SRPArea     = 80;
        DryMass     = 1;
        Attitude    = 1;  % 1 for J2000, 2 for VNB
        
        %%  Power configuration
        %  Basic power config
        PowerModel         = 'Solar';  % Options: Solar, Nuclear.
        InitialMaxPower    = 1.2  %  kW
        PowerInitialEpoch  = 21545
        PowerDecayRate     = 5 ;  % Percent per year decay rate
        PowerMargin        = 5;
        ModelShadows       = 0;
        BusCoeff1          = .3;
        BusCoeff2          = 0;
        BusCoeff3          = 0;
        
        %  Define coeffiecients of the panel system
        SolarCoeff1 =  1.320770;
        SolarCoeff2 = -0.108480;
        SolarCoeff3 = -0.116650;
        SolarCoeff4 =  0.108430;
        SolarCoeff5 = -0.012790;
        
        %%  Constants and miscellaneous
        %AU = 149597870.691;
        AU = 1.49597870e8;
        
        
        %%  Hardware
        Thrusters   = {};
        Tanks       = {};
        ThrusterSet = {};
        debugMath = true();
        
    end
    
    methods
        
        %  Set the orbit state w/r/t CentralBody
        function SetOrbitState(obj,theState)
            [m,n] = size(theState);
            if m ~= 6 || n ~= 1
                disp('Error in Spacecraft::SetOrbitState.  State is not 6x1')
                stop
            end
            obj.OrbitState = theState;
        end
        
        %  Set the A1ModJulian epoch
        function SetOrbitEpoch(obj,theEpoch)
            obj.Epoch = theEpoch;
        end
        
        %  Set the central body
        function SetCentralBody(obj,centralBodyId)
            obj.CentralBody = centralBodyId;
        end
        
        %  Compute total power available
        function powerGenerated = GetPowerGenerated(obj)
            basePower = GetBasePower(obj);
            if strcmp(obj.PowerModel,'Solar')
                satToSunVector = GetSatToSunVector(obj);
                sunDist  = norm(satToSunVector)/obj.AU;
                %  Compute solar panel model
                num  =  obj.SolarCoeff1 + obj.SolarCoeff2/sunDist + ...
                    obj.SolarCoeff3/sunDist^2;
                denom = 1 + obj.SolarCoeff4*sunDist +...
                    obj.SolarCoeff5*sunDist^2;
                solarScaleFactor = 1/sunDist^2*num/denom;
                if obj.ModelShadows
                    percentSun = GetPercentSun(obj);
                else
                    percentSun = 1;
                end
                %  Englander Eq: 17
                powerGenerated = percentSun*basePower*solarScaleFactor;
            elseif strcmp(obj.PowerModel,'Nuclear')
                powerGenerated = basePower;
            end
            if obj.debugMath
                disp(['Power Generated : ' num2str(powerGenerated,16)]);
            end
        end
        
        %  Compute base power
        function basePower = GetBasePower(obj)
            yearsFromStart = (obj.Epoch - obj.PowerInitialEpoch)/365.25;
            % Englander: Eq. 18
            basePower = obj.InitialMaxPower*(1 -...
                obj.PowerDecayRate/100)^yearsFromStart;
            if obj.debugMath
                disp('======== Power System Debug Data ========');
                disp(['Base Power      : ' num2str(basePower,16)]);
            end
        end
        
        %  Compute power required by the s/c bus
        function busPower = GetSpacecraftBusPower(obj)
            sunDist  = GetSatToSunDistance(obj)/obj.AU;
            % Englander: Eq. 19
            busPower = obj.BusCoeff1 + obj.BusCoeff2/sunDist + ...
                obj.BusCoeff3/sunDist^2;
            if obj.debugMath
                disp(['Bus Power       : ' num2str(busPower,16)]);
                disp(['Sun Dist        : ' num2str(sunDist,16)]);
            end
        end
        
        %  Compute power available for thrust
        function powerAvailable = GetThrustPower(obj)
            generatedPower  = GetPowerGenerated(obj);
            busPower        = GetSpacecraftBusPower(obj);
            %  Englander: Eq. 16
            powerAvailable = (1-obj.PowerMargin /100)*...
                (generatedPower - busPower);
            if powerAvailable < 0
                %  Don't set to zero to avoid derivative issues.
%                 powerAvailable = 1e-10;
            end
            if obj.debugMath
                disp(['Power Available : ' num2str(powerAvailable,16)]);
            end
        end
        
        %  Compute total mass
        function totalMass = GetTotalMass(obj)
            totalMass = obj.DryMass;
            for i = 1:length(obj.Tanks)
                totalMass = totalMass + obj.Tanks{i}.FuelMass;
            end
        end
        
        %  Compute the vector from spacecraft to Sun
        function satToSunVector = GetSatToSunVector(obj)
            if obj.CentralBody == 11
                satToSunVector = -obj.OrbitState(1:3);
            else
            	sunVec = GetSunVector(obj);
                satToSunVector = sunVec - obj.OrbitState(1:3);
            end
        end
        
        %  Compute the distance to the sun
        function sunDistance = GetSatToSunDistance(obj)
            satToSunVector = GetSatToSunVector(obj);
            sunDistance    = norm(satToSunVector);
        end
        
        %  Get the percent sun for shadowing by Earth
        function percentSun = GetPercentSun(obj)
            %  Get body radii and location of sun and sat.
            satPos     = obj.OrbitState(1:3,1);
            sunPos     = obj.GetSunVector();
            percentSun = GetPercentSun(satPos,sunPos);
            if obj.debugMath
                disp(['Percent Sun     : ' num2str(percentSun,16)]);
            end
        end
        
        %  Returns the vector from central body to the sun
        function sunVec = GetSunVector(obj)
            jdTAI  = obj.Epoch - 0.0343817/86400 + 2430000.;
            jdTT   = jdTAI + 32.184/86400;
            ttt = (jdTT - 2451545) / 36525;
            mE = mod(357.5277233 + 35999.05034 * ttt, 360);
            jdTDB = jdTT + (0.001658 * sind(mE) + 0.00001385 *sind(2*mE))/(86400);
            % TODO: Fix this, NaN should be handled better
            if isnan(jdTDB)
                sunVec = [1000 10000 10000]';
                return
            end
            sunVec = pleph(jdTDB,11,obj.CentralBody,1);
        end
        
        %  Set and configure attached thrusters
        function SetThrusterSet(obj,ThrusterSet)
            % Input is a cell array of Thruster objects
            numThrusters = length(ThrusterSet.Thrusters);
            for idx = 1:numThrusters
                ThrusterSet.Thrusters{idx}.Spacecraft = obj;
            end
            ThrusterSet.Spacecraft = obj;
            obj.ThrusterSet = ThrusterSet;
        end
        
        %  Set the mass on the tank.
        function SetTankMass(obj,totalMass)
            % Assumes there is only ONE tank!
            %  TODO:  Add a method to FuelTank to do this.
            obj.Tanks{1}.FuelMass = totalMass - obj.DryMass;
        end
        
        %  Set the mass on the tank.
        function SetTankMassTankOnly(obj,tankMass)
            % Assumes there is only ONE tank!
            obj.Tanks{1}.FuelMass = tankMass;
        end
    end
    
end



