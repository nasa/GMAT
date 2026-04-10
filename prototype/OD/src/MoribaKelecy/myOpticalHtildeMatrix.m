function [Htilde] = myOpticalHtildeMatrix(radeccomp,X)

format long g


global OWLT eciPos speedoflight

% define constants
%deg2rad = pi/180;
%rad2deg = 180/pi;

% convert angles to radians
%alpha = deg2rad*radeccomp(1);
%delta = deg2rad*radeccomp(2);
alpha = radeccomp(1);
delta = radeccomp(2);

ltc = 0;
% define relevant components and magnitudes
for ii = 1 : 3
    aVec = X(1:3,1) - eciPos - ltc*X(4:6,1);
    x = aVec(1,1);
    y = aVec(2,1);
    z = aVec(3,1);
    a = norm(aVec);
    ltc = OWLT * a / speedoflight;
end


% initialize Htilde
Htilde = zeros(2,length(X));

% compute partials of Right Ascension wrt State
dAlphadX = - tan(alpha)/(x + y*tan(alpha));
dAlphadY = 1/(x + y*tan(alpha));
%dAlphadXdot = OWLT*dAlphadX;
%dAlphadYdot = OWLT*dAlphadY;
dAlphadXdot = ltc*dAlphadX;
dAlphadYdot = ltc*dAlphadY;

% compute partials of Declination wrt State
dDeltadX = -(x*tan(delta))/(a^2);
dDeltadY = -(y*tan(delta))/(a^2);
dDeltadZ = sqrt(1 - (sin(delta)^2))/a;
%dDeltadXdot = OWLT*dDeltadX;
%dDeltadYdot = OWLT*dDeltadY;
%dDeltadZdot = OWLT*dDeltadZ;
dDeltadXdot = ltc*dDeltadX;
dDeltadYdot = ltc*dDeltadY;
dDeltadZdot = ltc*dDeltadZ;

% Populate Htilde
Htilde(1,1) = dAlphadX;
Htilde(1,2) = dAlphadY;
Htilde(1,4) = dAlphadXdot;
Htilde(1,5) = dAlphadYdot;
Htilde(2,1) = dDeltadX;
Htilde(2,2) = dDeltadY;
Htilde(2,3) = dDeltadZ;
Htilde(2,4) = dDeltadXdot;
Htilde(2,5) = dDeltadYdot;
Htilde(2,6) = dDeltadZdot;

%...end of function