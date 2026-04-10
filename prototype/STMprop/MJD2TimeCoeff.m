function [DeltaAT, DeltaUT1] = MJD2TimeCoeff(MJD)

%  This needs to be rewritten some day, it was modified from a function
%  that provides both UT2 and DeltaAT offsets.  I removed the DeltaUT1
%  offset because it is not needed and I didn't have the most recent data
%  to update the file. 


%  From Table 3-4 Vallado.  Pg 195.
DeltaATvec   = [24 24 24 24 25 25 26 26 26 27 27 28 28 29 29 29 30 ...
                30 30 31 31 31 32 32 32 32 33 34 35]';
 
JDVec     = [2447161.5  2447343.5  2447527.5  2447708.5   2447892.5 ...
             2448073.5  2448257.5  2448438.5   2448622.5  2448804.5 ...
             2448988.5  2449169.5  2449353.5  2449534.5   2449718.5 ...
             2449899.5  2450083.5  2450265.5  2450449.5   2450630.5 ...
             2450814.5  2450995.5  2451179.5  2451360.5   2451544.5 ...
             2451726.5  2453736.5  2454832.5  2456109.5]';

m = size(JDVec,1);

if MJD < JDVec(1);
    DeltaAT  = DeltaATvec(1);
    return
elseif  MJD > JDVec(m)
    DeltaAT = DeltaATvec(m);
else
    count = 1;
    while  MJD > JDVec(count)
        count = count + 1;
    end
    count =  count - 1;
    DeltaAT = DeltaATvec(count);
end

     