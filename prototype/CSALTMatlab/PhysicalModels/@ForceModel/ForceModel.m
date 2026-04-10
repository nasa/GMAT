classdef ForceModel < handle
    
    properties (SetAccess = 'public')
        
        %  Flags to determine which thusters to use
        UseSTM        = 0;
        UseSRP        = 0;
        UseRelativity = 0;
        Debug         = 0;
        UseFiniteBurn = 1;
        Drag          = 'None';
        Spacecraft
        ComputeAMatrix = 0;
        
        %  New stuff
        CentralBodyId = 3
        PointMassIds
        SolarSystem
        
    end
    
    properties (SetAccess = 'private')
        %  Contants.  These should not go here but am refactoring and will
        %  get back to this. -SPH
        deltaAT       = 32;
        xp            = -0.083901;
        yp            = 0.467420;
        deltaUT1      = -0.4709524;
        lod           = 0.0004155;
        CentralBodyMu = 398600.4415;
        flattening    = 0.0033527;
        bodyRadius    = 6378.1363;
        bodyMu        = 398600.4415;
        speedOfLight  = 299792.458;
        muSun         = 132712440017.99;
        radiusSun     = 695990;
        refHeight     = 0;
        refDensity    = 1217000000;
        scaleHeight   = 8.5;
        F107A         = 150;
        F107          = 150;
        Ap            = 15*ones(1,7);
        flux          = 1367;
        AU            = 149597870.691;
        RefEpoch
        debugMath = false;
        
        
        orbitForce = zeros(6,1);
        VarEq_Orbit_Orbit = zeros(0,0);
        
    end
    
    methods
        
        function obj = ForceModel()
            obj.SolarSystem.Mu = [22032.080486418
                324858.59882646
                398600.4415
                42828.314258067
                126712767.85780
                37940626.061137
                5794549.0070719
                6836534.0638793
                981.60088770700
                4902.8005821478
                132712440017.99];
        end
        
        function Initialize(obj,Config,Spacecraft)
            obj.UseSTM = Config.UseSTM;
            obj.UseSRP = Config.UseSRP;
            obj.UseRelativity = Config.UseRelativity;
            obj.Debug = Config.Debug;
            obj.UseFiniteBurn = Config.UseFiniteBurn;
            obj.Drag = Config.Drag;
            obj.Spacecraft = Spacecraft;
            obj.ComputeAMatrix = Config.ComputeAMatrix;
            obj.RefEpoch = Spacecraft.Epoch;
            obj.PointMassIds = Config.PointMassIds;
            obj.CentralBodyId = Config.CentralBodyIdx;
            obj.Spacecraft.SetCentralBody(obj.CentralBodyId);
        end
        
        function [Xdot,A] = ComputeForces(obj,t,x)
            [Xdot,A] = obj.EvaluateODE(t,x);
        end
        
        function [Xdot,A] = ComputeForcesGivenControl(obj,t,x,u)
			for i = 1:obj.Spacecraft.ThrusterSet.numThrusters
				obj.Spacecraft.ThrusterSet.Thrusters{i}.ThrustMode = 2;
            	obj.Spacecraft.ThrusterSet.Thrusters{i}.SetThrustVector(u);
			end
            [Xdot,A] = obj.EvaluateODE(t,x);
        end
		
        function [Xdot,A] = ComputeForcesGivenSphericalControl(obj,t,x,u)
			for i = 1:obj.Spacecraft.ThrusterSet.numThrusters
				obj.Spacecraft.ThrusterSet.Thrusters{i}.ThrustMode = 4;
            	obj.Spacecraft.ThrusterSet.Thrusters{i}.SetThrustVector(u);
            end
            [Xdot,A] = obj.EvaluateODE(t,x);
        end
		
        function [Xdot,A] = ComputeForcesGivenPowerPerEngine(obj,t,x,u)
			for i = 1:obj.Spacecraft.ThrusterSet.numThrusters
				obj.Spacecraft.ThrusterSet.Thrusters{i}.ThrustMode = 3;
                if 2+i <= length(u)
                    obj.Spacecraft.ThrusterSet.Thrusters{i}.SetPowerAllocated(u(2+i));
                else
                    obj.Spacecraft.ThrusterSet.Thrusters{i}.SetPowerAllocated(0);
                end
            end
            [Xdot,A] = obj.EvaluateODE(t,x);
        end
        
        function [jdTDB] = MJDA1ToTDB(~,mjdA1)
            jdTAI  = mjdA1 - 0.0343817/86400 + 2430000.;
            jdTT   = jdTAI + 32.184/86400;
            ttt = (jdTT - 2451545) / 36525;
            mE = mod(357.5277233 + 35999.05034 * ttt, 360);
            jdTDB = jdTT+(0.001658*sind(mE)+0.00001385*sind(2*mE))/(86400);
        end
        
        function SumForce(obj,forceVec)
            obj.orbitForce(4:6) = obj.orbitForce(4:6) + forceVec;
        end
        
        function SumVarCMatrix(obj,CMatrix)
            obj.VarEq_Orbit_Orbit(4:6,1:3) = obj.VarEq_Orbit_Orbit(4:6,1:3) + CMatrix;
        end
        
        function ResetOrbitVarMatrix(obj)
            obj.VarEq_Orbit_Orbit = zeros(6,6);
            obj.VarEq_Orbit_Orbit(1:3,4:6) = eye(3);
        end
        
        function [Xdot,A] = EvaluateODE(obj,t,X)
            
            %% Initializations
            obj.orbitForce = zeros(6,1);
            obj.orbitForce(1:3,1) = X(4:6,1);
            obj.ResetOrbitVarMatrix();
            
            %  Update objects based on state vector
            obj.Spacecraft.OrbitState = X(1:6,1);
            obj.Spacecraft.Epoch = obj.RefEpoch + t/86400;
            if obj.UseFiniteBurn
                tankMass = X(7,1) - obj.Spacecraft.DryMass;
                obj.Spacecraft.SetTankMassTankOnly(tankMass);
            end
            
            %% Compute Forces from Point Mass Perturbations
            [Force,CMatrix] = obj.PointMassGrav(obj.Spacecraft.Epoch,X);
            obj.SumForce(Force);
            obj.SumVarCMatrix(CMatrix)
            
            % Compute force due to SRP
