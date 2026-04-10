%
% ----------------------------------------------------------------------------
%
%                           function eci2teme
%
%  this function transforms a vector from the mean equator mean equinox frame
%    (j2000) to the true equator mean equinox of date (teme).
%
%  author        : david vallado                  719-573-2600   21 jun 2002
%
%  revisions
%    vallado     - add order                                     29 sep 2002
%
%  inputs          description                    range / units
%    reci        - position vector eci            km
%    veci        - velocity vector eci            km/s
%    aeci        - acceleration vector eci        km/s2
%    ttt         - julian centuries of tt         centuries
%    order       - number of terms for nutation   4, 50, 106, ...
%    eqeterms    - number of terms for eqe        0, 2
%    opt         - option for processing          a - complete nutation
%                                                 b - truncated nutation
%                                                 c - truncated transf matrix
%
%  outputs       :
%    rteme       - position vector of date
%                    true equator, mean equinox   km
%    vteme       - velocity vector of date
%                    true equator, mean equinox   km/s
%    ateme       - acceleration vector of date
%                    true equator, mean equinox   km/s2
%
%  locals        :
%    prec        - matrix for eci - mod
%    nutteme     - matrix for mod - teme
%    tm          - combined matrix for teme
%
%  coupling      :
%   precess      - rotation for precession        eci - mod
%   truemean     - rotation for truemean          mod - teme
%
%  references    :
%    vallado       2001, 216-219, eq 3-654
%
% [rteme,vteme,ateme] = eci2teme  ( reci,veci,aeci,ttt,order,eqeterms,opt );
% ----------------------------------------------------------------------------

function [rteme,vteme,ateme] = eci2teme  ( reci,veci,aeci,ttt,order,eqeterms,opt );

        [prec,psia,wa,ea,xa] = precess ( ttt, '80' );

        [nutteme] = truemean(ttt,order,eqeterms,opt);

        rteme = nutteme'*prec'*reci;

        vteme = nutteme'*prec'*veci;

        ateme = nutteme'*prec'*aeci;

