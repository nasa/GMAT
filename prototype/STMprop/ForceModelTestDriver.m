clc
global SolarSystem jd_ref 

% --------------------------------------
%          Define the Force Model
% --------------------------------------
ForceModel.CentralBody   = 'Sun';
ForceModel.PointMasses   = { 'Sun','Earth','Luna' };
ForceModel.SRP = 'Off';
ForceModel.SolarRadiationPressure.Flux = 1367;
ForceModel.SolarRadiationPressure.Nominal_Sun = 149597870.691;
ForceModel.EphemerisSource = 'SPICE';
ForceModel.IncludeCentralBody = false;

% Set the spacecraft initial state and epoch
Sat.Epoch = '17 Apr 2014 19:56:34.235' ;
Sat.X = -177752991.28891;
Sat.Y = -60115454.477842;
Sat.Z = -21787710.987005;
Sat.VX = 3.8369598289566;
Sat.VY = -23.242693574021;
Sat.VZ = -10.744923481157;
datevec = datestr2vec(Sat.Epoch);
jd_ref  = greg2jd(datevec);   

%  Initialize the solar system
InitSolarSystem;

%  Initialize the force model
ForceModel = InitForceModel(ForceModel); 

%  Call the force model
X0v  = [Sat.X Sat.Y Sat.Z...
       Sat.VX Sat.VY Sat.VZ ]';
   
[Xdot] = dynfunc(0,X0v,ForceModel)