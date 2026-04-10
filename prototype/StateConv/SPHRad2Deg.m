function [out] = SPHRad2Deg(in);

out = in;
d2r    = pi/180;
out(2) = out(2)/d2r;
out(3) = out(3)/d2r;
out(5) = out(5)/d2r;
out(6) = out(6)/d2r;