% ------------------------------------------------------------------------------
%
%                           function rngaz
%
%  this function calculates the range and azimuth between two specified
%    ground points on a spherical earth.  notice the range will always be
%    within the range of values listed since you for not know the direction of
%    firing, long or short.  the function will calculate rotating earth ranges
%    if the tof is passed in other than 0.0 . range is calulated in rad and
%    converted to er by s = ro, but the radius of the earth = 1 er, so it's
%    s = o.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                - dav 31 may 06 add elliptical model
%
%  inputs          description                    range / units
%    llat        - start geocentric latitude      -pi/2 to  pi/2 rad
%    llon        - start longitude (west -)       0.0  to 2pi rad
%    tlat        - end geocentric latitude        -pi/2 to  pi/2 rad
%    tlon        - end longitude (west -)         0.0  to 2pi rad
%    tof         - time of flight if icbm, or 0.0 min
%
%  outputs       :
%    range       - range between points           km
%    az          - azimuth                        0.0  to 2pi rad
%
%  locals        :
%    none.
%
%  coupling      :
%    site, rot3, binomial, cross, atan2, dot, unit
%
%  references    :
%    vallado       2001, 774-775, eq 11-3, eq 11-4, eq 11-5
%
% [range,az] = rngaz ( llat,llon,tlat,tlon,tof );
% rad = 180/pi;
% [range, az] = rngaz( 60.0/rad, -80.0/rad ,41.0/rad, 74.0/rad, 0);
% ------------------------------------------------------------------------------

function [range,az] = rngaz ( llat,llon,tlat,tlon,tof );

        twopi      = 2.0*pi;
        small      = 0.00000001;
        omegaearth = 0.05883359221938136;
% fix units on tof and omegaearth

        % -------------------------  implementation   -------------------------
        range= acos( sin(llat)*sin(tlat) + ...
              cos(llat)*cos(tlat)*cos(tlon-llon + omegaearth*tof) );

        % ------ check if the range is 0 or half the earth  ---------
        if ( abs( sin(range)*cos(llat) ) < small )
            if ( abs( range - pi ) < small )
                az= pi;
              else
                az= 0.0;
              end
          else
            az= acos( ( sin(tlat) - cos(range) * sin(llat)) / ...
                      ( sin(range) * cos(llat)) );
          end

        % ------ check if the azimuth is grt than pi ( 180deg ) -------
        if ( sin( tlon - llon + omegaearth*tof ) < 0.0  )
            az= twopi - az;
        end

        fprintf(1,'spehrical rnage %11.7f km az %11.7f \n',range*6378.1363,az*180/pi);
        
        
        
        rad = 180/pi;
        % test ellipsoidal approach
        alt = 0.0;
        [rlch,vlch] = site ( llat,llon,alt );
        [rtgt,vtgt] = site ( tlat,tlon,alt );
        fprintf(1,'U rlch %11.7f %11.7f  %11.7f \n',rlch(1),rlch(2),rlch(3) );
        fprintf(1,'  rtgt %11.7f %11.7f  %11.7f \n',rtgt(1),rtgt(2),rtgt(3) );
        
        [rlu] = unit ( rlch ); % his u vector
        [rtu] = unit ( rtgt );
        
        fprintf(1,'u rlu %11.7f %11.7f  %11.7f \n',rlu(1),rlu(2),rlu(3) );
        fprintf(1,'  rtu %11.7f %11.7f  %11.7f \n',rtu(1),rtu(2),rtu(3) );
        
        rp=6356.0; % polar radius
        re = 6378.137; %equatorial radius
        delta = re*re/(rp^2) - 1.0

        eps = delta*(rlu(3)^2 + rtu(3)^2);
        
        w = cross(rlu,rtu);
        fprintf(1,'w UxV %11.7f %11.7f  %11.7f \n',w(1),w(2),w(3) );
        if (w(3) < 0.0)
            w = cross(rtu,rlu); % switch order
            fprintf(1,'w UxV %11.7f %11.7f  %11.7f \n',w(1),w(2),w(3) );
        end;
        v = cross(rlu,w);
        fprintf(1,'v uxw %11.7f %11.7f  %11.7f \n',v(1),v(2),v(3) );
        
        phi = pi- 0.5*atan2(-2*v(3)*rlu(3), v(3)^2-rlu(3)^2); % use to get angle from just xy
        fprintf(1,'phi %11.7f %11.7f \n',phi, phi*rad);

