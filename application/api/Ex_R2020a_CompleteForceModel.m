% GMAT Application Programmer's Interface Example
%
% Coded by D. Conway. Thinking Systems, Inc.
% Ported to MATLAB by J. McGreevy, Emergent Space Technologies Inc.
%
% This file configures a dynamics model using the GMAT API.  The model built
% here contains an Earth 8x8 gravity model, point mass gravity from the Sun and 
% Moon, Jacchia-Roberts drag, and solar radiation pressure providing one way of 
% solving the configuration exercises in the use case 1 tutorial in the GMAT API 
% User's Guide.

% Load GMAT into memory
load_gmat();

% Spacecraft configuration preliminaries
earthorb = GMATAPI.Construct("Spacecraft", "EarthOrbiter");
earthorb.SetField("DateFormat", "UTCGregorian");
earthorb.SetField("Epoch", "20 Jul 2020 12:00:00.000");

earthorb.SetField("CoordinateSystem", "EarthMJ2000Eq");
earthorb.SetField("DisplayStateType", "Keplerian");

% Orbital state
earthorb.SetField("SMA", 6600);
earthorb.SetField("ECC", 0.05);
earthorb.SetField("INC", 78);
earthorb.SetField("RAAN", 45);
earthorb.SetField("AOP", 90);
earthorb.SetField("TA", 180);

% Spacecraft ballistic properties for the SRP and Drag models
earthorb.SetField("SRPArea", 2.5);
earthorb.SetField("Cr", 1.75);
earthorb.SetField("DragArea", 1.8);
earthorb.SetField("Cd", 2.1);
earthorb.SetField("DryMass", 80);

% Force model settings
fm = GMATAPI.Construct("ForceModel", "FM");
fm.SetField("CentralBody", "Earth");

% An 8x8 JGM-3 Gravity Model.  No name set, so management resides with the user
earthgrav = GMATAPI.Construct("GravityField");
earthgrav.SetField("BodyName","Earth");
earthgrav.SetField("PotentialFile","../data/gravity/earth/JGM3.cof");
earthgrav.SetField("Degree",8);
earthgrav.SetField("Order",8);

% The Point Masses
moongrav = GMATAPI.Construct("PointMassForce");
moongrav.SetField("BodyName","Luna");
sungrav = GMATAPI.Construct("PointMassForce");
sungrav.SetField("BodyName","Sun")

% Drag using Jacchia-Roberts
jrdrag = GMATAPI.Construct("DragForce");
jrdrag.SetField("AtmosphereModel","JacchiaRoberts");

% Build and set the atmosphere for the model
atmos = GMATAPI.Construct("JacchiaRoberts", "Atmos");
jrdrag.SetReference(atmos);

% Solar Radiation Pressure
srp = GMATAPI.Construct("SolarRadiationPressure");


% Add force into the dynamics model.  Here we pass ownership to the force model
fm.AddForce(earthgrav);
fm.AddForce(moongrav);
fm.AddForce(sungrav);
fm.AddForce(srp);

% Note that the drag force is commented out because of a bug in the drag models 
% when the A-Matrix is computed.  Builds that have issue GMT-6950 resolved can 
% use the A-Matrix and drag together.
%fm.AddForce(jrdrag);

% Setup the state vector used for the force, connecting the spacecraft
psm = gmat.PropagationStateManager();
psm.SetObject(earthorb);

% Include the orbital A-matrix
psm.SetProperty("AMatrix");

psm.BuildState();

% Finish the object connection
fm.SetPropStateManager(psm);
fm.SetState(psm.GetState());

% Assemble all of the objects together 
gmat.gmat.Initialize();

% Finish force model setup:
%%  Map the spacecraft state into the model
fm.BuildModelFromMap();
%% Load the physical parameters needed for the forces
fm.UpdateInitialData();

% Now access the state and get the derivative data
pstate = earthorb.GetState().GetState();
fm.GetDerivatives(pstate);
dv = fm.GetDerivativeArray()

vec = fm.GetDerivativesForSpacecraft(earthorb)
