% ----------------------------------------------------------------------------
%
%                           function fk4
%
%  this function converts vectors between the b1950 and j2000 epochs.  be
%    aware that this process is not exact. there are different secular rates
%    for each system, and there are differences in the central location. the
%    matrices are multiplied directly for speed.
%
%  author        : david vallado                  719-573-2600   21 jun 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    rj2000      - initial j2000 eci position vec er, km, etc
%    vj2000      - initial j2000 eci velocity vec
%    direction   - which set of vars to output    from  too
%
%  outputs       :
%    fk4m        - conversion b1950 to j2000
%    fk4mi       - conversion j2000 to b1950
%
%  locals        :
%
%  coupling      :
%
%  references    :
%    vallado       2001, 227-228
%
% [fk4m] = fk4 ( );
% rj2000 = fk4m * rb1950;
% ----------------------------------------------------------------------------

function [fk4m] = fk4
%   in book
% 1950 - 2000
        fk4m(1,1) =  0.9999256794956877;
        fk4m(1,2) = -0.0111814832204662;
        fk4m(1,3) = -0.0048590038153592;

        fk4m(2,1) =  0.0111814832391717;
        fk4m(2,2) =  0.9999374848933135;
        fk4m(2,3) = -0.0000271625947142;

        fk4m(3,1) =  0.0048590037723143;
        fk4m(3,2) = -0.0000271702937440;
        fk4m(3,3) =  0.9999881946043742;

return

%
% stk approach
% New way is formed by multiplying the matrices on pages
% 173 and 174 and adding in the correction to equinox given
% on page 168 of the supplement to the astronomical almanac 
% 1950 - 2000

        fk4m(1,1) =  0.999925678612394;
        fk4m(1,2) = -0.011181874556714;
        fk4m(1,3) = -0.004858284812600;

        fk4m(2,1) =  0.011181874524964;
        fk4m(2,2) =  0.999937480517880;
        fk4m(2,3) = -0.000027169816135;

        fk4m(3,1) =  0.004858284884778;
        fk4m(3,2) = -0.000027156932874;
        fk4m(3,3) =  0.999988198095508;
return

 % from Exp supp to Ast Almanac pg 185 6x6
 % 1950 - 2000
        fk4m(1,1) =  0.9999256782;
        fk4m(1,2) = -0.0111820611;
        fk4m(1,3) = -0.0048579477;
        fk4m(1,4) =  0.00000242395018;
        fk4m(1,5) = -0.00000002710663;
        fk4m(1,6) = -0.00000001177656;

        fk4m(2,1) =  0.0111820610;
        fk4m(2,2) =  0.9999374784;
        fk4m(2,3) = -0.0000271765;
        fk4m(2,4) =  0.00000002710663;
        fk4m(2,5) =  0.00000242397878;
        fk4m(2,6) = -0.00000000006587;

        fk4m(3,1) =  0.0048579479;
        fk4m(3,2) = -0.0000271474;
        fk4m(3,3) =  0.9999881997;
        fk4m(3,4) =  0.00000001177656;
        fk4m(3,5) = -0.00000000006582;
        fk4m(3,6) =  0.00000242410173;

        fk4m(4,1) = -0.000551;
        fk4m(4,2) = -0.238565;
        fk4m(4,3) =  0.435739;
        fk4m(4,4) =  0.99994704;
        fk4m(4,5) = -0.01118251;
        fk4m(4,6) = -0.00485767;

        fk4m(5,1) =  0.238514;
        fk4m(5,2) = -0.002667;
        fk4m(5,3) = -0.008541;
        fk4m(5,4) =  0.01118251;
        fk4m(5,5) =  0.99995883;
        fk4m(5,6) = -0.00002718;

        fk4m(6,1) = -0.435623;
        fk4m(6,2) =  0.012254;
        fk4m(6,3) =  0.002117;
        fk4m(6,4) =  0.00485767;
        fk4m(6,5) = -0.00002714;
        fk4m(6,6) =  1.00000956;
return

        


 