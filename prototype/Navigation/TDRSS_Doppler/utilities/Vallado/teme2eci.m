% ----------------------------------------------------------------------------
%
%                           function teme2eci
%
%  this function transforms a vector from the true equator mean equinox system,
%    (teme) to the mean equator mean equinox (j2000) system.
%
%  author        : david vallado                  719-573-2600   25 jun 2002
%
%  revisions
%    vallado     - add order                                     29 sep 2002
%
%  inputs          description                    range / units
%    rteme       - position vector of date
%                    true equator, mean equinox   km
%    vteme       - velocity vector of date
%                    true equator, mean equinox   km/s
%    ateme       - acceleration vector of date
%                    true equator, mean equinox   km/s2
%    ttt         - julian centuries of tt         centuries
%    eqeterms       - number of terms for eqe        0, 2
%    opt         - option for processing          a - complete nutation
%                                                 b - truncated nutation
%                                                 c - truncated transf matrix
%
%  outputs       :
%    reci        - position vector eci            km
%    veci        - velocity vector eci            km/s
%    aeci        - acceleration vector eci        km/s2
%
%  locals        :
%    prec        - matrix for eci - mod
%    nutteme     - matrix for mod - teme - an approximation for nutation
%    tm          - combined matrix for teme
%
%  coupling      :
%   precess      - rotation for precession        eci - mod
%   truemean     - rotation for truemean          eci - teme
%
%  references    :
%    vallado       2001, 216-219, eq 3-654
%
% [reci,veci,aeci] = teme2eci  ( rteme,vteme,ateme,ttt,order,eqeterms,opt );
% ----------------------------------------------------------------------------

function [reci,veci,aeci] = teme2eci  ( rteme,vteme,ateme,ttt,order,eqeterms,opt );

        [prec,psia,wa,ea,xa] = precess ( ttt, '80' );

        [deltapsi,trueeps,meaneps,omega,eqe,nutteme] = truemean ( ttt,order,eqeterms,opt );

        reci = prec * nutteme * rteme;

        veci = prec * nutteme * vteme;

        aeci = prec * nutteme * ateme;

