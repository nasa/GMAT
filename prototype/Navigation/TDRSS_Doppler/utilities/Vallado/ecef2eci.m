%
% ----------------------------------------------------------------------------
%
%                           function ecef2eci
%
%  this function transforms a vector from the earth fixed (itrf) frame, to
%    the eci mean equator mean equinox (j2000).
%
%  author        : david vallado                  719-573-2600    4 jun 2002
%
%  revisions
%    vallado     - add terms for ast calculation                 30 sep 2002
%    vallado     - consolidate with iau 2000                     14 feb 2005
%
%  inputs          description                    range / units
%    recef       - position vector earth fixed    km
%    vecef       - velocity vector earth fixed    km/s
%    aecef       - acceleration vector earth fixedkm/s2
%    ttt         - julian centuries of tt         centuries
%    jdut1       - julian date of ut1             days from 4713 bc
%    lod         - excess length of day           sec
%    xp          - polar motion coefficient       rad
%    yp          - polar motion coefficient       rad
%    eqeterms    - terms for ast calculation      0,2
%    ddpsi       - delta psi correction to gcrf   rad
%    ddeps       - delta eps correction to gcrf   rad
%
%  outputs       :
%    reci        - position vector eci            km
%    veci        - velocity vector eci            km/s
%    aeci        - acceleration vector eci        km/s2
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
%   precess      - rotation for precession       
%   nutation     - rotation for nutation          
%   sidereal     - rotation for sidereal time     
%   polarm       - rotation for polar motion      
%
%  references    :
%    vallado       2004, 219-228
%
% [reci,veci,aeci] = ecef2eci  ( recef,vecef,aecef,ttt,jdut1,lod,xp,yp,eqeterms,ddpsi,ddeps );
% ----------------------------------------------------------------------------

function [reci,veci,aeci] = ecef2eci  ( recef,vecef,aecef,ttt,jdut1,lod,xp,yp,eqeterms,ddpsi,ddeps );

        % ---- find matrices
        [prec,psia,wa,ea,xa] = precess ( ttt, '80' );

        [deltapsi,trueeps,meaneps,omega,nut] = nutation(ttt,ddpsi,ddeps);

        [st,stdot] = sidereal(jdut1,deltapsi,meaneps,omega,lod,eqeterms );

        [pm] = polarm(xp,yp,ttt,'80');

        % ---- perform transformations
        thetasa= 7.29211514670698e-05 * (1.0  - lod/86400.0 );
        omegaearth = [0; 0; thetasa;];

        rpef = pm*recef;
        reci = prec*nut*st*rpef;

        vpef = pm*vecef;
        veci = prec*nut*st*(vpef + cross(omegaearth,rpef));

        % veci1 = prec*nut * (stdot*recef + st*pm*vecef)  % alt approach using sidereal rate

        
        
        temp = cross(omegaearth,rpef);
        aeci = prec*nut*st*( pm*aecef + cross(omegaearth,temp) ...
               + 2.0*cross(omegaearth,vpef) );

