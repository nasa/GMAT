% ----------------------------------------------------------------------------
%
%                           function ecef2teme
%
%  this function trsnforms a vector from the earth fixed (ITRF) frame to the 
%    true equator mean equniox frame (teme). the results take into account
%    the effects of sidereal time, and polar motion.
%
%  author        : david vallado                  719-573-2600   10 dec 2007
%
%  revisions
%
%  inputs          description                    range / units
%    recef       - position vector earth fixed    km
%    vecef       - velocity vector earth fixed    km/s
%    aecef       - acceleration vector earth fixedkm/s2
%    lod         - excess length of day           sec
%    ttt         - julian centuries of tt         centuries
%    jdut1       - julian date of ut1             days from 4713 bc
%    xp          - polar motion coefficient       arc sec
%    yp          - polar motion coefficient       arc sec
%
%  outputs       :
%    rteme       - position vector teme           km
%    vteme       - velocity vector teme           km/s
%    ateme       - acceleration vector teme       km/s2
%
%  locals        :
%    st          - matrix for pef - tod 
%    pm          - matrix for ecef - pef 
%
%  coupling      :
%   gstime       - greenwich mean sidereal time   rad
%   polarm       - rotation for polar motion      pef - ecef
%
%  references    :
%    vallado       2007, 219-228
%
% [rteme, vteme, ateme] = ecef2teme( recef, vecef, aecef, ttt, jdut1, lod, xp, yp );
% ----------------------------------------------------------------------------

function [rteme, vteme, ateme] = ecef2teme( recef, vecef, aecef, ttt, jdut1, lod, xp, yp );


        % ------------------------ find gmst --------------------------
        gmst= gstime( jdut1 );

        thetasa    = 7.29211514670698e-05 * (1.0  - lod/86400.0 );
        omegaearth = thetasa;

        st(1,1) =  cos(gmst);
        st(1,2) = -sin(gmst);
        st(1,3) =  0.0;
        st(2,1) =  sin(gmst);
        st(2,2) =  cos(gmst);
        st(2,3) =  0.0;
        st(3,1) =  0.0;
        st(3,2) =  0.0;
        st(3,3) =  1.0;

        [pm] = polarm(xp,yp,ttt,'80');

        thetasa= 7.29211514670698e-05 * (1.0  - lod/86400.0 );
        omegaearth = [0; 0; thetasa;];

        rpef = pm*recef;
        rteme = st*rpef;

        vpef = pm*vecef;
        vteme = st*(vpef + cross(omegaearth,rpef));

        temp = cross(omegaearth,rpef);
        ateme = st*( pm*aecef + cross(omegaearth,temp) ...
               + 2.0*cross(omegaearth,vpef) );


