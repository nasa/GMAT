clc

% --------------------------------------
%          Define the Force Model
% --------------------------------------
ForceModel.CentralBody   = 'Sun';
ForceModel.PrimaryBodies = {'Sun'};
ForceModel.PointMasses   = {'Sun','Mars'};
%ForceModel.PointMasses   = {'Earth' 'Luna' 'Sun' ,'Mars', 'Mercury', 'Venus', 'Jupiter', 'Saturn', 'Uranus','Neptune','Pluto'};

ForceModel.SRP           = 'Off';
ForceModel.SolarRadiationPressure.Flux = 1367;
ForceModel.SolarRadiationPressure.Nominal_Sun = 149597870.691;
ForceModel.EphemerisSource = 'DE405';

% --------------------------------------
%          Define the Spacecraft
% --------------------------------------
Sat.Epoch = '17 Apr 2014 19:56:34.235' ;
Sat.X = -177752991.28891;
Sat.Y = -60115454.477842;
Sat.Z = -21787710.987005;
Sat.VX = 3.8369598289566;
Sat.VY = -23.242693574021;
Sat.VZ = -10.744923481157;

% --------------------------------------
%          Propagate the Spacecraft
% --------------------------------------
TOF     = 165*86400;
[t, X, Phi] = Propagate(Sat,ForceModel,TOF);

% --------------------------------------
%          Output
% --------------------------------------
disp('  ')
disp('-------------------------------')
disp('----Propagation Complete-------')
disp('-------------------------------')
disp('  ')
n =  size(X,1);
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( X(n,1:3),14) ]);
disp(['Velocity   ' num2str( X(n,4:6),14) ]);
disp('  ')

plot3(X(:,1),X(:,2),X(:,3))

