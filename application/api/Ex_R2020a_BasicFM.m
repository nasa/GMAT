% GMAT Application Programmer's Interface Example
%
% Build for GMAT R2020a.  Updated as needed.
%
% Coded by D. Conway. Thinking Systems, Inc.
% Ported to MATLAB by J. McGreevy, Emergent Space Technologies Inc.
%
% This file configures the dynamics for the propagation examples. 
%
% BasicForceModel.py provides a more complete example for users looking for 
% access to GMAT's dynamics outside of the propagation domain. 

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

% Add force into the dynamics model.  Here we pass ownership to the force model
fm.AddForce(earthgrav);

% Assemble all of the objects together 
gmat.gmat.Initialize();
