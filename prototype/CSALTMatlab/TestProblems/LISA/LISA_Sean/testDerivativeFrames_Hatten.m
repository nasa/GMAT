format long g

% save current directory
currDir = pwd;

% change to gmat dir
binDir = 'C:/GMAT/builds/master_OutOfSourceBuild/bin';
cd(binDir);
   
% Load GMAT into memory
gmatloaded = exist('gmat.gmat');
if gmatloaded == 0
   load_gmat();
end

if gmat.gmat.Exists("FM") == true()
    gmat.gmat.Clear("FM");
end
if gmat.gmat.Exists("EarthMJ2000EqExt") == true()
    gmat.gmat.Clear("EarthMJ2000EqExt");
end
if gmat.gmat.Exists("SunMJ2000EqExt") == true()
    gmat.gmat.Clear("SunMJ2000EqExt");
end
if gmat.gmat.Exists("EarthMJ2000EcExt") == true()
    gmat.gmat.Clear("EarthMJ2000EcExt");
end
if gmat.gmat.Exists("EarthMJ2000EcExt") == true()
    gmat.gmat.Clear("EarthMJ2000EcExt");
end
if gmat.gmat.Exists("spacecraft") == true()
    gmat.gmat.Clear("spacecraft");
end
if gmat.gmat.Exists("scTemp") == true()
    gmat.gmat.Clear("scTemp");
end

EarthMJ2000EqExt = gmat.gmat.Construct("CoordinateSystem", "EarthMJ2000EqExt", "Earth", "MJ2000Eq");
SunMJ2000EqExt = gmat.gmat.Construct("CoordinateSystem", "SunMJ2000EqExt", "Sun", "MJ2000Eq");
EarthMJ2000EcExt = gmat.gmat.Construct("CoordinateSystem", "EarthMJ2000EcExt", "Earth", "MJ2000Ec");

% create a spacecraft just for state transformations
scTemp = gmat.gmat.Construct("Spacecraft", "scTemp");
scTemp = gmat.Spacecraft.SetClass(scTemp);
scTemp.SetField("DateFormat", "UTCGregorian");
scTemp.SetField("Epoch", "20 Jul 2020 12:00:00.000");
scTemp.SetField("CoordinateSystem", "EarthMJ2000EqExt");
scTemp.SetField("DisplayStateType", "Cartesian");

% Orbital state
% scTemp.SetField("SMA", 6600);
% scTemp.SetField("ECC", 0.0);
% scTemp.SetField("INC", 78);
% scTemp.SetField("RAAN", 45);
% scTemp.SetField("AOP", 90);
% scTemp.SetField("TA", 180);
scTemp.SetField("X", 970.304058685899);
scTemp.SetField("Y", -970.304058685899);
scTemp.SetField("Z", -6455.77416481823);
scTemp.SetField("VX", 5.49517999195112);
scTemp.SetField("VY", 5.49517999195112);
scTemp.SetField("VZ", 0);
gmat.gmat.Initialize();

% change state to sun-centered because we want to propagate in a
% sun-centered frame
scTemp.SetField("CoordinateSystem", "SunMJ2000EqExt");
scTemp.SetField("DisplayStateType", "Cartesian");
gmat.gmat.Initialize();

initialStateRVector = scTemp.GetCartesianState();
initialState = zeros(6,1);
for i=1:6
    initialState(i) = initialStateRVector.GetElement(i-1);
end
%initialState

% Spacecraft configuration preliminaries
%initialState = [150000000; 0; 0; 0; 30; 0];
initialState = [71245751.5098882
         -123209792.780125
         -53417620.6136361
          31.3360138207812
          18.2078880658141
          5.50975700865882];
sc = gmat.gmat.Construct("Spacecraft", "spacecraft");
sc = gmat.Spacecraft.SetClass(sc);
sc.SetField("DateFormat", "UTCGregorian");
sc.SetField("Epoch", "20 Jul 2020 12:00:00.000");
sc.SetField("CoordinateSystem", "SunMJ2000EqExt");
sc.SetField("DisplayStateType", "Cartesian");
gmat.gmat.Initialize();
cartStateStrings = ["X", "Y", "Z", "VX", "VY", "VZ"];
for i=1:6
    sc.SetField(cartStateStrings(i), initialState(i));
end

% Force model settings
fm = gmat.gmat.Construct("ForceModel", "FM");
fm = gmat.ODEModel.SetClass(fm);
fm.SetField("CentralBody", "Sun");

earthgrav = gmat.gmat.Construct("PointMassForce");
earthgrav = gmat.PointMassForce.SetClass(earthgrav);
earthgrav.SetField("BodyName", "Earth");

sungrav = gmat.gmat.Construct("PointMassForce");
sungrav = gmat.PointMassForce.SetClass(sungrav);
sungrav.SetField("BodyName", "Sun");

%fm.AddForce(earthgrav);
fm.AddForce(sungrav);

% Setup the state vector used for the force, connecting the spacecraft
psm = gmat.PropagationStateManager();
psm.setSwigOwnership(false()); 
psm.SetObject(sc);
psm.BuildState();

% Finish the object connection
fm.SetPropStateManager(psm);
fm.SetState(psm.GetState());

% Assemble all of the objects together 
gmat.gmat.Initialize();

% Finish force model setup:
% Map the spacecraft state into the model
fm.BuildModelFromMap();

% Load the physical parameters needed for the forces
fm.UpdateInitialData();

% move back to working directory
cd(currDir);

% Now access the state and get the derivative data
%sc.SetField("CoordinateSystem", "EarthMJ2000EcExt");
gmat.gmat.Initialize(); % need this or else crashes on rerun when i set the spacecraft coordinate system
pstate = sc.GetState().GetState()
fm.GetDerivatives(initialState, 0.0);
dv = fm.GetDerivativeArray()

% state converter
converter = gmat.CoordinateConverter();
sunSystem = gmat.CoordinateSystem.SetClass(SunMJ2000EqExt);
earthSystem = gmat.CoordinateSystem.SetClass(EarthMJ2000EqExt);

% convert coordinates and see if it works as i expect
instate = gmat.Rvector6();
for i=1:6
    instate.SetElement(i-1, pstate(i));
end
outstate = gmat.Rvector6();

% get the time at which to do the conversion
sc.SetField("DateFormat", "A1ModJulian");
time = sc.GetEpoch();
gmat.gmat.Initialize();
converter.Convert(time, instate, earthSystem, outstate, sunSystem);
stateSun = zeros(6,1);
for i=1:6
    stateSun(i) = outstate.GetElement(i-1);
end
stateSun

