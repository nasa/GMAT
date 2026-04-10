function [E,M] = enu2EM(e,nu)
%  [E,M] = enu2EM(e,nu)
%
%  Calculate the eccentric anomaly and the mean anomaly from the
%  eccentricity and true anomaly by solving Kepler's equation

cosE = (e+cos(nu))/(1+e*cos(nu));
sinE = sqrt(1-e^2)*sin(nu)/(1+e*cos(nu));

E = angleCheck(atan2(sinE,cosE));

M = angleCheck(E-e*sin(E));


function  x = angleCheck(x)
%  insures 0 < x < 2 pi
while (x<0)
   x = x + 2*pi;
end
while (x>2*pi)
   x = x -2*pi;
end