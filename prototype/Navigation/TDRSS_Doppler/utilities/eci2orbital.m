function [ h_mag a inc OMEGA e_mag omega ta ] = eci2orbital(r, v)
% [ h_mag a inc OMEGA e_mag omega ta ] = eci2orbital(r, v)
%
% Inputs:    r and v vectors in eci coordinates in m. 
%            Inputs are allowed to be arrays of state
%            vectors but must be mx3
%            
% Outputs:  Angular momentum magnitude
%           Semi-major axis(m)
%           inc(degs)
%           RAAN(Degs)
%           e_mag
%           argument of perigee, omega (degs)
%           ta(degrees)
%



% initialize orbit type flags to 0
circular=0;
equatorial=0;

% input error checks
[m1 n1] = size(r);
[m2 n2 ] = size(v);
if m1~=m2 || n1~=n2
    error('Input vectors must be the same size!')
elseif n1~=3 || n2~=3
    error('Input vectors must be mx3')
end

% take the sqrt of machine tolerance for bounds
tol = sqrt(eps);

% Compure magnitude of radius vector and orbital angular momentum vector
r_mag = dimwise_mag((r),'row');
[ m n ] = size(r);
h = NaN(m,n);
for i =1:length(r)
    h(i,:) = cross(r(i,:),v(i,:));
end
h_mag = dimwise_mag((h),'row')';

% compute inclination
inc = NaN(length(h),1);
for i=1:length(h)
    inc(i) = r2d(acos(h(i,3)./h_mag(i))); 
end

% compute node
k(length(h),3) = zeros;
k(:,3) = ones;
N = NaN(m,n);
for i = 1:length(h)
    N(i,:) = cross(k(i,:),h(i,:));
end
N_mag = dimwise_mag(N,'row');

% % Compute raan
 OMEGA = NaN(length(N_mag),1);
for i=1:length(N_mag)
    
    if inc(i) <= tol
     equatorial = 1;
    end
    
    if equatorial
         OMEGA(i,1) = 0;
    else
         OMEGA(i,1) = r2d(atan2(N(i,2),N(i,1)));
    end
    equatorial = 0;
end

% if N(2)>=0
%     OMEGA = acosd(N(1)/N_mag);
% else
%     OMEGA = 360 - acosd(N(1)/N_mag);
% end

% compute eccentricity
[ m n ] = size(h);
e = NaN(m,n); 
for i = 1:length(r_mag)
e(i,:) = (1/mu('m')) .* ( cross(v(i,:),h(i,:)) -( mu('m') .*r(i,:)./r_mag(i)));
end

% Compute row wise magnitude of eccentricity
e_mag = dimwise_mag(e,'row')';

% Initialize omega vector
omega = NaN(length(e_mag),1);
%Compute argument of perigee
for i=1:length(e_mag)
    % decide if orbit is circular
    if e_mag(i) <=tol
      circular=1;
    end
    if circular
      omega(i)=0;
    elseif e(i,3)>= 0
      omega(i) = r2d(acos(dot(N(i,:),e(i,:))/(N_mag(i)*e_mag(i))));
    else
      omega(i) =360- r2d(acos(dot(N(i,:),e(i,:))/(N_mag(i)*e_mag(i))));
    end
    circular=0;
end

% Compute true anomaly
% Compute radial velocity
v_r = NaN(length(r_mag),1);
ta = NaN(length(r_mag),1);
for i=1:length(r_mag)
    v_r(i) = dot(r(i,:),v(i,:))/r_mag(i);
    if v_r >=0
        ta(i,1) = r2d(acos(dot(e(i,:),r(i,:))/(e_mag(i)*r_mag(i))));
    else
        ta(i,1) = 360 - r2d(acos(dot(e(i,:),r(i,:))/(e_mag(i)*r_mag(i))));
    end
end

% SMA Computation
rp = NaN(length(h_mag),1);
ra = NaN(length(h_mag),1);
a = NaN(length(h_mag),1);
for i=1:length(h_mag)
% compute apogee and perigee radii
    rp(i) = ((h_mag(i))^2./mu('m')) *(1/(1+e_mag(i)));
    ra(i) = ((h_mag(i))^2./mu('m')) *(1/(1-e_mag(i)));
% Compute semi-major axis
    a(i,1) = (1/2).*(rp(i) + ra(i));
end

end