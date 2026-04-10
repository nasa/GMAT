function [R_IB, Rdot_IB, Omega_IB_B] = PrecessingSpinner(pv_I,sv_B,spin_t0,spinrate,...
                                          prec_t0,precrate,nut, dt)

% pv_I: precession vector in the inertial frame
% sv_b: spin axis vector in body frame
% spin_t0:  spin angle at t0
% spinrate: spin rate
% prec_t0:  precession angle at t0
% precrate: precesion angle rate
% nut:  nutation angle

%%  Normalize vectors and validate input
normpv = norm(pv_I);
if normpv < 1e-9
    disp('Error in PrecessingSpinner: Precession vector is nearly zero');
    R_IB = [];
    return
else
    pv_I = pv_I/normpv;
end
normsv = norm(sv_B);
if normsv < 1e-9
    disp('Error in PrecessingSpinner: Spin vector is nearly zero');
    R_IB = [];
    return
else
    sv_B = sv_B/normsv;
end
if nut< 1e-5
    disp(['Warning in PrecessingSpinner:  Nutation is near zero and'...
          ' attitude is poorly defined' ]);
end

%% Compute the rotation matrix from body to inertial
%  Starting from body frame, apply spin angle about s_B
spin   = spin_t0 + spinrate*dt;
spin   = mod(spin,2*pi);
R_Spin = EulerAxisAngle(sv_B,-spin);

%  Starting from inertial frame, apply precession angle
prec   =  prec_t0 + precrate*dt;
prec   = mod(prec,2*pi);
R_Prec =  EulerAxisAngle(pv_I,-prec);

%  Compute vector normal to spin vector and prec vector.  This is the
%  vector about which nutation occurs.  
n            = cross(pv_I,sv_B);
currentAngle = acos(pv_I'*sv_B);
rotAngle     = currentAngle - (-nut);
nhat         = n/norm(n);
R_Nut        = EulerAxisAngle(nhat,rotAngle);
R_IB         = R_Prec*R_Nut*R_Spin;
Rdot_IB      = zeros(3,3);  % TODO: finish this computation
Omega_IB_B   = sv_B*spinrate;

function R = EulerAxisAngle(a,phi)

% R = EulerAxisAngle(a,phi)
%     a is the Euler axis (a 3 x 1 matrix)
%     phi is the Euler principal angle, a scalar
%     returns the rotation matrix

[m,n]=size(a);
if (m==1 && n==3)
    a=a';
end
if ((m~=1 && n~=1) || (m~=3 && n~=3))
   disp('a is not a 3 x 1 matrix');
   R=NaN*ones(3,3);
   return;
end
c = cos(phi); s = sin(phi);
R = c*eye(3) + (1-c)*(a*a') -s*skew(a);

% This function takes a 3X1 column vector and turns it into a
% 3X3 skew symmetric matrix

function [V]= skew(v)

V = [0 -v(3) v(2);v(3) 0 -v(1);-v(2) v(1) 0];    