%        phi = 0.5*atan2(-2*.47024*.86603, .47024^2-.86603^2); 
%        fprintf(1,'phi %11.7f %11.7f \n',phi, phi*rad);
 
         temp = [dot(rlch,rlu) dot(rlch,rtu) 0.0];
         [uprime] = rot3 ( temp, phi )/6378.137; % he uses cannonical units
         fprintf(1,'uprime %11.7f %11.7f  %11.7f \n',uprime(1),uprime(2),uprime(3) );        
         phi1 = 2*pi + atan2( uprime(2)*sqrt(1 + eps), uprime(1));
         fprintf(1,'phi1 %11.7f %11.7f \n',phi1, phi1*rad);          
         
         temp = [dot(rtgt,rlu) dot(rtgt,rtu) 0.0];
         [vprime] = rot3 ( temp, phi )/6378.137; % he uses cannonical units
         fprintf(1,'vprime %11.7f %11.7f  %11.7f \n',vprime(1),vprime(2),vprime(3) );         
         phi2 = 2*pi + atan2( vprime(2)*sqrt(1 + eps), vprime(1));
         fprintf(1,'phi1 %11.7f %11.7f \n',phi2, phi2*rad);          
         
         e = 0.08181922; 
         % do each half of integral evaluation
         phi = phi2
         m = 1;
         r = 0;
         s1 = ( factorial(2*m)*(factorial(r))^2 ) / (2^(2*m-2*r)*factorial(2*r+1)*(factorial(m))^2 ) * (cos(phi)^(2*r+1));
         f1 = binomial(2*m, m)*phi/2^(2*m) + sin(phi)*s1;
         r = 0;
         m = 2;
         s1 = ( factorial(2*m)*(factorial(r))^2 ) / (2^(2*m-2*r)*factorial(2*r+1)*(factorial(m))^2 ) * (cos(phi)^(2*r+1));         
         r = 1;
         s2 = ( factorial(2*m)*(factorial(r))^2 ) / (2^(2*m-2*r)*factorial(2*r+1)*(factorial(m))^2 ) * (cos(phi)^(2*r+1));
         f2 = binomial(2*m, m)*phi/2^(2*m) + sin(phi)*(s1+s2);
         funct2 = (1.0 - e^2/2*f1 - binomial(2*m-3, m-2) * (e^2*f2)/(m*2^(2*m-2)) );
          
         phi = phi1
         m = 1;
         r = 0;
         s1 = ( factorial(2*m)*(factorial(r))^2 ) / (2^(2*m-2*r)*factorial(2*r+1)*(factorial(m))^2 ) * (cos(phi)^(2*r+1));
         f1 = binomial(2*m, m)*phi/2^(2*m) + sin(phi)*s1;
         r = 0;
         m = 2;
         s1 = ( factorial(2*m)*(factorial(r))^2 ) / (2^(2*m-2*r)*factorial(2*r+1)*(factorial(m))^2 ) * (cos(phi)^(2*r+1));         
         r = 1;
         s2 = ( factorial(2*m)*(factorial(r))^2 ) / (2^(2*m-2*r)*factorial(2*r+1)*(factorial(m))^2 ) * (cos(phi)^(2*r+1));
         f2 = binomial(2*m, m)*phi/2^(2*m) + sin(phi)*(s1+s2);
         funct1 = (1.0 - e^2/2*f1 - binomial(2*m-3, m-2) * (e^2*f2)/(m*2^(2*m-2)) );

         range = (funct2 - funct1)*re

        
        
        
        
        
        
          
        
        
