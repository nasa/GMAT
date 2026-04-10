function R = RTN2XYZ(r, v)
%RTN2XYZ calculates transformation matrix to transform 3x1 vector from
% radial-transverse-normal (RTN) frame into Cartesian XYZ frame

% the transformation matrix assumes that the RTN vector is organized such
% that the first element is the radial element, the second element is the
% transverse element (i.e., in plane), and the third element is the normal
% element (i.e., in direction of angular momentum vector)

% inputs:
% r: 3x1 position vector in XYZ
% v: 3x1 velocity vector in XYZ

% outputs:
% R: 3x3 transformation matrix such that x_Cart = R * x_RTN

h = cross(r, v); % angular momentum vector (N direction)

rmag = norm(r);
hmag = norm(h);

ir = r / rmag; % unit vector in R direction
ih = h / hmag; % unit vector in N direction
it = cross(ih, ir); % unit vector in T direction

R = [ir, it, ih]; % transformation matrix

end

