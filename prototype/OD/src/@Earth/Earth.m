
classdef Earth < handle

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------

    %-----  Set the public data
    properties  (SetAccess = 'public')

        Radius                = 6378.1363;
        FlatteningCoefficient = 3.353642e-3;
        AngularVelocity       = 7.2921158553*(10^(-5));
        J2                    = 0.001082626683553;
        Mu                    = 398600.4415;

    end
    
    %-----  Set the public data
    properties  (SetAccess = 'protected')
        
        %  This data is for FK5 reduction using Vallado's code.  Much of
        %  the parameters should be read from a file because they are time
        %  varying.  I'm just getting this to work right now. 
        dut1     = -0.1142208;
        dat      = 33;
        xp       =  0.093609;  % arcseconds
        yp       =  0.486619;  % arcseconds
        lod      =  0.0015563;
        ddpsi    = -0.054137;  % "
        ddeps    = -0.007758;
        timezone =0;
        order    = 106;
        eqeterms = 2; % use the extra ee terms in j2000
        opt      = 'a';
        
    end


    %----------------------------------------------------------------------
    %  Define the object's methods
    %----------------------------------------------------------------------

    methods

        %----- Intialize
        function earth = Earth(earth)

        end % Initialize

        %----- Fixed2Inert
        function [R,Rdot] = Fixed2Inert(earth,jd)


            %  jd input is in UTC!!

            %  Time calculations
            jdut1 = jd + earth.dut1/86400;
            jdtai = jd + earth.dat/86400;
            jdtt  = jdtai + 32.184/86400;
            TT    = (jdtt - 2451545.0  )/ 36525.0;
            
            %  Calculate the precession
            [prec,psia,wa,ea,xa] = precess ( TT, '80' );

            %  Calculate the nutation
            ddpsi = earth.ddpsi*pi / (180*3600);  % rad
            ddeps = earth.ddeps*pi / (180*3600);
            [deltapsi,trueeps,meaneps,omega,nut] = nutation(TT,ddpsi,ddeps);

            %  Calculate the sidereal time
            [st,stdot] = sidereal(jdut1,deltapsi,meaneps,omega,earth.lod,earth.eqeterms );

            %  Calculate the polar motion
            [pm] = polarm(earth.xp,earth.yp,TT,'80');

            thetasa= 7.29211514670698e-05*(1.0  - earth.lod/86400.0 );
            omegaearth = [0; 0; thetasa;];

            R    = prec*nut*st*pm; 
            Rdot = prec*nut*stdot*pm;          
            
        end

    end % methods

end % classdef