
% --------------------------------------
%  Define dX0 (km and km/s)
% --------------------------------------
dx  = [.001 .001 .001 0.00001 0 0];

% --------------------------------------
%   Define The Spacecraft State
% --------------------------------------
Sat.Epoch = '01 Jan 2000 11:59:28.000';
Sat.X = 1463.475046306674;
Sat.Y = 1306.933050619089;
Sat.Z = 379.2951674693759;
Sat.VX = -1.053689826526843;
Sat.VY = 1.156596755746541;
Sat.VZ = 0.08537397172999123;

% --------------------------------------
%   Define the Force Model
% --------------------------------------
ForceModel.CentralBody = 'Luna';
ForceModel.PointMasses = {'Sun', 'Earth', 'Luna'};
ForceModel.SRP = 'Off';
ForceModel.SolarRadiationPressure.Flux = 1367;
ForceModel.SolarRadiationPressure.Nominal_Sun = 149597870.691;

% --------------------------------------
%   Propagate the nominal Orbit
% --------------------------------------
TOF     = 3600;
[t, X, Phi] = Propagate(Sat,ForceModel,TOF);

% --------------------------------------
%   Output data for nominal orbit
% --------------------------------------
disp('  ')
disp('-------------------------------')
disp('----Propagation Complete-------')
disp('-------------------------------')
disp('  ')
n =  size(X,1);
disp('--------------    Propation Results For Orbit 1  -----------------')
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( X(n,1:3),12) ]);
disp(['Velocity   ' num2str( X(n,4:6),12) ]);
disp('  ')
phi  = Phi(:,:,n);
x1 = X(n,1:6);

% --------------------------------------
%   Propagate the perturbed orbit
% -------------------------------------
Sat.X = 1463.475046306674 + dx(1);
Sat.Y = 1306.933050619089 + dx(2);
Sat.Z = 379.2951674693759 + dx(3);
Sat.VX = -1.053689826526843 + dx(4);
Sat.VY = 1.156596755746541 + dx(5);
Sat.VZ = 0.08537397172999123 + dx(6);
[t, X, Phi] = Propagate(Sat,ForceModel,TOF);

% --------------------------------------
%   Output data for perturbed orbit 
% --------------------------------------

n =  size(X,1);
disp('--------------    Propation Results For Orbit 2  -----------------')
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( X(n,1:3),12) ]);
disp(['Velocity   ' num2str( X(n,4:6),12) ]);
disp('  ')
x2 = X(n,1:6);

% ----------------------------------------------
%   Output exact and approx final relative state
% ----------------------------------------------

disp('--------------    Relative Final States (Exact)  -----------------')
relx_exact = x2 - x1;
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( relx_exact(1,1:3),12) ]);
disp(['Velocity   ' num2str( relx_exact(1,4:6),'%14.12f') ]);
disp('  ')

relx_approx = (phi*dx')';

disp('--------------    Relative Final States (From STM)  -----------------')
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( relx_approx(1,1:3),12) ]);
vxstr = num2str( relx_approx(1,4),'%14.12f');
vystr = num2str( relx_approx(1,4),'%14.12f');
vzstr = num2str( relx_approx(1,4),'%14.12f');
disp(['Velocity   '  vxstr '    '  vystr '    ' vzstr ]);
disp('  ')

% ----------------------------------------------
%   Output the difference between exact and approx
%   final relative states
% ----------------------------------------------
disp('--------------    Difference Between Exact and STM  -----------------')
diff = relx_exact -  relx_approx;
xstr = num2str(  diff(1,1),'%14.12f');
ystr = num2str(  diff(1,2),'%14.12f');
zstr = num2str(  diff(1,3),'%14.12f');
disp(['Position   '  xstr '    '  ystr '    ' zstr ]);
vxstr = num2str(  diff(1,4),'%14.12f');
vystr = num2str(  diff(1,4),'%14.12f');
vzstr = num2str(  diff(1,4),'%14.12f');
disp(['Velocity   '  vxstr '    '  vystr '    ' vzstr ]);
disp('  ')








