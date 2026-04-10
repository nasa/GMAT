%
% ------------------------------------------------------------------------------
%
%                           function ijk2lle
%
%  these subroutines convert a geocentric equatorial (ijk) position vector into
%    latitude and longitude.  geodetic and geocentric latitude are found.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    r           - ijk position vector            km
%    jd          - julian date                    days from 4713 bc
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
%    gstime      - greenwich sidereal time
%    gcgd        - converts between geocentric and geodetic latitude
%
%  references    :
%    vallado       2001, 174-179, alg 12 and alg 13, ex 3-3
%
% [latgc,latgd,lon,hellp] = ijk2ll ( r, jd );
% ------------------------------------------------------------------------------

function [latgc,latgd,lon,hellp] = ijk2lle ( r, jd );

        % -------------------------  implementation   -----------------
        twopi      =     2.0*pi;
        small      =     0.00000001;         % small value for tolerances
        eesqrd     =     0.006694385000;     % eccentricity of earth sqrd

        % -------------------  initialize values   --------------------
        magr = mag( r );
        oneminuse2 = 1.0  - eesqrd;

        % ---------------- find longitude value  ----------------------
        temp = sqrt( r(1)*r(1) + r(2)*r(2) );
        if ( abs( temp ) < small )
            rtasc= dsign(r(3))*pi*0.5;
          else
            rtasc= atan2( r(2) , r(1) );
          end
        gst  = gstime( jd );
        lon  = rtasc - gst;

        if ( abs(lon) >= pi )
            if ( lon < 0.0  )
                lon= twopi + lon;
              else
                lon= lon - twopi;
              end
          end

        % -------------- set up initial latitude value  ---------------
        decl    = asin( r(3) / magr );
        latgc= decl;
        deltalat= 100.0;
        rsqrd   = magr^2;

        % ---- iterate to find geocentric and geodetic latitude  -----
        i= 1;
        while ( ( abs( olddelta - deltalat ) >= small ) and ( i < 10 ))
            olddelta = deltalat;
            rsite    = sqrt( oneminuse2 / (1.0  - eesqrd*(cos(latgc))**2 ) );
            latgd    = atan( tan(latgc) / oneminuse2 );
            temp     = latgd-latgc;
            sintemp  = sin( temp );
            hellp    = sqrt( rsqrd - rsite*rsite*sintemp*sintemp ) - rsite*cos(temp);
            deltalat = asin( hellp*sintemp / magr );
            latgc = decl - deltalat;
            i = i + 1;
          end

        if ( i >= 10 )
            fprintf( 'ijktolatlon did not converge\n ');
          end

