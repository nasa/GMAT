%
% ------------------------------------------------------------------------------
%
%                           function ijk2llb
%
%  these subroutines convert a geocentric equatorial (ijk) position vector into
%    latitude and longitude.  geodetic and geocentric latitude are found.
%
%  author        : david vallado                  719-573-2600    9 jun 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    r           - ijk position vector            km
%
%  outputs       :
%    latgc       - geocentric latitude            -pi to pi rad
%    latgd       - geodetic latitude              -pi to pi rad
%    lon         - longitude (west -)             -2pi to 2pi rad
%    hellp       - height above the ellipsoid     km
%
%  locals        :
%    rc          - range of site wrt earth center er
%    height      - height above earth wrt site    er
%    alpha       - angle from iaxis to point, lst rad
%    olddelta    - previous value of deltalat     rad
%    deltalat    - diff between delta and
%                  geocentric lat                 rad
%    delta       - declination angle of r in ijk  rad
%    rsqrd       - magnitude of r squared         er2
%    sintemp     - sine of temp                   rad
%    c           -
%
%  coupling      :
%    mag         - magnitude of a vector
%    gcgd        - converts between geocentric and geodetic latitude
%
%  references    :
%    vallado       2001, 174-179, alg 12 and alg 13, ex 3-3
%
% [latgc,latgd,lon,hellp] = ijk2llb ( r );
% ------------------------------------------------------------------------------

function [latgc,latgd,lon,hellp] = ijk2llb ( r );

        twopi =     2.0*pi;
        small =     0.00000001;

% -------------------------  implementation   -------------------------
        % ---------------- find longitude value  ----------------------
        temp = sqrt( r(1)*r(1) + r(2)*r(2) );
        if ( abs( temp ) < small )
            rtasc= dsign(r(3))*pi*0.5;
          else
            rtasc= atan2( r(2) , r(1) );
          end
        lon  = rtasc;
        if ( abs(lon) >= pi )
            if ( lon < 0.0  )
                lon= twopi + lon;
              else
                lon= lon - twopi;
              end
          end

        a= 6378.1363;
        b= sign(r(3)) * 6356.75160056;

        % -------------- set up initial latitude value  ---------------
        atemp= 1.0 /(a*temp);
        e= (b*r(3)-a*a+b*b)*atemp;
        f= (b*r(3)+a*a-b*b)*atemp;
        third= 1.0 /3.0;
        p= 4.0 *third*(e*f + 1.0  );
        q= 2.0 *(e*e - f*f);
        d= p*p*p + q*q;

        if ( d > 0.0  )
            nu= (sqrt(d)-q)^third - (sqrt(d)+q)^third;
          else
            sqrtp= sqrt(-p);
            nu= 2.0 *sqrtp*cos( third*acos(q/(p*sqrtp)) );
          end
        g= 0.5 *(sqrt(e*e + nu) + e);
        t= sqrt(g*g + (f-nu*g)/(2.0 *g-e)) - g;

        latgd= atan(a*(1.0 -t*t)/(2.0 *b*t));
        hellp= (temp-a*t)*cos( latgd) + (r(3)-b)*sin(latgd);

        latgc = gd2gc( latgd );

