function [equinoctial] =  cart2equinoctial(cart,mu)

%  Initializations
if nargin < 2
    mu = 398600.4415;
end
rv = cart(1:3,1);
vv = cart(4:6,1);
r = norm(rv);
v = norm(vv);
j = 1;

% Calculate sma,and eccentricity 
ev  = ((v^2 - mu/r)*rv - (rv'*vv)*vv)/mu;
e   = norm(ev);

if (1 - e) < 1e-7
    disp(['Error:  A non-closed orbit was encountered while converting from'...
        'the Cartesian state to the equinoctial elements.' ...
        'Equinoctial elements are only valid for orbits with 0 <= eccentricy < .9999999 ']);
    equinoctial = [];
    return
end
Energy = v^2/2 - mu/r;
a   = -mu/2/Energy;

if a*(1 - e) < .01
    disp(['Error:  A nearly singular conic section was encounterd while' ...
        'converting the Cartesian state to the equinoctial elements.' ...
        'Equinoctial elements are only valid for orbits with a periapsis radius > 10 meters']);
    equinoctial = [];
    return
end

hv    = cross(rv,vv);
hvhat = hv/norm(hv);
hx    = hvhat(1,1); hy = hvhat(2,1); hz = hvhat(3,1);

denom = (1 + hz*j);
fx = 1 - hx^2/denom;
fy = -hx*hy/denom;
fz = -hx*j;
fvhat = [fx fy fz]';
gv = cross(hvhat,fvhat);

h = ev'*gv;
k = ev'*fvhat;
p = hx/denom;
q = -hy/denom;

X1 = rv'*fvhat;
Y1 = rv'*gv;

denom2 = a*sqrt(1 - h^2 - k^2);
Beta   = 1/(1 + sqrt(1 - h^2 - k^2));
prod = h*k*Beta;
cosF = k + ((1-k^2*Beta)*X1 - h*k*Beta*Y1)/denom2;
sinF = h + ((1-h^2*Beta)*Y1 - h*k*Beta*X1)/denom2;
F    = atan2(sinF,cosF);
lambda = F + h*cosF - k*sinF;

if lambda < 0
    lambda = lambda + 2*pi;
end

equinoctial = [ a h k p q lambda]';
    
