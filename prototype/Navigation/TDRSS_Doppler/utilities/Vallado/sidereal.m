%
% ----------------------------------------------------------------------------
%
%                           function sidereal
%
%  this function calulates the transformation matrix that accounts for the
%    effects of sidereal time. Notice that deltaspi should not be moded to a
%    positive number because it is multiplied rather than used in a
%    trigonometric argument.
%
%  author        : david vallado                  719-573-2600   25 jun 2002
%
%  revisions
%    vallado     - fix units on kinematic terms                   5 sep 2002
%    vallado     - add terms                                     30 sep 2002
%    vallado     - consolidate with iau 2000                     14 feb 2005
%
%  inputs          description                    range / units
%    jdut1       - julian centuries of ut1        days
%    deltapsi    - nutation angle                 rad
%    meaneps     - mean obliquity of the ecliptic rad
%    omega       - long of asc node of moon       rad
%    lod         - length of day                  sec
%    eqeterms    - terms for ast calculation      0,2
%
%  outputs       :
%    st          - transformation matrix for pef - tod
%    stdot       - transformation matrix for pef - tod rate
%
%  locals        :
%    gmst         - mean greenwich sidereal time   0 to 2pi rad
%    ast         - apparent gmst                   0 to 2pi rad
%    hr          - hour                           hr
%    min         - minutes                        min
%    sec         - seconds                        sec
%    temp        - temporary vector
%    tempval     - temporary variable
%
%  coupling      :
%
%  references    :
%    vallado       2004, 222-224
%
% [st,stdot]  = sidereal (jdut1,deltapsi,meaneps,omega,lod,eqeterms );
% ----------------------------------------------------------------------------

function [st,stdot]  = sidereal (jdut1,deltapsi,meaneps,omega,lod,eqeterms );

        % ------------------------ find gmst --------------------------
        gmst= gstime( jdut1 );

        % ------------------------ find mean ast ----------------------
        if (jdut1 > 2450449.5 ) & (eqeterms > 0)
            ast= gmst + deltapsi* cos(meaneps) ...
                + 0.00264*pi /(3600*180)*sin(omega) ...
                + 0.000063*pi /(3600*180)*sin(2.0 *omega);
          else
            ast= gmst + deltapsi* cos(meaneps);
          end

        ast = rem (ast,2*pi);
        thetasa    = 7.29211514670698e-05 * (1.0  - lod/86400.0 );
        omegaearth = thetasa;

%fprintf(1,'st gmst %11.7f ast %11.7f ome  %11.7f \n',gmst*180/pi,ast*180/pi,omegaearth*180/pi );

        st(1,1) =  cos(ast);
        st(1,2) = -sin(ast);
        st(1,3) =  0.0;
        st(2,1) =  sin(ast);
        st(2,2) =  cos(ast);
        st(2,3) =  0.0;
        st(3,1) =  0.0;
        st(3,2) =  0.0;
        st(3,3) =  1.0;

        % compute sidereal time rate matrix
        stdot(1,1) = -omegaearth * sin(ast);
        stdot(1,2) = -omegaearth * cos(ast);
        stdot(1,3) =  0.0;
        stdot(2,1) =  omegaearth * cos(ast);
        stdot(2,2) = -omegaearth * sin(ast);
        stdot(2,3) =  0.0;
        stdot(3,1) =  0.0;
        stdot(3,2) =  0.0;
        stdot(3,3) =  0.0;

