%
% ----------------------------------------------------------------------------
%
%                           function nutation
%
%  this function calulates the transformation matrix that accounts for the
%    effects of nutation.
%
%  author        : david vallado                  719-573-2600   27 jun 2002
%
%  revisions
%    vallado     - consolidate with iau 2000                     14 feb 2005
%
%  inputs          description                    range / units
%    ttt         - julian centuries of tt
%    ddpsi       - delta psi correction to gcrf   rad
%    ddeps       - delta eps correction to gcrf   rad
%
%  outputs       :
%    deltapsi    - nutation angle                 rad
%    trueeps     - true obliquity of the ecliptic rad
%    meaneps     - mean obliquity of the ecliptic rad
%    omega       -                                rad
%    nut         - transformation matrix for tod - mod
%
%  locals        :
%    iar80       - integers for fk5 1980
%    rar80       - reals for fk5 1980
%    ttt2        - ttt squared
%    ttt3        - ttt cubed
%    l           -                                rad
%    ll          -                                rad
%    f           -                                rad
%    d           -                                rad
%    deltaeps    - change in obliquity            rad
%
%  coupling      :
%    fundarg     - find fundamental arguments
%
%  references    :
%    vallado       2004, 221-222
%
% [deltapsi, trueeps, meaneps, omega,nut] = nutation  (ttt,ddpsi,ddeps );
% ----------------------------------------------------------------------------

function [deltapsi, trueeps, meaneps, omega,nut] = nutation  (ttt,ddpsi,ddeps );

        deg2rad = pi/180.0;

        [iar80,rar80] = iau80in;  % coeff in deg

        % ---- determine coefficients for iau 1980 nutation theory ----
        ttt2= ttt*ttt;
        ttt3= ttt2*ttt;

        meaneps = -46.8150 *ttt - 0.00059 *ttt2 + 0.001813 *ttt3 + 84381.448;
        meaneps = rem( meaneps/3600.0 ,360.0  );
        meaneps = meaneps * deg2rad;

        [ l, l1, f, d, omega, ...
          lonmer, lonven, lonear, lonmar, lonjup, lonsat, lonurn, lonnep, precrate ...
        ] = fundarg( ttt, '80' );
        %fprintf(1,'nut del arg %11.7f  %11.7f  %11.7f  %11.7f  %11.7f  \n',l*180/pi,l1*180/pi,f*180/pi,d*180/pi,omega*180/pi );

        deltapsi= 0.0;
        deltaeps= 0.0;
        for i= 106:-1: 1
            tempval= iar80(i,1)*l + iar80(i,2)*l1 + iar80(i,3)*f + ...
                     iar80(i,4)*d + iar80(i,5)*omega;
            deltapsi= deltapsi + (rar80(i,1)+rar80(i,2)*ttt) * sin( tempval );
            deltaeps= deltaeps + (rar80(i,3)+rar80(i,4)*ttt) * cos( tempval );
          end

        % --------------- find nutation parameters --------------------
        deltapsi = rem( deltapsi + ddpsi, 2.0 * pi );
        deltaeps = rem( deltaeps + ddeps, 2.0 * pi );
        trueeps  = meaneps + deltaeps;

%fprintf(1,'meaneps %11.7f dp  %11.7f de  %11.7f te  %11.7f  ttt  %11.7f \n',meaneps*180/pi,deltapsi*180/pi,deltaeps*180/pi,trueeps*180/pi, ttt );

        cospsi  = cos(deltapsi);
        sinpsi  = sin(deltapsi);
        coseps  = cos(meaneps);
        sineps  = sin(meaneps);
        costrueeps = cos(trueeps);
        sintrueeps = sin(trueeps);

        nut(1,1) =  cospsi;
        nut(1,2) =  costrueeps * sinpsi;
        nut(1,3) =  sintrueeps * sinpsi;
        nut(2,1) = -coseps * sinpsi;
        nut(2,2) =  costrueeps * coseps * cospsi + sintrueeps * sineps;
        nut(2,3) =  sintrueeps * coseps * cospsi - sineps * costrueeps;
        nut(3,1) = -sineps * sinpsi;
        nut(3,2) =  costrueeps * sineps * cospsi - sintrueeps * coseps;
        nut(3,3) =  sintrueeps * sineps * cospsi + costrueeps * coseps;

      %   n1 = rot1mat( trueeps );
      %   n2 = rot3mat( deltapsi );
      %   n3 = rot1mat( -meaneps );
      %   nut = n3*n2*n1

