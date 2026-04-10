classdef ShuttleReEntryPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            global auxdata
            
            % Extract the state and control
            state = obj.GetStateVec();
            control = obj.GetControlVec();
            time = obj.GetTime();
            
            rad = state(1);
            lon = state(2);
            lat = state(3);
            speed = state(4);
            fpa = state(5);
            azimuth = state(6);
            aoa = control(1);
            bank = control(2);
            
            cd0 = auxdata.cd(1);
            cd1 = auxdata.cd(2);
            cd2 = auxdata.cd(3);
            cl0 = auxdata.cl(1);
            cl1 = auxdata.cl(2);
            mu  = auxdata.mu;
            rho0 = auxdata.rho0;
            H = auxdata.H;
            S = auxdata.S;
            mass = auxdata.mass;
            altitude = rad - auxdata.Re;
            
            CD = cd0+cd1*aoa+cd2*aoa.^2;
            
            rho = rho0*exp(-altitude/H);
            CL = cl0+cl1*aoa;
            gravity = mu./rad.^2;
            dynamic_pressure = 0.5*rho.*speed.^2;
            D = dynamic_pressure.*S.*CD./mass;
            L = dynamic_pressure.*S.*CL./mass;
            slon = sin(lon);
            clon = cos(lon);
            slat = sin(lat);
            clat = cos(lat);
            tlat = tan(lat);
            sfpa = sin(fpa);
            cfpa = cos(fpa);
            sazi = sin(azimuth);
            cazi = cos(azimuth);
            cbank = cos(bank);
            sbank = sin(bank);
            
            raddot   = speed.*sfpa;
            londot   = speed.*cfpa.*sazi./(rad.*clat);
            latdot   = speed.*cfpa.*cazi./rad;
            speeddot = -D-gravity.*sfpa;
            fpadot   = (L.*cbank-cfpa.*(gravity-speed.^2./rad))./speed;
            azidot   = (L.*sbank./cfpa + speed.^2.*cfpa.*sazi.*tlat./rad)./speed;

            obj.SetDynFunctions([raddot, londot, latdot, speeddot, fpadot, azidot]');
            
        end
        
        function EvaluateJacobians(obj)

        end
    end
    
end



