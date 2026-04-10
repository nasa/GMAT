
function [az, el]  = ecef2azel(gdlat,long, h, r_ecef, v_ecef)
%   [az, el]  = ecef2azel(gdlat,long, h, r_ecef, v_ecef)
%
%   Computes satellite elevation and azimuth angle with respect to a topocentric
%   observer
%
%   Inputs: gdlat, geodetic lat of observer (degs)
%           long,  lat of observer (degs)
%           h,     height above WSG84 ellipsiod (m)
%           r_ecef, ecef postion vector of satellite (m)
%           v_ecef, ecef velocity vector in m/s
%   Output: Az, Azimuth of the satellite with respect to the ground station
%           (degrees)
%           El, Elevation of the satellite with respect to the ground 

% compute ground station ecef vector
r_ecef_gs = lla2ecef(gdlat,long,h);


% compute the ecef range vector between the site and the user in ecef
rho_ecef = r_ecef-r_ecef_gs*1000;
rhodot_ecef = v_ecef;

% rotate the range vector into the topocentric frame
r2 = [ cosd(90-gdlat)       0       -sind(90-gdlat); 
       0                    1                   0;
      sind(90-gdlat)       0       cosd(90-gdlat)];
r3 = [ cosd(long)     sind(long)                0;
       -sind(long)      cosd(long)              0;
       0                   0                    1];



rho_sez= r2*r3*rho_ecef;

fprintf('rho_sez: %f\n', rho_sez);
rhodot_sez = r2 * r3 *  rhodot_ecef;
 
% compute elevation
el = asind(rho_sez(3)/norm(rho_sez));

s_rho = rho_sez(2)/sqrt(rho_sez(1).^2 + rho_sez(2).^2);
c_rho = -rho_sez(1)/sqrt(rho_sez(1).^2 + rho_sez(2).^2);
s_rhodot = rho_sez(2)/sqrt(rho_sez(1).^2 + rho_sez(2).^2);
c_rhodot = -rho_sez(1)/sqrt(rho_sez(1).^2 + rho_sez(2).^2);

if el ~= 90
    az = atan2d(s_rho,c_rho);
elseif az==90
    az = atan2d(s_rhodot,c_rhodot);
end


end

