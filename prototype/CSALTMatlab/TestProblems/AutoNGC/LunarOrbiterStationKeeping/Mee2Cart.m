% % input : ModEquinoct = [ p, f, g, h, k, L ]
% % outut : Cart = [r1 r2 r3 v1 v2 v3]'  (unit: km, sec)

function [ Cart ] = Mee2Cart(ModEquinoct,mu)

% % If mu is not provided, use the mu for Earth
if nargin < 2
    mu = 398600.4415;
end


% % Initializations
p = ModEquinoct(1); % semi-latus rectum
f = ModEquinoct(2); % equinoctial f
g = ModEquinoct(3); % equinoctial g
h = ModEquinoct(4); % equinoctial h
k = ModEquinoct(5); % equinoctial k
L = ModEquinoct(6); % true longitude

% % Sanity check input
if ( length(ModEquinoct) ~= 6 )
    Cart = [];
    disp('Error: Modified Equinoctial Elements have six elements')
    return
end

%if ( abs(p) < 1e-7 )
%    Cart = [];
%    disp('Error: A nearly parabolic orbit is undefined with Modified Equinoctial Elements')
%    return
%end

%if ( p < 0 )
%    Cart = [];
%    disp('Error: Semi-latus rectum has to be greater than 0')
%    return
%end



% % Coversion from Modifieid Equinoctial to Cartesian
alpha2 = h^2 - k^2;
s2 = 1 + h^2 + k^2;
w = 1 + f*cos(L)+g*sin(L);
r = p / w;

rs2 = r/s2; smup = 1/s2*sqrt(mu/p);

reci1 = rs2 * (cos(L)+alpha2*cos(L)+2*h*k*sin(L));
reci2 = rs2 * (sin(L)-alpha2*sin(L)+2*h*k*cos(L));
reci3 = 2*rs2 * (h*sin(L)-k*cos(L));

veci1 = -smup * (sin(L)+alpha2*sin(L)-2*h*k*cos(L)+g-2*f*h*k+alpha2*g);
veci2 = -smup * (-cos(L)+alpha2*cos(L)+2*h*k*sin(L)-f+2*g*h*k+alpha2*f);
veci3 = 2*smup * (h*cos(L)+k*sin(L)+f*h+g*k);

reci = [reci1 reci2 reci3]';
veci = [veci1 veci2 veci3]';

Cart = [reci; veci];

end
