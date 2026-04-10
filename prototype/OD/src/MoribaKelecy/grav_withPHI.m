% ode file for asen5070_4. This file will contain the acceleration equations,
% which are nothing more than the velocity derivatives with J2 and drag...

function [dV] = grav_withPHI(dt,X)

format long g

global Rearth muearth J2 statesize

%statesize = 6;

phi = reshape(X(7:end,1),statesize,statesize);

% Earth's mean equatorial radius (m)
%Re = 6378136.3;
% Earth's angular velocity in rad/sec
%omega = 7.2921158553*(10^(-5));
% dominant zonal harmonic coefficient
%J2=1.082626925638815*(10^(-3));	% J2 coefficient
% gravitational parameter of Earth (m^3/s^2)
%muearth = 398600.4328969392*(1000^3);

% orbital radius (magnitude)
y = [X(1,1);X(2,1);X(3,1)];
r = norm(y);

point_mass = -muearth/(r^3);
term1 = 1 - (3/2)*J2*((Rearth/r)^2)*(5*((X(3,1)/r)^2)-1);
term2 = 1 - (3/2)*J2*((Rearth/r)^2)*(5*((X(3,1)/r)^2)-3);
term3 = 1 - (5/2)*J2*((Rearth/r)^2)*(7*((X(3,1)/r)^2)-1);
term4 = 1 - (5/2)*J2*((Rearth/r)^2)*(7*((X(3,1)/r)^2)-3);
term5 = 1 - (5/2)*J2*((Rearth/r)^2)*(7*((X(3,1)/r)^2)-5);
termA = (3*muearth*X(1,1)*X(2,1))/(r^5);
termB = (3*muearth*X(1,1)*X(3,1))/(r^5);
termC = (3*muearth*X(2,1)*X(3,1))/(r^5);
termD = (3*muearth*X(1,1)*X(1,1))/(r^5);
termE = (3*muearth*X(2,1)*X(2,1))/(r^5);
termF = (3*muearth*X(3,1)*X(3,1))/(r^5);

% velocity derivatives in 3 space (x,y,z)
F(1) = X(1,1)*point_mass*term1;
F(2) = X(2,1)*point_mass*term1;
F(3) = X(3,1)*point_mass*term2;

% compute PHIdot
Amatrix = zeros(statesize,statesize);
Amatrix(1,4) = 1;
Amatrix(2,5) = 1;
Amatrix(3,6) = 1;
Amatrix(4,1) = point_mass*term1 + termD*term3;
Amatrix(4,2) = term3*termA;
Amatrix(4,3) = term4*termB;
Amatrix(5,1) = Amatrix(4,2);
Amatrix(5,2) = point_mass*term1 + termE*term3;
Amatrix(5,3) = term4*termC;
Amatrix(6,1) = term4*termB;
Amatrix(6,2) = Amatrix(5,3);
Amatrix(6,3) = point_mass*term2 + termF*term5;

phiDotM = Amatrix*phi;
phiDot = reshape(phiDotM,statesize*statesize,1);

dV = [X(4:6,1);F';phiDot];

%...end of function [dV] = grav_withPHI(dt,X)