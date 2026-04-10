function [fm_earth,...
     e2s_converter,...
       earthSystem,...
         sunSystem,...
           instate,...
          outstate] = GMAT_API_ForceModel_LISA(stateGuess, configEarth)

% Load GMAT into memory
currDir    = pwd;
gmatloaded = exist('gmat.gmat');
binDir = 'C:\Users\snapier\Desktop\GMAT-R2019a-Windows-x64\bin';%'C:/GMAT/builds/master_OutOfSourceBuild/bin';
cd(binDir);
if gmatloaded == 0
   binDir = 'C:\Users\snapier\Desktop\GMAT-R2019a-Windows-x64\bin';
   cd(binDir);
   load_gmat();
   % move back to working directory
   %cd(currDir);
%end
else
     gmat.gmat.Clear();
end

% Create force model
if gmat.gmat.Exists("FM") == false()
    
    % Coordinate System
    EJ2000Eq = gmat.gmat.Construct("CoordinateSystem", "EJ2000Eq", "Earth", "MJ2000Eq");

    %--- LISA1
    LISA1 = gmat.gmat.Construct("Spacecraft", "LISA1");
    LISA1 = gmat.Spacecraft.SetClass(LISA1);
    LISA1.SetField("DateFormat", "A1ModJulian");
    LISA1.SetField("Epoch", "34114.99962963");
    LISA1.SetField("CoordinateSystem", "EJ2000Eq");
    LISA1.SetField("DisplayStateType", "Cartesian");
    
    % Orbital state 1
    LISA1.SetField( "X", stateGuess(1,2)*configEarth.DU);
    LISA1.SetField( "Y", stateGuess(1,3)*configEarth.DU);
    LISA1.SetField( "Z", stateGuess(1,4)*configEarth.DU);
    LISA1.SetField("VX", stateGuess(1,5)*configEarth.VU);
    LISA1.SetField("VY", stateGuess(1,6)*configEarth.VU);
    LISA1.SetField("VZ", stateGuess(1,7)*configEarth.VU);
    
    % Force model settings
   fm_earth = gmat.gmat.Construct("ForceModel", "FM");
   fm_earth = gmat.ODEModel.SetClass(fm_earth);
   fm_earth.SetField("CentralBody", "Earth");
   
   % Add forces to force model
   earthgrav = gmat.gmat.Construct("PointMassForce");
   earthgrav = gmat.PointMassForce.SetClass(earthgrav);
   earthgrav.SetField("BodyName", "Earth");
   fm_earth.AddForce(earthgrav);
   
   sungrav = gmat.gmat.Construct("PointMassForce");
   sungrav = gmat.PointMassForce.SetClass(sungrav);
   sungrav.SetField("BodyName", "Sun");
   fm_earth.AddForce(sungrav);

   %vengrav = gmat.gmat.Construct("PointMassForce");
   %vengrav = gmat.PointMassForce.SetClass(vengrav);
   %vengrav.SetField("BodyName", "Venus");
   %fm_earth.AddForce(vengrav);
   
%    lunagrav = gmat.gmat.Construct("PointMassForce");
%    lunagrav = gmat.PointMassForce.SetClass(lunagrav);
%    lunagrav.SetField("BodyName", "Luna");
%    fm_earth.AddForce(lunagrav);

   % Setup the state vector used for the force, connecting the spacecraft
   psm = gmat.PropagationStateManager();
   psm.setSwigOwnership(false()); 
   psm.SetObject(LISA1);
   psm.BuildState();
   
   % Finish the object connection
   fm_earth.SetPropStateManager(psm);
   fm_earth.SetState(psm.GetState());
   
   % Assemble all of the objects together
   gmat.gmat.Initialize()
   
   % Finish force model setup:
   fm_earth.BuildModelFromMap(); %  Map the spacecraft state into the model
   fm_earth.UpdateInitialData(); % Load the physical parameters needed for the forces
   
   % Test
   %    pstate = [stateGuess(1,1:3)'*configEarth.DU; stateGuess(1,4:6)'*configEarth.VU];
   %    fm_earth.GetDerivatives(pstate);
   %    dx = fm_earth.GetDerivativeArray();
   
   %% Test reporting a state in a different coordinate system
   
   % Initialize data
   SunJ2000Ec    = gmat.gmat.Construct("CoordinateSystem", "SunJ2000Ec", "Sun", "MJ2000Ec");
   earthSystem   = gmat.CoordinateSystem.SetClass(EJ2000Eq);
   sunSystem     = gmat.CoordinateSystem.SetClass(SunJ2000Ec);
   instate       = gmat.Rvector6();
   outstate      = gmat.Rvector6();
   e2s_converter = gmat.CoordinateConverter();
   
   gmat.gmat.Initialize();
   
   %time          = LISA1.GetEpoch();
   %stateSun      = zeros(6,1);
   
   % Get the time at which to do the conversion
   %    for i=1:6
   %        instate.SetElement(i-1, pstate(i));
   %    end
   
   % Convert coordinates
   %    e2s_converter.Convert(time, instate, earthSystem, outstate, sunSystem);
   %    for i=1:6
   %        stateSun(i) = outstate.GetElement(i-1);
   %    end
   %    stateSun;
end

cd(currDir);











































