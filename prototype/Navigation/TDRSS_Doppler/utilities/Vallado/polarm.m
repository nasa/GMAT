%
% ----------------------------------------------------------------------------
%
%                           function polarm
%
%  this function calulates the transformation matrix that accounts for polar
%    motion. both the 1980 and 2000 theories are handled. note that the rotation 
%    order is different between 1980 and 2000 .
%
%  author        : david vallado                  719-573-2600   25 jun 2002
%
%  revisions
%    vallado     - consolidate with iau 2000                     14 feb 2005
%
%  inputs          description                    range / units
%    xp          - polar motion coefficient       rad
%    yp          - polar motion coefficient       rad
%    ttt         - julian centuries of tt (00 theory only)
%    opt         - method option                  '01', '02', '80'
%
%  outputs       :
%    pm          - transformation matrix for ecef - pef
%
%  locals        :
%    convrt      - conversion from arcsec to rad
%    sp          - s prime value
%
%  coupling      :
%    none.
%
%  references    :
%    vallado       2004, 207-209, 211, 223-224
%
% [pm] = polarm ( xp, yp, ttt, opt );
% ----------------------------------------------------------------------------

function [pm] = polarm ( xp, yp, ttt, opt );


        cosxp = cos(xp);
        sinxp = sin(xp);
        cosyp = cos(yp);
        sinyp = sin(yp);

        if (opt == '80')
            pm(1,1) =  cosxp;
            pm(1,2) =  0.0;
            pm(1,3) = -sinxp;
            pm(2,1) =  sinxp * sinyp;
            pm(2,2) =  cosyp;
            pm(2,3) =  cosxp * sinyp;
            pm(3,1) =  sinxp * cosyp;
            pm(3,2) = -sinyp;
            pm(3,3) =  cosxp * cosyp;

            % a1 = rot2mat(xp);
            % a2 = rot1mat(yp);
            % pm = a2*a1;          
            % Approximate matrix using small angle approximations
            %pm(1,1) =  1.0;
            %pm(2,1) =  0.0;
            %pm(3,1) =  xp;
            %pm(1,2) =  0.0;
            %pm(2,2) =  1.0;
            %pm(3,2) = -yp;
            %pm(1,3) = -xp;
            %pm(2,3) =  yp;
            %pm(3,3) =  1.0;
        else  
            convrt = pi / (3600.0*180.0);
            % approximate sp value in rad
            sp = -47.0e-6 * ttt * convrt;
            cossp = cos(sp);
            sinsp = sin(sp);

            % form the matrix
            pm(1,1) =  cosxp * cossp;
            pm(1,2) = -cosyp * sinsp + sinyp * sinxp * cossp;
            pm(1,3) = -sinyp * sinsp - cosyp * sinxp * cossp;
            pm(2,1) =  cosxp * sinsp;
            pm(2,2) =  cosyp * cossp + sinyp * sinxp * sinsp;
            pm(2,3) =  sinyp * cossp - cosyp * sinxp * sinsp;
            pm(3,1) =  sinxp;
            pm(3,2) = -sinyp * cosxp;
            pm(3,3) =  cosyp * cosxp;

            % a1 = rot1mat(yp);
            % a2 = rot2mat(xp);
            % a3 = rot3mat(-sp);
            % pm = a3*a2*a1;
        end;

