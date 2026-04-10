function [ x, y, z ] = LLA_to_XYZ( Lat, Lon, Alt )

% [ x, y, z ] = XYZ_to_LLA( Lat, Lon, Alt )
%
% purpose:  to convert geodetic (planet-fixed) latitude, longitude, 
%           altitude to x, y, z
%
% input:  lat, lon, alt - latitude, longitude and altitude in deg, deg, m
%
% outnput:  x, y, z - planet-fixed position vector in m
%
% t. kelecy, boeing-lts, circa the dark ages
%
%...initialize parameters
format long g

global Rearth fearth deg2rad

   %dtr = pi / 180;
   
   %a = 6378.1370;  %  earth semi-major axis (km)
   %b = 6356.7523;  %  earth semi-minor axis (km)

   %f = ( a - b ) / a;
   
   %a = 6378136.3;
   %f = 3.353642e-3;

   % f = 1 / 298.257223563;
   
   Lat = Lat * deg2rad;  % convert deg to rad
   Lon = Lon * deg2rad;  % convert deg to rad
   
   sinLat = sin( Lat );
   cosLat = cos( Lat );
   sinLat2 = sinLat * sinLat;
   Denom = sqrt( 1 - ( 2 * fearth - fearth * fearth ) * sinLat2 );
   G1 = Rearth / Denom + Alt;
   G2 = Rearth * ( 1 - fearth )^2 / Denom + Alt;
   
   x = G1 * cosLat * cos( Lon );
   y = G1 * cosLat * sin( Lon );
   z = G2 * sinLat;

% end  % function [ x, y, z ] = LLA_to_XYZ( Lat, Lon, Alt )

