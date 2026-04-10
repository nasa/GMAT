%
% ------------------------------------------------------------------------------
%
%                           function ijk2ll
%
%  these subroutines convert a geocentric equatorial position vector into
%    latitude and longitude.  geodetic and geocentric latitude are found. the
%    inputs must be ecef.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%    vallado     - fix jdut1 var name, add clarifying comments   26 aug 2002
%    vallado     - fix documentation for ecef                    19 jan 2005
%
%  inputs          description                    range / units
%    r           - ecef position vector           km
%
%  outputs       :
%    latgc       - geocentric latitude            -pi to pi rad
%    latgd       - geodetic latitude              -pi to pi rad
%    lon         - longitude (west -)             -2pi to 2pi rad
%    hellp       - height above the ellipsoid     km
%
%  locals        :
%    temp        - diff between geocentric/
%                  geodetic lat                   rad
%    sintemp     - sine of temp                   rad
%    olddelta    - previous value of deltalat     rad
%    rtasc       - right ascension                rad
%    decl        - declination                    rad
%    i           - index
%
%  coupling      :
%    mag         - magnitude of a vector
%    gcgd        - converts between geocentric and geodetic latitude
%
%  references    :
%    vallado       2001, 174-179, alg 12 and alg 13, ex 3-3
%
% [latgc,latgd,lon,hellp] = ijk2ll ( r );
% ------------------------------------------------------------------------------

function [latgc,latgd,lon,hellp] = ijk2ll ( r );

        twopi      =     2.0*pi;
        small      =     0.00000001;         % small value for tolerances
        re         =     6378.137;
        eesqrd     =     0.006694385000;     % eccentricity of earth sqrd

        % -------------------------  implementation   -----------------
        magr = mag( r );

        % ----------------- find longitude value  ---------------------
        temp = sqrt( r(1)*r(1) + r(2)*r(2) );
        if ( abs( temp ) < small )
            rtasc= sign(r(3))*pi*0.5;
          else
            rtasc= atan22( r(2), r(1) );
          end
        lon  = rtasc;
        if ( abs(lon) >= pi )   % mod it ?
            if ( lon < 0.0  )
                lon= twopi + lon;
            else
                lon= lon - twopi;
            end
        end
        decl = asin( r(3) / magr );
        latgd= decl;

        % ------------- iterate to find geodetic latitude -------------
        i= 1;
        olddelta = latgd + 10.0;

        while ((abs(olddelta-latgd)>=small) && (i<10))
            olddelta= latgd;
            sintemp = sin( latgd );
            c       = re  / (sqrt( 1.0 -eesqrd*sintemp*sintemp ));
            latgd= atan( (r(3)+c*eesqrd*sintemp)/temp );
            i = i + 1;
          end

        % Calculate height
        if (pi*0.5 - abs(latgd)) > pi/180.0  % 1 deg
            hellp   = (temp/cos(latgd)) - c;
          else
            s = c * (1.0 - eesqrd);
            hellp   = r(3)/sin(latgd) - s;
        end

        latgc = gd2gc(latgd);

