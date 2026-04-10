% ------------------------------------------------------------------------------
%
%                           function satfov
%
%  this function finds parameters relating to a satellite's fov.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    incl        - inclination                    rad
%    az          - azimuth                        rad
%    slatgd      - geodetic latitude of sat       rad
%    slon        - longitude of sat               rad
%    salt        - altitudeof satellite           er
%    tfov        - total field of view            rad
%    etactr      - ctr where sensor looks         rad
%
%  outputs       :
%    fovmax      - maximum field of view          rad
%    totalrng    -
%    rhomax      -
%    rhomin      -
%    tgtlat      -
%    tgtlon      -
%
%  locals        :
%    r           -
%    etahopriz   -
%    rhohoriz    -
%    gamma       -
%    rho         -
%    fovmin      -
%    lat         -
%    lon         -
%    maxlat      -
%    minlkat     -
%    i           - index
%
%  coupling      :
%    path        - finds tgt location given initial location, range, and az
%
%  references    :
%    vallado       2001, 776-781, eq 11-8 to eq 11-13, ex 11-1
%
% [totalrng, rhomax, rhomin,tgtlat,tgtlon] = ...
%  satfov ( incl,az, slatgd, slon, salt,tfov,etactr,fovmax );
% ------------------------------------------------------------------------------

function [rhomin, rhomax] = ...
         satfov ( incl, az, slatgd, slon, salt, tfov, etactr );

        rad2deg    =    180.0/pi;
        re = 6378.1363;
   
        % -------------------------  implementation   -----------------
        % ------- find satellite parameters and limiting cases --------
        r       = re  + salt;
        etahoriz= asin(re /r);
        rhohoriz= r*cos(etahoriz); % in km
   fprintf(1,'etahoriz %11.7f rhohoriz %11.7f km \n',etahoriz*rad2deg, rhohoriz);

        % ---------------- find ground range angle --------------------
        lambda = acos(re/r);
   fprintf(1,'lambda %11.7f  %11.7f km  \n',lambda*rad2deg, lambda*re);

   fprintf(1,'maximum locations \n');
        % -------- for maximum, if the sensor looks off axis ----------
            fovmin  = etactr + tfov*0.5;
            gamma   = pi - asin( r*sin(fovmin)/re );  % use larger angle
            rhomax  = re*cos( gamma ) + r*cos(fovmin);
   fprintf(1,'fovmin %11.7f gamma %11.7f gamma %11.7f rho %11.7f  \n',fovmin*rad2deg, gamma*rad2deg, (pi-gamma)*rad2deg, rhomax);
   
            % --------------------- slant range --------------------
            lambda  = asin(rhomax * sin(fovmin)/re);
            rhomin  = lambda * re; 
   fprintf(1,'lambda %11.7f rhomin %11.7f \n',lambda*rad2deg, rhomin );
 %         end

        % -------------- find location of center of fov ---------------
        if ( abs(etactr) > 0.00001  )
            [lat, lon] = pathm( slatgd, slon, lambda, az );
          else
            lat= slatgd;
            lon= slon;
        end

   fprintf(1,'max NS lat %11.7f lon %11.7f \n',(lat+lambda)*rad2deg, lon*rad2deg );
   fprintf(1,'min NS lat %11.7f lon %11.7f \n',(lat-lambda)*rad2deg, lon*rad2deg );
   fprintf(1,'max EW lat %11.7f lon %11.7f \n',lat*rad2deg, (lon+lambda)*rad2deg );
   fprintf(1,'min EW lat %11.7f lon %11.7f \n',lat*rad2deg, (lon-lambda)*rad2deg );
        
        

