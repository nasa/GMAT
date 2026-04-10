%
% ---------------------------------------------------------------------------
%
%                           function site
%
%  this function finds the position and velocity vectors for a site.  the
%    answer is returned in the geocentric equatorial (ecef) coordinate system.
%    note that the velocity is zero because the coordinate system is fixed to
%    the earth.
%
%  author        : david vallado                  719-573-2600   25 jun 2002
%
%  revisions
%    vallado     - fix velocity vector                           23 jul 2002
%
%  inputs          description                    range / units
%    latgd       - geodetic latitude              -pi/2 to pi/2 rad
%    lon         - longitude of site              -2pi to 2pi rad
%    alt         - altitude                       km
%
%  outputs       :
%    rs          - ecef site position vector      km
%    vs          - ecef site velocity vector      km/s
%
%  locals        :
%    sinlat      - variable containing  sin(lat)  rad
%    temp        - temporary real value
%    rdel        - rdel component of site vector  km
%    rk          - rk component of site vector    km
%    cearth      -
%
%  coupling      :
%    none
%
%  references    :
%    vallado       2001, 404-407, alg 47, ex 7-1
%
% [rs,vs] = site ( latgd,lon,alt );
% -----------------------------------------------------------------------------

function [rs,vs] = site ( latgd,lon,alt );

        constastro;

        % -------------------------  implementation   -----------------
        sinlat      = sin( latgd );

        % ------  find rdel and rk components of site vector  ---------
        cearth= re / sqrt( 1.0 - ( eccearthsqrd*sinlat*sinlat ) );
        rdel  = ( cearth + alt )*cos( latgd );
        rk    = ( (1.0-eccearthsqrd)*cearth + alt )*sinlat;

        % ---------------  find site position vector  -----------------
        rs(1) = rdel * cos( lon );
        rs(2) = rdel * sin( lon );
        rs(3) = rk;
        rs = rs';
        
        % ---------------  find site velocity vector  -----------------
        [vs] = [0.0; 0.0; 0.0];

