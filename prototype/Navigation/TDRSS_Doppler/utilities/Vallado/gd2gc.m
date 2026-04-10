% ------------------------------------------------------------------------------
%
%                           function gd2gc
%
%  this function converts from geodetic to geocentric latitude for positions
%    on the surface of the earth.  notice that (1-f) squared = 1-esqrd.
%
%  author        : david vallado                  719-573-2600   30 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    latgd       - geodetic latitude              -pi to pi rad
%
%  outputs       :
%    latgc       - geocentric latitude            -pi to pi rad
%
%  locals        :
%    none.
%
%  coupling      :
%    none.
%
%  references    :
%    vallado       2001, 146, eq 3-11
%
% [latgc] = gd2gc ( latgd );
% ------------------------------------------------------------------------------

function [latgc] = gd2gc ( latgd );

        eesqrd = 0.006694385000;     % eccentricity of earth sqrd

        % -------------------------  implementation   -----------------
        latgc= atan( (1.0  - eesqrd)*tan(latgd) );

