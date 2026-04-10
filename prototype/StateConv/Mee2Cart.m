% % input : ModEquinoct = [ p, f, g, h, k, L ]
% % outut : Cart = [r1 r2 r3 v1 v2 v3]'  (unit: km, sec)

function [ Cart ] = Mee2Cart(ModEquinoct,mu,j)

% % If mu is not provided, use the mu for Earth
if ( nargin < 2 )
    mu = 398600.4415;
end

if ( nargin < 3 )
    j = 1; % j = 1 for direct orbits, j = -1 for retrograde orbits
end

% % Initializations
p = ModEquinoct(1); % semi-latus rectum
f = ModEquinoct(2); % equinoctial f
g = ModEquinoct(3); % equinoctial g
h = ModEquinoct(4); % equinoctial h
k = ModEquinoct(5); % equinoctial k
L = ModEquinoct(6); % *pi/180; % true longitude

% % Sanity check input
if ( length(ModEquinoct) ~= 6 )
    Cart = [];
    disp('Error: Modified Equinoctial Elements have six elements')
    return
end

if ( p < 0 )
    Cart = [];
    disp('Error: Semi-latus rectum has to be greater than 0')
    return
end

% % % Reffered to 'GMAT Math spec.pdf' (p.51-52)
% lambda = L + g*cos(L) - f*sin(L); % mean longitude
% f = L + g*cos(L) - f*sin(L) - lambda;
% beta = 1 / ( 1+sqrt(1-g^2-f^2) ); SMA = p / (1-f^2-g^2);
% n = sqrt(mu/(SMA^3)); 
% % r = SMA*( 1-f*cos(L) - g*sin(L) );
% r = SMA*( 1-f*cos(L) - g*sin(f) );
% 
% X1 = SMA*( (1-g^2*beta)*cos(L) + g*f*beta*sin(L) - f );
% Y1 = SMA*( (1-f^2*beta)*sin(L) + g*f*beta*cos(L) - g );
% dotX1 = n*(SMA^2)/r*( g*f*beta*cos(L) - (1-g^2*beta)*sin(L) );
% dotY1 = n*(SMA^2)/r*( (1-f^2*beta)*cos(L) - g*f*beta*sin(L) );
% 
% denorm = 1 / (1 + k^2 + h^2);
% f_hat = [ 1-k^2+h^2; 2*k*h; -2*k*j ].*denorm;
% g_hat = [ 2*k*h*j; (1+k^2-h^2)*j; 2*h ].*denorm;
% 
% reci = X1.*f_hat + Y1.*g_hat;
% veci = dotX1.*f_hat + dotY1.*g_hat;
% 
% Cart = [reci; veci];

% % % Reffered to 'Modified Equinoctial Elements.pdf' (p.2-3)
% alpha2 = h^2 - k^2;
% s2 = 1 + h^2 + k^2;
% w = 1 + f*cos(L)+g*sin(L);
% r = p / w;
% 
% rs2 = r/s2; smup = 1/s2*sqrt(mu/p);
% 
% reci1 = rs2 * (cos(L)+alpha2*cos(L)+2*h*k*sin(L));
% reci2 = rs2 * (sin(L)-alpha2*sin(L)+2*h*k*cos(L));
% reci3 = 2*rs2 * (h*sin(L)-k*cos(L));
% 
% veci1 = -smup * (sin(L)+alpha2*sin(L)-2*h*k*cos(L)+g-2*f*h*k+alpha2*g);
% veci2 = -smup * (-cos(L)+alpha2*cos(L)+2*h*k*sin(L)-f+2*g*h*k+alpha2*f);
% veci3 = 2*smup * (h*cos(L)+k*sin(L)+f*h+g*k);
% 
% reci = [reci1 reci2 reci3]';
% veci = [veci1 veci2 veci3]';
% 
% Cart = [reci; veci];

% New derivation from Modified Equinoctial spec
r = p/(1 + f*cos(L) + g*sin(L));

X1 = r * cos(L);
Y1 = r * sin(L);

if p == 0
    dotX1 = 0;
    dotY1 = 0;
else
    dotX1 = -sqrt(mu/p) * (g + sin(L));
    dotY1 = sqrt(mu/p) * (f + cos(L));
end

alpha2 = h^2 - k^2;
s2 = 1 + h^2 + k^2;
f_hat = [1+alpha2; 2*k*h; -2*k*j]./s2;
g_hat = [2*k*h*j; (1-alpha2)*j; 2*h ]./s2;

reci = X1.*f_hat + Y1.*g_hat;
veci = dotX1.*f_hat + dotY1.*g_hat;

Cart = [reci; veci];

end
