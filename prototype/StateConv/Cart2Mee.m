function [ Mee ] = Cart2Mee(Cart,mu,j)

% % If mu is not provided, use the mu for Earth
if ( nargin < 2 )
    mu = 398600.4415;
end

if ( nargin < 3 )
   j = 1; % j = 1 for direct orbits, j = -1 for retrograde orbits
end

if ( length(Cart) ~= 6 )
    Mee = [];
    disp('Error: Cartesian Elements have six elements')
    return
end

% % Initializations
reci(1:3,1) = Cart(1:3); veci(1:3,1) = Cart(4:6);
rmag = norm(reci); vmag = norm(veci);
hvec = cross(reci,veci); hmag = norm(hvec);
if rmag == 0
    r_hat = [0;0;0];
else
    r_hat = reci/rmag;
end
if hmag == 0
    h_hat = [0;0;0];
    v_hat = [0;0;0];
else
    h_hat = hvec/hmag;
    v_hat = (rmag * veci - dot(reci,veci)/rmag*reci)/hmag;
end
evec = cross(veci,hvec)/mu - r_hat;

% Energy = vmag^2/2 - mu/rmag; SMA = -mu/2/Energy;
p_mee = hmag^2/mu;

if ( p_mee < 0 )
    Mee = [];
    disp('Error: Semi-latus rectum has to be greater than 0')
    return
end

% % Reffered to 'GMAT math spec' (p.52-53)
denom = ( 1 + h_hat(3)*j );

if ( abs(denom) < 1e-7 )
    disp('Warning: Singularity may occur during calculate Modified Equinoctial element h and k')
elseif ( abs(denom) < 2*eps )
    Mee = [];
    disp('Error: Singularity occurs during calculate Modified Equinoctial element h and k.')
    return
end

fx = 1 - h_hat(1)^2/denom; fy = -h_hat(1)*h_hat(2)/denom;
fz = -h_hat(1)*j; f_hat = [fx fy fz]';
g_hat = cross(h_hat,f_hat);

f_mee = evec'*f_hat;
g_mee = evec'*g_hat;
h_mee = -h_hat(2)/denom;
k_mee = h_hat(1)/denom;

% % Reffered to 'Orbital mechanics with MATLAB.pdf' (p.34)
sinl = r_hat(2)-v_hat(1);
cosl = r_hat(1)+v_hat(2);

% % L_mee = atan3(sinl,cosl);
L_mee = mod(atan2(sinl,cosl),2*pi);

Mee = [p_mee f_mee g_mee h_mee k_mee L_mee]';

end

