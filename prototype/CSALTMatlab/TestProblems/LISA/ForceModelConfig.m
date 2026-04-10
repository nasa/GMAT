clc
global SolarSystem jd_ref 

% --------------------------------------
%          Define the Force Model
% --------------------------------------
theForceModel.CentralBody   = 'Sun';
theForceModel.PointMasses   = { 'Sun','Earth','Luna' };
theForceModel.SRP = 'Off';
theForceModel.SolarRadiationPressure.Flux = 1367;
theForceModel.SolarRadiationPressure.Nominal_Sun = 149597870.691;
theForceModel.EphemerisSource = 'SPICE';
theForceModel.IncludeCentralBody = false;
theForceModel.PropSTM = false;

% Set the spacecraft initial state and epoch
jd_ref  = refEpochTDB + 2430000.0;   

%  Initialize the solar system
InitSolarSystem;

%  Initialize the force model
theForceModel = InitForceModel(theForceModel); 

