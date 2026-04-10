
%  This script tests propagation using Luna as the central body.  

Sat.Epoch = '01 Jan 2000 11:59:28.000';
Sat.X = 1463.475046306674;
Sat.Y = 1306.933050619089;
Sat.Z = 379.2951674693759;
Sat.VX = -1.053689826526843;
Sat.VY = 1.156596755746541;
Sat.VZ = 0.08537397172999123;

ForceModel.CentralBody = 'Luna';
ForceModel.PointMasses = {'Sun', 'Earth', 'Luna'};
ForceModel.SRP = 'Off';
ForceModel.SolarRadiationPressure.Flux = 1367;
ForceModel.SolarRadiationPressure.Nominal_Sun = 149597870.691;


% --------------------------------------
%          Propagate the Spacecraft
% --------------------------------------
TOF     = 3600;
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
disp(['Position   ' num2str( X(n,1:3),12) ]);
disp(['Velocity   ' num2str( X(n,4:6),12) ]);
disp('  ')