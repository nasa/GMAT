% Unit test script for Earth class

cb = Earth();

GMT = cb.ComputeGMT(2457260.12345679);

%  Need to verify the sign. (compares hi-fi output from GMAT/STK with
%  low-fi model in TAT-C, hence the loose tolerance
if abs(GMT*180/pi - 198.002628503035)/198.002628503035 >= 1e-5
   error('error calcualing GMT')
end

