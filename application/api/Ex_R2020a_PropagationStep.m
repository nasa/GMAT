% GMAT Application Programmer's Interface Example
%
% Coded by D. Conway. Thinking Systems, Inc.
% Ported to MATLAB by J. McGreevy, Emergent Space Technologies Inc.
%
% This file configures a propagator using the GMAT API and uses it to advance an 
% initial state by 60 seconds.

% Load a force model used for the propagation
Ex_R2020a_BasicFM;

% Build the propagation container class 
pdprop = GMATAPI.Construct("Propagator","PDProp");

% Create and assign a numerical integrator for use in the propagation
gator = GMATAPI.Construct("PrinceDormand78");
pdprop.SetReference(gator);

% Assign the force model imported from BasicFM
pdprop.SetReference(fm);

% Set some of the fields for the integration
pdprop.SetField("InitialStepSize", 60.0);
pdprop.SetField("Accuracy", 1.0e-12);
pdprop.SetField("MinStep", 0.0);

% Perform top level initialization
gmat.gmat.Initialize();

% Setup the spacecraft that is propagated
pdprop.AddPropObject(earthorb);
pdprop.PrepareInternals();

% Refresh the 'gator reference
gator = pdprop.GetPropagator();

% Take a 60 second step, showing the state before and after
gator.GetState()
gator.Step(60);
gator.GetState()
