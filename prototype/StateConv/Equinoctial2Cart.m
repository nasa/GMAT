function [cart] = Equinoctial2Cart(equinoct,mu)

%%  If mu in not provided, use the mu for Earth
if nargin < 2
   mu = 398600.4415;
end

if max(size(equinoct))~= 6
    cart = [];
    return
end

%  Initializations
sma = equinoct(1,1);
h   = equinoct(2,1);
k   = equinoct(3,1);
p   = equinoct(4,1);
q   = equinoct(5,1);
meanLon = equinoct(6,1);
p2 = p*p; h2 = h*h; k2 = k*k; q2 = q*q;
j = 1;  

%%  Iterate to find eccentric longitude, F
F = meanLon;
diff = 1e12;
while diff > 1e-13
    func = F + h*cos(F) - k*sin(F) - meanLon;
	funcDeriv = 1 - h*sin(F) - k*cos(F);
	oldF = F;
	F = F - func/funcDeriv;
	diff = abs(F - oldF);
end

%% Compute cartesian components in equinoctial coordinate system.
sinF = sin(F); cosF = cos(F);
beta = 1/( 1 + sqrt(1 - h2 - k2));
n    = sqrt(mu/sma^3);
r     = sma*(1 - k*cosF - h*sinF);
X1    = sma*( (1-h2*beta)*cosF + h*k*beta*sinF - k );
Y1    = sma*( (1-k2*beta)*sinF + h*k*beta*cosF - h );
X1dot = n*sma*sma/r*( h*k*beta*cosF - ( 1 - h2*beta)*sinF );
Y1dot = n*sma*sma/r*( (1 - k2*beta)*cosF - h*k*beta*sinF );

%%  Convert cartesian components to "inertial" system
denom = (1 + p2 + q2);
fHat  = [ 1- p2  + q2 , 2*p*q , -2*p*j ]'/denom;
gHat  = [  2*p*q*j ,(1 + p2 - q2)*j,2*q]'/denom;
cart(1:3,1) = X1*fHat + Y1*gHat;
cart(4:6,1) = X1dot*fHat + Y1dot*gHat;


