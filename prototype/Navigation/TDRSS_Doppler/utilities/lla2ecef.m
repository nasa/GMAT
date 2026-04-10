function r_ecef = lla2ecef(gd_lat,long,h)
%  r_ecef = lla2ecef(gd_lat,long,h)
% all angles are in degrees
%
% Altitude is altitude above earth's ellipsoid in km
%
% Reference: Vallado, Fundamentals of Astrodynamics Version 4
%            http://microem.ru/files/2012/08/GPS.G1-X-00006.pdf
% Uses WSG84 ellipsoid
% Unit tested against: http://www.oc.nps.edu/oc2902w/coord/llhxyz.htm

% compute WSG84 earth eccentricity
e_earth = sqrt(2*fe-fe^2);

% define auxillary quantities (obtained from geometric quantities of an
% ellipse
N = (RE)/sqrt(1-e_earth^2 .* (sind(gd_lat)).^2);

r_x = (N + h) * cosd(gd_lat) * cosd(long);
r_y = (N + h) * cosd(gd_lat) * sind(long);

b = RE*(1-fe);
r_z = ((b^2/RE^2)*N+h)*sind(gd_lat);

r_ecef = [ r_x; r_y; r_z];


end