# GMAT Application Programmer's Interface Example
#
# Coded by J. McGreevy, Emergent Space Technologies Inc.
#
# This file configures and fires a range measurement using the GMAT navigation
# subsystem. 


from load_gmat import *

gmat.UseLogFile("PythonLogAPI.txt");

# If the script is rerun, these objects need to be cleared beforehand
gmat.Clear("SimData");
gmat.Clear("GDS");

# Construct the PropSetup to control propagation (for light time)
prop = gmat.Construct("PropSetup", "prop");
prop.GetODEModel().SetField("ErrorControl", "None");
prop.GetPropagator().SetNumber("MinStep", 0);

# Create objects for generating measurement
simsat = gmat.Construct("Spacecraft", "SimSat");
gds = gmat.GroundStation.SetClass(gmat.Construct("GroundStation", "GDS"));

# Configure Spacecraft initial conditions
simsat.SetField("DateFormat", "A1ModJulian");
simsat.SetField("Epoch", "21550");

# Configure GroundStation
gds.SetField("StateType", "Spherical");
gds.SetField("HorizonReference", "Ellipsoid");
gds.SetField("Location1", 0);
gds.SetField("Location2", 90.0);
gds.SetField("Location3", 0);

# Enable these to enable media corrections
# gds.SetField("IonosphereModel", "IRI2007");
# gds.SetField("TroposphereModel", "Marini");

# Create communication hardware
# Hardware for ground station
ant1 = gmat.Antenna.SetClass(gmat.Construct("Antenna", "Antenna1"));
tmit = gmat.Transmitter.SetClass(gmat.Construct("Transmitter", "Transmitter1"));
tmit.SetField("Frequency", 2067.5);
rec = gmat.Receiver.SetClass(gmat.Construct("Receiver", "Receiver1"));

# Hardware for spacecraft
ant2 = gmat.Antenna.SetClass(gmat.Construct("Antenna", "Antenna2"));
tpond = gmat.Transponder.SetClass(gmat.Construct("Transponder", "Transponder1"));
tpond.SetField("TurnAroundRatio","240/221");

# Set fields
# Use Antenna1 for Transmitter1 and Receiver1
tmit.SetField("PrimaryAntenna","Antenna1");
rec.SetField("PrimaryAntenna","Antenna1");

# Use Antenna2 for Transponder1
tpond.SetField("PrimaryAntenna","Antenna2");

# Add Antenna2 and Transponder1 to spacecraft
simsat.SetField("AddHardware","{Antenna2, Transponder1}");

# Add Antenna1, Transmitter1, and Receiver1 to station
gds.SetField("AddHardware","{Antenna1, Transmitter1, Receiver1}");


# Define range measurements and error model
tem = gmat.ErrorModel.SetClass(gmat.Construct("ErrorModel", "TheErrorModel"));
# Specify these measurements are range measurements in km
tem.SetField("Type","Range");
tem.SetField("NoiseSigma", 0.050); # Standard deviation of noise
tem.SetField("Bias",0); # Bias in measurement

# Define doppler range rate measurements and error model
tem2 = gmat.ErrorModel.SetClass(gmat.Construct("ErrorModel", "TheErrorModel2"));
# Specify these measurements are doppler range rate measurements
tem2.SetField("Type","RangeRate");
tem2.SetField("NoiseSigma", 5e-5); # Standard deviation of noise
tem2.SetField("Bias",0); # Bias in measurement

# Add ErrorModels to the ground station
gds.SetField("ErrorModels","{TheErrorModel, TheErrorModel2}");

# Create a TrackingFileSet to manage the observations
tfs = gmat.TrackingFileSet.SetClass(gmat.Construct("TrackingFileSet", "SimData"));
tfs.SetField("FileName","TrkFile_API_GN.gmd"); # Still needed even though it's not written to
tfs.SetField("UseLightTime", False);
tfs.SetField("UseRelativityCorrection", False);
tfs.SetField("UseETminusTAI", False);

# Define signal paths and measurement type(s)
# 2-way measurements are used here along the path GDS -> SimSat -> GDS
# Add range measurements to TrackingFileSet
tfs.SetField("AddTrackingConfig", "{{GDS,SimSat,GDS}, Range}");
# Add doppler range rate measurements to TrackingFileSet
tfs.SetField("AddTrackingConfig", "{{GDS,SimSat,GDS}, RangeRate}");
tfs.SetPropagator(prop); # Tell TrackingFileSet the propagator to use


# Initialize the GMAT objects
gmat.Initialize()

# Calculate the measurement
tdas = tfs.GetAdapters()
numMeas = tdas.size()

tda = tdas[0]
md0 = tda.CalculateMeasurement()
print("GMAT Range Measurement Value:")
print(md0.value[0])


# Make sure this is correct
import numpy as np
satState = simsat.GetState()
gsLoc = gds.GetMJ2000Position(satState.GetEpochGT())[:]
r = np.subtract(gsLoc, satState.GetState()[0:3])
rNorm = np.linalg.norm(r)
print("Numerical Range Measurement Value (no lighttime):")
print(2*rNorm)

print()

xid = simsat.GetParameterID("CartesianX");
tda.CalculateMeasurementDerivatives(simsat,xid);
deriv = [tda.ApiGetDerivativeValue(0,ii) for ii in range(6)]
print("GMAT Range Measurement Derivatives:")
print(deriv)

print()

tda = tdas[1]
md1 = tda.CalculateMeasurement()
print("GMAT RangeRate Measurement Value:")
print(md1.value[0])
