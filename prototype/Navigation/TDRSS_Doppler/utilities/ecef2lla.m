function [lat, long, alt] = ecef2lla(r_ecef )
% 
% r_ecef = earth centered earth fixed postion vector in km
% [lat long alt] = ecef2lla(r_ecef )
% Uses WGS84 flattening
%
% Reference: Vallado,4th edition , pg 173

[m ,n] = size(r_ecef);
if m~=3
    r_ecef=r_ecef';
end
% compute earth eccentricity 
e_earth = sqrt(2*fe-fe^2);

rdelta_sat = sqrt(r_ecef(1,:).^2 +r_ecef(2,:).^2);
a = RE;

% compute polar radius
rb = a*(1-fe);
% rb =  6356.75160056;
% b = sqrt(rb*(1-e_earth^2))*sign(r_ecef(3,:));
b= sign(r_ecef(3,:)) .* 6356.75160056;
E = (b.*r_ecef(3,:) - (a.^2-b.^2))./(a.*rdelta_sat);

% compute longitude
alpha = atan2(r_ecef(2,:)./rdelta_sat,r_ecef(1,:)./rdelta_sat);
long = alpha;

F = (b.*r_ecef(3,:) + (a.^2-b.^2))/(a.*rdelta_sat);
P = 4*(E.*F+1)/3;
Q = 2*(E.^2 -F.^2);
D = P.^3 + Q.^2;

if (D > 0)
    nu = ((sqrt(D) - Q).^(1/3)) -( (sqrt(D) + Q).^(1/3));
else
    nu = 2.*sqrt(-P)*cos((1/3)*acos(Q./(P.*sqrt(-P))));
end

G=(1/2).*(sqrt(E.^2+nu) + E);

t = sqrt(G.^2 + ((F-nu.*G)/(2.*G-E))) -G;

lat = atan(a.*(1-t.^2)./(2.*b.*t));

alt = (rdelta_sat - a.*t).*cos(lat) + (r_ecef(3,:) - b).*sin(lat);

% convert to degrees
lat = r2d(lat);
long = r2d(long);

end