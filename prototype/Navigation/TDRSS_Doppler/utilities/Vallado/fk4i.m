% ----------------------------------------------------------------------------
%
%                           function fk4i
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
% [fk4mi] = fk4 ( );
% rb1950 = fk4mi * rj2000;
% ----------------------------------------------------------------------------

function [fk4mi] = fk4i

%   in book
% 2000 - 1950
        fk4mi(1,1) =  0.9999256794956877;
        fk4mi(1,2) =  0.0111814832391717;
        fk4mi(1,3) =  0.0048590037723143;

        fk4mi(2,1) = -0.0111814832204662;
        fk4mi(2,2) =  0.9999374848933135;
        fk4mi(2,3) = -0.0000271702937440;

        fk4mi(3,1) = -0.0048590038153592;
        fk4mi(3,2) = -0.0000271625947142;
        fk4mi(3,3) =  0.9999881946043742;

return

% stk approach
% New way is formed by multiplying the matrices on pages
% 173 and 174 and adding in the correction to equinox given
% on page 168 of the supplement to the astronomical almanac 
% 2000 - 1950
        fk4mi(1,1) =  0.999925678612394;
        fk4mi(1,2) =  0.011181874524964;
        fk4mi(1,3) =  0.004858284884778;

        fk4mi(2,1) = -0.011181874556714;
        fk4mi(2,2) =  0.999937480517880;
        fk4mi(2,3) = -0.000027156932874;
        
        fk4mi(3,1) = -0.004858284812600;
        fk4mi(3,2) = -0.000027169816135;
        fk4mi(3,3) =  0.999988198095508;
  return
%

% from Exp supp to Ast Almanac pg 186 3x3 only
% 2000 - 1950
        fk4mi(1,1) =  0.9999256795;
        fk4mi(1,2) =  0.0111814828;
        fk4mi(1,3) =  0.0048590039;

        fk4mi(2,1) = -0.0111814828;
        fk4mi(2,2) =  0.9999374849;
        fk4mi(2,3) = -0.0000271771;

        fk4mi(3,1) = -0.0048590040;
        fk4mi(3,2) = -0.0000271557;
        fk4mi(3,3) =  0.9999881946;
  return


 %
 % from Exp supp to Ast Almanac pg 186 6x6
 % 2000 - 1950
        fk4mi(1,1) =  0.9999256795;
        fk4mi(1,2) =  0.0111814828;
        fk4mi(1,3) =  0.0048590039;
        fk4mi(1,4) = -0.00000242389840;
        fk4mi(1,5) = -0.00000002710544;
        fk4mi(1,6) = -0.00000001177742;

        fk4mi(2,1) = -0.0111814828;
        fk4mi(2,2) =  0.9999374849;
        fk4mi(2,3) = -0.0000271771;
        fk4mi(2,4) =  0.00000002710544;
        fk4mi(2,5) = -0.00000242392702;
        fk4mi(2,6) =  0.00000000006585;

        fk4mi(3,1) = -0.0048590040;
        fk4mi(3,2) = -0.0000271557;
        fk4mi(3,3) =  0.9999881946;
        fk4mi(3,4) =  0.00000001177742;
        fk4mi(3,5) =  0.00000000006585;
        fk4mi(3,6) = -0.00000242404995;

        fk4mi(4,1) = -0.000551;
        fk4mi(4,2) =  0.238509;
        fk4mi(4,3) = -0.435614;
        fk4mi(4,4) =  0.99990432;
        fk4mi(4,5) =  0.01118145;
        fk4mi(4,6) =  0.00485852;

        fk4mi(5,1) = -0.238560;
        fk4mi(5,2) = -0.002667;
        fk4mi(5,3) =  0.012254;
        fk4mi(5,4) = -0.01118145;
        fk4mi(5,5) =  0.99991613;
        fk4mi(5,6) = -0.00002717;

        fk4mi(6,1) =  0.435730;
        fk4mi(6,2) = -0.008541;
        fk4mi(6,3) =  0.002117;
        fk4mi(6,4) = -0.00485852;
        fk4mi(6,5) = -0.00002716;
        fk4mi(6,6) =  0.99996684;
    return    
    
      


        
        
        
        
        
        
        
        
        
        
        
        
        
        
        

