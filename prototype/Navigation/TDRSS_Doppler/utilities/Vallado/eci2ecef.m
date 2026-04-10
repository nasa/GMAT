%
% ----------------------------------------------------------------------------
%
%                           function eci2ecef
%
%  this function trsnforms a vector from the mean equator mean equniox frame
%    (j2000), to an earth fixed (ITRF) frame.  the results take into account
%    the effects of precession, nutation, sidereal time, and polar motion.
%
%  author        : david vallado                  719-573-2600   27 jun 2002
%
%  revisions
%    vallado     - add terms for ast calculation                 30 sep 2002
%    vallado     - consolidate with iau 2000                     14 feb 2005
%
%  inputs          description                    range / units
%    reci        - position vector eci            km
%    veci        - velocity vector eci            km/s
%    aeci        - acceleration vector eci        km/s2
%    ttt         - julian centuries of tt         centuries
%    jdut1       - julian date of ut1             days from 4713 bc
%    lod         - excess length of day           sec
%    xp          - polar motion coefficient       arc sec
%    yp          - polar motion coefficient       arc sec
%    eqeterms    - terms for ast calculation      0,2
%    ddpsi       - delta psi correction to gcrf   rad
%    ddeps       - delta eps correction to gcrf   rad
%
%  outputs       :
%    recef       - position vector earth fixed    km
%    vecef       - velocity vector earth fixed    km/s
%    aecef       - acceleration vector earth fixedkm/s2
%
%  locals        :
%    deltapsi    - nutation angle                 rad
%    trueeps     - true obliquity of the ecliptic rad
%    meaneps     - mean obliquity of the ecliptic rad
%    omega       -                                rad
%    prec        - matrix for mod - eci 
%    nut         - matrix for tod - mod 
%    st          - matrix for pef - tod 
%    stdot       - matrix for pef - tod rate
%    pm          - matrix for ecef - pef 
%
%  coupling      :
%   precess      - rotation for precession        eci - mod
%   nutation     - rotation for nutation          mod - tod
%   sidereal     - rotation for sidereal time     tod - pef
%   polarm       - rotation for polar motion      pef - ecef
%
%  references    :
%    vallado       2004, 219-228
%
% [recef,vecef,aecef] = eci2ecef  ( reci,veci,aeci,ttt,jdut1,lod,xp,yp,eqeterms,ddpsi,ddeps );
% ----------------------------------------------------------------------------

function [recef,vecef,aecef] = eci2ecef  ( reci,veci,aeci,ttt,jdut1,lod,xp,yp,eqeterms,ddpsi,ddeps );

        [prec,psia,wa,ea,xa] = precess ( ttt, '80' );

        [deltapsi,trueeps,meaneps,omega,nut] = nutation(ttt,ddpsi,ddeps);

        [st,stdot] = sidereal(jdut1,deltapsi,meaneps,omega,lod,eqeterms );

        [pm] = polarm(xp,yp,ttt,'80');

        thetasa= 7.29211514670698e-05 * (1.0  - lod/86400.0 );
        omegaearth = [0; 0; thetasa;];

        rpef  = st'*nut'*prec'*reci;
        recef = pm'*rpef;

        vpef  = st'*nut'*prec'*veci - cross( omegaearth,rpef );
        vecef = pm'*vpef;

        temp  = cross(omegaearth,rpef);

        aecef = pm'*(st'*nut'*prec'*aeci - cross(omegaearth,temp) ...
                - 2.0*cross(omegaearth,vpef));

