% GMAT Application Programmer's Interface Example
%
% Coded by J. McGreevy, Emergent Space Technologies Inc.
%
% This file configures and fires a range measurement using the GMAT navigation
% subsystem. 

clear; clc;
load_gmat;

gmat.gmat.UseLogFile("MATLABLogAPI.txt");

% If the script is rerun, these objects need to be cleared beforehand
gmat.gmat.Clear("SimData");
gmat.gmat.Clear("GDS");

% Construct the PropSetup to control propagation (for light time)
prop = GMATAPI.Construct("PropSetup", "prop");
prop.GetODEModel().SetField("ErrorControl", "None");
prop.GetPropagator().SetNumber("MinStep", 0);

% Create objects for generating measurement
simsat = GMATAPI.Construct("Spacecraft", "SimSat");
gds = GMATAPI.Construct("GroundStation", "GDS");

% Configure Spacecraft initial conditions
simsat.SetField("DateFormat", "A1ModJulian");
simsat.SetField("Epoch", "21550");

% Configure GroundStation
gds.SetField("StateType", "Spherical");
gds.SetField("HorizonReference", "Ellipsoid");
gds.SetField("Location1", 0);
gds.SetField("Location2", 90.0);
gds.SetField("Location3", 0);

% Enable these to enable media corrections
% gds.SetField("IonosphereModel", "IRI2007");
% gds.SetField("TroposphereModel", "Marini");

% Create communication hardware
% Hardware for ground station
ant1 = GMATAPI.Construct("Antenna", "Antenna1");
tmit = GMATAPI.Construct("Transmitter", "Transmitter1");
tmit.SetField("Frequency", 2067.5);
rec = GMATAPI.Construct("Receiver", "Receiver1");

% Hardware for spacecraft
ant2 = GMATAPI.Construct("Antenna", "Antenna2");
tpond = GMATAPI.Construct("Transponder", "Transponder1");
tpond.SetField("TurnAroundRatio","240/221");

% Set fields
% Use Antenna1 for Transmitter1 and Receiver1
tmit.SetField("PrimaryAntenna","Antenna1");
rec.SetField("PrimaryAntenna","Antenna1");

% Use Antenna2 for Transponder1
tpond.SetField("PrimaryAntenna","Antenna2");

% Add Antenna2 and Transponder1 to spacecraft
simsat.SetField("AddHardware","{Antenna2, Transponder1}");

% Add Antenna1, Transmitter1, and Receiver1 to station
gds.SetField("AddHardware","{Antenna1, Transmitter1, Receiver1}");


% Define range measurements and error model
tem = GMATAPI.Construct("ErrorModel", "TheErrorModel");
% Specify these measurements are range measurements in km
tem.SetField("Type","Range");
tem.SetField("NoiseSigma", 0.050); % Standard deviation of noise
tem.SetField("Bias",0); % Bias in measurement

% Define doppler range rate measurements and error model
tem2 = GMATAPI.Construct("ErrorModel", "TheErrorModel2");
% Specify these measurements are doppler range rate measurements
tem2.SetField("Type","RangeRate");
tem2.SetField("NoiseSigma", 5e-5); % Standard deviation of noise
tem2.SetField("Bias",0); % Bias in measurement

% Add ErrorModels to the ground station
gds.SetField("ErrorModels","{TheErrorModel, TheErrorModel2}");

% Create a TrackingFileSet to manage the observations
tfs = GMATAPI.Construct("TrackingFileSet", "SimData");
tfs.SetField("FileName","TrkFile_API_GN.gmd"); % Still needed even though it's not written to
tfs.SetField("UseLightTime", false);
tfs.SetField("UseRelativityCorrection", false);
tfs.SetField("UseETminusTAI", false);

% Define signal paths and measurement type(s)
% 2-way measurements are used here along the path GDS -> SimSat -> GDS
% Add range measurements to TrackingFileSet
tfs.SetField("AddTrackingConfig", "{{GDS,SimSat,GDS}, Range}");
% Add doppler range rate measurements to TrackingFileSet
tfs.SetField("AddTrackingConfig", "{{GDS,SimSat,GDS}, RangeRate}");
tfs.SetPropagator(prop); % Tell TrackingFileSet the propagator to use


% Initialize the GMAT objects
gmat.gmat.Initialize()

% Calculate the measurement
tdas = tfs.GetAdapters();
numMeas = tdas.size();

tda = tdas.get(0);
md0 = tda.CalculateMeasurement();
disp("GMAT Range Measurement Value:")
disp(md0.getValue().get(0))


% Make sure this is correct
satState = simsat.GetState();
gsPos = gds.GetMJ2000Position(satState.GetEpochGT()).GetDataVector();
satPos = satState.GetState();
r = gsPos - satPos(1:3);
rNorm = norm(r);
disp("Numerical Range Measurement Value (no lighttime):")
disp(2*rNorm)

disp("")

xid = simsat.GetParameterID("CartesianX");
tda.CalculateMeasurementDerivatives(simsat,xid);
for ii = 0:5
    deriv(ii+1) = tda.ApiGetDerivativeValue(0,ii);
end
disp("GMAT Range Measurement Derivatives:")
disp(deriv)

disp("")

tda = tdas.get(1);
md1 = tda.CalculateMeasurement();
disp("GMAT RangeRate Measurement Value:")
disp(md1.getValue().get(0))
