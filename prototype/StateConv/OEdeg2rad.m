function out = OEdeg2rad(in)

d2r = pi/180;
out = in;
out(3) = out(3)*d2r;
out(4) = out(4)*d2r;
out(5) = out(5)*d2r;
out(6) = out(6)*d2r;