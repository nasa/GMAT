function [ Mee ] = Cart2Mee(Cart,mu)

% % If mu is not provided, use the mu for Earth
if nargin < 2
    mu = 398600.4415;
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
r_hat = reci/rmag; h_hat = hvec/hmag;
v_hat = (rmag * veci - dot(reci,veci)/rmag*reci)/hmag;
evec = cross(veci,hvec)/mu - r_hat;

%if ( abs(h_hat(3) - (-1)) < 1e-7 )
%    Mee = [];
%    disp('Error: Singularity occurs during calculate Modified Equinoctial element h and k.')
%    return
%end

Energy = vmag^2/2 - mu/rmag; SMA = -mu/2/Energy;
p_mee = hmag^2/mu;

%if ( abs( 1- norm(evec) ) < 1e-7)
%    Mee = [];
%    disp('Error: A nearly parabolci orbit is undefiend with Modified Equinoctial Elements')
%    return
%end
%
%if ( abs(p_mee) < 1e-7 )
%    Mee = [];
%    disp('Error: A nearly parabolic orbit is undefined with Modified Equinoctial Elements')
%    return
%end

%if ( p_mee < 0 )
%    Mee = [];
%    disp('Error: Semi-latus rectum has to be greater than 0')
%    return
%end

% % % % % % % GMAT Math spec 참고한 부분 ( k h g f 계산 )
j = 1;

denom = ( 1 + h_hat(3)^j );
fx = 1 - h_hat(1)^2/denom; fy = -h_hat(1)*h_hat(2)/denom;
fz = -h_hat(1)^j; f_hat = [fx fy fz]';
g_hat = cross(h_hat,f_hat);

f_mee = evec'*f_hat;
g_mee = evec'*g_hat;
h_mee = -h_hat(2)/denom;
k_mee = h_hat(1)/denom;

% % % % % % % GMAT Math spec 참고한 부분 ( True longitude 계산 )
% % X1 = reci'*f_hat; Y1 = reci' * g_hat;
% % denom2 = SMA * sqrt(1-g_mee^2-f_mee^2);
% % Beta = 1/(1+sqrt(1-g_mee^2-f_mee^2));
% % prod = g_mee * f_mee * Beta;
% % cosl = f_mee + ((1-f_mee^2*Beta)*X1 - g_mee*f_mee*Beta*Y1)/denom2;
% % sinl = g_mee + ((1-g_mee^2*Beta)*Y1 - g_mee*f_mee*Beta*X1)/denom2;

% % % % % % % Orbital mechanics with MATLAB.pdf 참고한 부분( k h g f 계산)
% % k_mee = h_hat(1)/(1+h_hat(3));
% % h_mee = -h_hat(2) /(1+h_hat(3));
% % 
% % f_hat(1) = 1 - k_mee^2 + h_mee^2;
% % f_hat(2) = 2*k_mee*h_mee;
% % f_hat(3) = 2*k_mee;
% % 
% % g_hat(1) = 2*k_mee*h_mee;
% % g_hat(2) = 1+k_mee^2-h_mee^2;
% % g_hat(3) = 2*h_mee;
% % 
% % f_til = f_hat ./(1+k_mee^2+h_mee^2);
% % g_til = g_hat ./(1+k_mee^2+h_mee^2);
% % 
% % f_mee = dot(evec,f_til);
% % g_mee = dot(evec,g_til);

% % % % % % % Orbital mechanics with MATLAB.pdf 참고한 부분( True longitude 계산)
sinl = r_hat(2)-v_hat(1);
cosl = r_hat(1)+v_hat(2);

% % L_mee = atan3(sinl,cosl);
L_mee = mod(atan2(sinl,cosl),2*pi);

Mee = real([p_mee f_mee g_mee h_mee k_mee L_mee]');

end

