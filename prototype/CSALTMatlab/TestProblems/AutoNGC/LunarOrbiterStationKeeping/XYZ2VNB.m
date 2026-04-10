function R = XYZ2VNB(r, v)
%XYZ2RTN calculates transformation matrix to transform 3x1 vector from
% Cartesian XYZ frame to velocity-normal-binormal (VNB) frame

% the transformation matrix assumes that the VNB vector is organized such
% that the first element is the velocity element, the second element is the
% normal element (i.e., in plane), and the third element is the binormal
% element (i.e., in direction of angular momentum vector)

% inputs:
% r: 3x1 position vector in XYZ
% v: 3x1 velocity vector in XYZ

% outputs:
% R: 3x3 transformation matrix such that x_VNB = R * x_Cart

h = cross(r, v); % angular momentum vector (N direction)

hmag = norm(h);
vmag = norm(v);

iv = v / vmag; % unit vector in V direction
ih = h / hmag; % unit vector in B direction
in = cross(ih, iv); % unit vector in N direction

R = [iv, in, ih]'; % transformation matrix

end

