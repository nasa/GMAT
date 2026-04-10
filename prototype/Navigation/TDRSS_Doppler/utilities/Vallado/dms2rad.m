% -----------------------------------------------------------------------------
%
%                           function dms2rad
%
%  this function converts degrees, minutes and seconds into radians.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    deg         - degrees                        0 .. 360
%    min         - minutes                        0 .. 59
%    sec         - seconds                        0.0 .. 59.99
%
%  outputs       :
%    dms         - result                         rad
%
%  locals        :
%    temp        - temporary variable
%
%  coupling      :
%    none.
%
%  references    :
%    vallado       2007, 203, alg 17 alg 18, ex 3-8
%
% [dms] = dms2rad( deg,min,sec );
% -----------------------------------------------------------------------------

function [dms] = dms2rad( deg,min,sec );

        deg2rad = pi/180.0;

        % ------------------------  implementation   ------------------
        dms = ( deg + min/60.0 + sec/3600.0 ) * deg2rad;