%             if obj.UseSRP
%                 [Force] = obj.SRPAccel(obj.Spacecraft.Epoch,X);
%                 obj.SumForce(Force);
%             end
            
            %% Compute thrust force
            massFlowTotal = 0;
            if obj.UseFiniteBurn
                powerAvailable = obj.Spacecraft.GetThrustPower();
                [thrustVector,massFlowTotal]= ...
                    obj.Spacecraft.ThrusterSet.GetThrustAndMassFlowRate(powerAvailable);
                Force = thrustVector/obj.Spacecraft.GetTotalMass();
                obj.SumForce(Force);
            end
            
            %%  Sum the forces
            if obj.UseFiniteBurn
                Xdot = [obj.orbitForce; massFlowTotal];
            else
                Xdot = obj.orbitForce;
            end
            
            A = obj.VarEq_Orbit_Orbit;
            
        end
        
        %  Computes acceleration and variational terms for point mass grav
        function [xddot,C] = PointMassGrav(obj,mjdA1,x)
            
            % TODO:  Move these.  Workaround for strange behavior
            Mu = [22032.080486418
                324858.59882646
                398600.4415
                42828.314258067
                126712767.85780
                37940626.061137
                5794549.0070719
                6836534.0638793
                981.60088770700
                4902.8005821478
                132712440017.99];
            
            %  Calculate useful terms for later
            rv     = x(1:3,1);
            r      = sqrt(rv'*rv);
            r3     = r^3;
            r5     = r^5;
            eye3   = eye(3);
            muCB   = Mu(obj.CentralBodyId);
            mubyr3 = muCB/r3;
            
            %  Calulate terms associated with central body
            xddot(1:3,1) = -mubyr3*rv;
            C  = -mubyr3*eye3 + 3*muCB*(rv*rv')/r5;
            
            %  Calulate terms associated with point mass perturbations
            xddot_PM   = zeros(3,1);
            C_PM      = zeros(3,3);
            [jd_tdb] = MJDA1ToTDB(obj,mjdA1);
            for i = 1:length(obj.PointMassIds)
                %  If the point mass is not the central body!!
                if obj.CentralBodyId ~= obj.PointMassIds(i)
                    muk = Mu(obj.PointMassIds(i));
                    rvk = obj.GetCBBodyState(jd_tdb,obj.PointMassIds(i),obj.CentralBodyId);
                    rvkmr   = rvk - rv;
                    rvk3    = norm(rvk)^3;
                    rvkmr3  = norm(rvkmr)^3;
                    rvkmr5  = norm(rvkmr)^5;
                    xddot_PM = xddot_PM + muk*(rvkmr/rvkmr3 - rvk/rvk3);
                    C_PM  = C_PM + muk*( - 1/rvkmr3*eye3 + 3*(rvkmr*rvkmr')/rvkmr5);
                end
            end
            
            %  Now add in point mass perturbations to acceleration and Amat
            xddot =  xddot + xddot_PM;
            C = C + C_PM;
        end
        
        function [stateVec] = GetCBBodyState(~,jd_tdb,targetId,centerId)
            stateVec = pleph(jd_tdb,targetId,centerId,1);
        end
        
        function [xddot] = SRPAccel(obj,mjdA1,x)
            if 11 == obj.CentralBodyId
                rvCBToSun = zeros(3,1);
            else
                [jd_tdb] = MJDA1ToTDB(obj,mjdA1);
                rvCBToSun = obj.GetCBBodyState(jd_tdb,11,obj.CentralBodyId);
            end
            
            %%  Calculate the shadow function
            rvSatToSun = rvCBToSun - x(1:3,1);
            fluxPressure = obj.flux/obj.speedOfLight;
            xddot       = -fluxPressure*obj.Spacecraft.Cr*obj.Spacecraft.SRPArea/1e3/...
                obj.Spacecraft.GetTotalMass()*rvSatToSun/norm(rvSatToSun)^3*obj.AU^2;
        end
        
    end
end
%             %%  Compute relativistic correction
%             if obj.UseSRP
%                 [accSRP,dadrSRP] = GetSRPAccel(t,X,Force,Sat);
%             else
%                 accSRP  = zeros(3,1);
%                 dadrSRP = zeros(3,3);
%             end
%
%             %% Compute drag Force
%             if strcmp(obj.Drag,'NRLMSISE') || strcmp(obj.Drag,'Exponential')
%
%                 %% Calculate lat lon height
%                 a1Epoch        = obj.Spacecraft.initialEpoch + t + 2430000;
%                 utcEpoch       = a1Epoch - 0.0343817/86400 - obj.deltaAT/86400;
%                 %[T_TT,JD_UT1]  = ConvertJD_UTC(utcEpoch,obj.deltaAT,obj.deltaUT1);
%                 [R,~,omega] = FK5( utcEpoch, obj.deltaAT, obj.deltaUT1,obj.lod,obj.xp,obj.yp,2);
%                 rvBodyFixed    = R*rv;
%                 [geolat,lon,height] = LatLonHeight(rvBodyFixed,obj.bodyRadius,obj.flattening);
%
%                 %% Calculate the density
%                 if strcmp(obj.Drag,'Exponential')
%                     [rho] = ExponentialAtmosphere(height,obj.ExponentialDensity.refHeight,...
%                         obj.ExponentialDensity.scaleHeight,...
%                         obj.ExponentialDensity.refDensity);
%                 elseif strcmp(obj.Drag,'NRLMSISE')
%                     [year,~,~]  = JD2GD(utcEpoch);
%                     sec           = ( (utcEpoch - .5) - fix(utcEpoch - 0.5))*86400  ;
%                     doy           = DayOfYear(utcEpoch);
%                     lst           = (sec/3600 + lon*180/pi/15);
%
%                     [apVector,F107file,F107Afile] = fileReader.GetApAndF107Data(utcEpoch);
%                     dragFlags = ones(23,1);
%                     dragFlags(9,1) = -1;
%                     [~, rho]      = atmosnrlmsise00(height*1000,geolat*180/pi,...
%                         lon*180/pi,year,doy,sec,lst,F107Afile,...
%                         F107file,apVector',dragFlags);
%
%                     rho           = rho(1,6)*1e9;
%                 end
%                 %  Compute the drag force
%                 vvrel         = vv - cross(omega,rv);
%                 vrel          = norm(vvrel);
%                 accDrag       = -0.5*obj.Spacecraft.Cd*obj.Spacecraft.DragArea/1e6/...
%                     obj.Spacecraft.DryMass*rho*vrel^2*vvrel/vrel;
%             else
%                 accDrag = zeros(3,1);
%             end


%             if obj.UseSTM  && obj.UseFiniteBurn == 0
%                 Xdot = [Xdot; reshape(STMdot,[36 1])];
%             elseif obj.UseSTM  && obj.UseFiniteBurn
%                 Xdot = [Xdot; massFlowTotal; reshape(STMdot,[36 1])];
%             elseif ~obj.UseSTM  && obj.UseFiniteBurn
%                 Xdot  = [Xdot; massFlowTotal];
%             end

%             %%  Compute relativistic correction
%             if obj.UseRelativity
%                 [accRelativity] = GetRelativityAccel(t,X,Force,Sat);
%             else
%                 accRelativity   = zeros(3,1);
%             end

%             %  Only configured for central body point mass and SRP.
%             if obj.UseSTM  == 1
%                 dvdr = zeros(3,3);
%                 dvdv = eye(3,3);
%                 %                A = [dvdr dvdv; (dadrTwoBody + dadrSRP) (dadvTwoBody + dadvSRP)];
%                 %                STMdot = A*reshape(X(7:42),[6 6]);
%             end