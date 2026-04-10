classdef ForceModel < handle
    
    properties (SetAccess = 'public')
        
        %  Flags to determine which thusters to use
        useSTM        = 0;
        useSRP        = 0;
        UseRelativity = 0;
        Debug         = 0;
        Drag          = 'None';
        
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
    end
    
    methods
        
        function [Xdot] = OrbitForce(t,X,dummy, Force,Sat)
            
            if ~isempty(dummy)
                Sat   = Force;
                Force = dummy;
            end
            
            % persistent weatherfileReader
            %
            % if ~exist(weatherfileReader)
            %     fileReader = WeatherFileReader;
            %     fileReader.Initialize();
            %     weatherFileReader = 1;
            % end
            %% Initializations
            rv = X(1:3,1);
            vv = X(4:6,1);
            r  = norm(rv);
            
            %  Update objects based on state vector
            Sat.OrbitState = [rv;vv];
            Sat.Epoch      = Force.RefEpoch + t/86400;
            if Force.UseFiniteBurn
                Sat.SetTankMass(X(7,1));
            end
            
            %% Compute two Body Force
            accTwoBody  = -Force.bodyMu /r^3*rv;
            dadrTwoBody = (-Force.bodyMu/r^3*eye(3) + 3*Force.bodyMu/r^5*rv*rv');
            dadvTwoBody = zeros(3,3);
            
            %% Compute drag Force
            if strcmp(Force.Drag,'NRLMSISE') || strcmp(Force.Drag,'Exponential')
                
                %% Calculate lat lon height
                a1Epoch        = Sat.initialEpoch + t/86400 + 2430000;
                utcEpoch       = a1Epoch - 0.0343817/86400 - Force.deltaAT/86400;
                [T_TT,JD_UT1]  = ConvertJD_UTC(utcEpoch,Force.deltaAT,Force.deltaUT1);
                [R,Rdot,omega] = FK5( utcEpoch, Force.deltaAT, Force.deltaUT1,Force.lod,Force.xp,Force.yp,2);
                rvBodyFixed    = R*rv;
                [geolat,lon,height] = LatLonHeight(rvBodyFixed,Force.bodyRadius,Force.flattening);
                
                %% Calculate the density
                if strcmp(Force.Drag,'Exponential')
                    [rho] = ExponentialAtmosphere(height,Force.ExponentialDensity.refHeight,...
                        Force.ExponentialDensity.scaleHeight,...
                        Force.ExponentialDensity.refDensity);
                elseif strcmp(Force.Drag,'NRLMSISE')
                    [year,month,day]  = JD2GD(utcEpoch);
                    sec           = ( (utcEpoch - .5) - fix(utcEpoch - 0.5))*86400  ;
                    doy           = DayOfYear(utcEpoch);
                    lst           = (sec/3600 + lon*180/pi/15);
                    
                    [apVector,F107,F107A] = fileReader.GetApAndF107Data(utcEpoch);
                    dragFlags = ones(23,1);
                    dragFlags(9,1) = -1;
                    [T, rho]      = atmosnrlmsise00(height*1000,geolat*180/pi,...
                        lon*180/pi,year,doy,sec,lst,F107A,...
                        F107,apVector',dragFlags);
                    
                    %[T, rho]      = atmosnrlmsise00(height*1000,geolat*180/pi,...
                    %                 lon*180/pi,year,doy,sec,lst,Force.MSISE.F107A,...
                    %                 Force.MSISE.F107,Force.MSISE.Ap);
                    
                    rho           = rho(1,6)*1e9;
                end
                %  Compute the drag force
                vvrel         = vv - cross(omega,rv);
                vrel          = norm(vvrel);
                accDrag       = -0.5*Sat.Cd*Sat.DragArea/1e6/...
                    Sat.DryMass*rho*vrel^2*vvrel/vrel;
            else
                accDrag = zeros(3,1);
            end
            
            %%  Compute relativistic correction
            if Force.UseRelativity
                [accRelativity] = GetRelativityAccel(t,X,Force,Sat);
            else
                accRelativity   = zeros(3,1);
            end
            
            %%  Compute relativistic correction
            if Force.UseSRP
                [accSRP,dadrSRP,dadvSRP] = GetSRPAccel(t,X,Force,Sat);
            else
                accSRP  = zeros(3,1);
                dadrSRP = zeros(3,3);
                dadvSRP = zeros(3,3);
            end
            
            %% Compute thrust force
            accFiniteBurn = zeros(3,1);
            massFlowTotal = 0;
            if Force.UseFiniteBurn
                powerAvailable = Sat.GetThrustPower();
                [thrustVector,massFlowTotal]= ...
                    Sat.ThrusterSet.GetThrustAndMassFlowRate(powerAvailable);
                accFiniteBurn = thrustVector/Sat.GetTotalMass();
                if Force.debugMath
                    disp(['======== Acceleration Debug Data ========']);
                    disp(['Acc. Vec    X   : ' num2str(accFiniteBurn(1),16)]);
                    disp(['Acc. Vec    Y   : ' num2str(accFiniteBurn(2),16)]);
                    disp(['Acc. Vec    Z   : ' num2str(accFiniteBurn(3),16)]);
                end
            end
            %accFiniteBurn = accFiniteBurn*1000;
            %massFlowTotal = massFlowTotal*0;
            
            %%  Sum the forces
            Xdot(1:3,1) = vv;
            Xdot(4:6,1) = (accTwoBody + accDrag + accRelativity +...
                accSRP + accFiniteBurn);
            
            %  Only configured for central body point mass and SRP.
            if Force.STM == 1
                dvdr = zeros(3,3);
                dvdv = eye(3,3);
                A = [dvdr dvdv; (dadrTwoBody + dadrSRP) (dadvTwoBody + dadvSRP)];
                STMdot = A*reshape(X(7:42),[6 6]);
            end
            
            if Force.STM == 1 && Force.UseFiniteBurn == 0
                Xdot = [Xdot; reshape(STMdot,[36 1])];
            elseif     Force.STM  && Force.UseFiniteBurn
                Xdot = [Xdot; massFlowTotal; reshape(STMdot,[36 1])];
            elseif ~Force.STM  && Force.UseFiniteBurn
                Xdot  = [Xdot; massFlowTotal];
            end
            
            
        end
    end
end
