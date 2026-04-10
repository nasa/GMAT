% prototype driver for lunar orbit stationkeeping (LOSK)

close all
clear all
format long g
clc

% add csalt and snopt to matlab path
snoptPath = 'C:/GMAT/Tutorial/CSALTMATLAB/ThirdParty/snopt';
csaltPath = 'C:/GMAT/CSALT_OutOfSourceBuild_Repo/gmat-git/prototype/CSALTMatlab';
addpath(genpath(snoptPath));
addpath(genpath(csaltPath));

% set things
kepInitialStateDeg = [7500; 0.1; 90; 44; 10; 0]; % initial keplerian state in lunar coordinates (km, km/s)
kepInitialState = kepInitialStateDeg;
for i=3:6
    kepInitialState(i) = deg2rad(kepInitialState(i));
end
initialMass = 150; % kg
Cr = 1; %
SRPArea = 0.5; % m^2
gravParamMoon = 4902.8005821478; % km3/s2

useGmatApi = 0;

if useGmatApi == 1
    %% get the gmat api ready
    autongcDir = 'C:/GMAT/CSALT_OutOfSourceBuild_Repo/gmat-git/prototype/CSALTMatlab/TestProblems/AutoNGC/LunarOrbiterStationKeeping';
    cd(autongcDir)

    % change to gmat dir
    binDir = 'C:/GMAT/builds/master_OutOfSourceBuild/bin';
    cd(binDir);

    % Load GMAT into memory
    gmatloaded = exist('gmat.gmat');
    if gmatloaded == 0
       load_gmat();
    end

    % check if objects exist and delete them if they do
    if gmat.gmat.Exists("sc") == true()
        gmat.gmat.Clear("sc");
    end
    if gmat.gmat.Exists("FM") == true()
        gmat.gmat.Clear("FM");
    end
    if gmat.gmat.Exists("FMMoon2body") == true()
        gmat.gmat.Clear("FMMoon2body");
    end
    if gmat.gmat.Exists("MoonMJ2000Eq") == true()
        gmat.gmat.Clear("MoonMJ2000Eq");
    end
    if gmat.gmat.Exists("MoonBodyInertial") == true()
        gmat.gmat.Clear("MoonBodyInertial");
    end

    % create reference frames
    MoonMJ2000Eq = gmat.gmat.Construct("CoordinateSystem", "MoonMJ2000Eq", "Luna", "MJ2000Eq");
    MoonBodyInertial = gmat.gmat.Construct("CoordinateSystem", "MoonBodyInertial", "Luna", "BodyInertial");
    
    % create a spacecraft and set stuff on it
    sc = gmat.gmat.Construct("Spacecraft", "sc");
    sc = gmat.Spacecraft.SetClass(sc);
    sc.SetField("DateFormat", "UTCGregorian");
    sc.SetField("Epoch", "31 Dec 2021 00:00:00.000");
    sc.SetField("DryMass", initialMass); % initial mass; *** will need to be updated as mass of spacecraft changes to get SRP force correct
    sc.SetField("SRPArea", SRPArea); % m^2
    sc.SetField("Cr", Cr);
    sc.SetField("CoordinateSystem", "MoonBodyInertial");
    sc.SetField("DisplayStateType", "Keplerian");
    
    % set the spacecraft's orbital state
    
    kepStateStrings = ["SMA", "ECC", "INC", "RAAN", "AOP", "TA"];
    for i=1:6
        sc.SetField(kepStateStrings(i), kepInitialStateDeg(i));
    end
    
    % create a force model for the full force and one for just moon 2-body
    % will need to get the difference to plug into MEE EOMs
    fm = gmat.gmat.Construct("ForceModel", "FM");
    fm = gmat.ODEModel.SetClass(fm);
    fm.SetField("CentralBody", "Luna");
    
    fmMoon2body = gmat.gmat.Construct("ForceModel", "FMMoon2body");
    fmMoon2body = gmat.ODEModel.SetClass(fmMoon2body);
    fmMoon2body.SetField("CentralBody", "Luna");
    
    % spherical harmonics for moon (4x4)
    moongrav = gmat.gmat.Construct("GravityField");
    moongrav = gmat.GravityField.SetClass(moongrav);
    moongrav.SetField("BodyName", "Luna");
    moongrav.SetField("PotentialFile", "C:/GMAT/builds/CSALT_OutOfSourceBuild/data/gravity/luna/LP165P.cof");
    moongrav.SetField("Degree", 4);
    moongrav.SetField("Order", 4);
    
    % point masses for third bodies: earth
    earthgrav = gmat.gmat.Construct("PointMassForce");
    earthgrav = gmat.PointMassForce.SetClass(earthgrav);
    earthgrav.SetField("BodyName", "Earth");
    
    % point masses for third bodies: sun
    sungrav = gmat.gmat.Construct("PointMassForce");
    sungrav = gmat.PointMassForce.SetClass(sungrav);
    sungrav.SetField("BodyName", "Sun");
    
    % point mass for moon
    moongravPoint = gmat.gmat.Construct("PointMassForce");
    moongravPoint = gmat.PointMassForce.SetClass(moongravPoint);
    moongravPoint.SetField("BodyName", "Luna");
    
    % SRP
    srp = gmat.gmat.Construct("SolarRadiationPressure");
    srp = gmat.SolarRadiationPressure.SetClass(srp);
    
    % add forces to force models
    %fm.AddForce(moongravPoint)
    fm.AddForce(moongrav);
    fm.AddForce(earthgrav);
    fm.AddForce(sungrav);
    %fm.AddForce(srp);
    
    fmMoon2body.AddForce(moongravPoint);
    
    % Setup the state vector used for the force, connecting the spacecraft
    psm = gmat.PropagationStateManager();
    psm.setSwigOwnership(false()); 
    psm.SetObject(sc);
    psm.BuildState();
    
    psmMoon2body = gmat.PropagationStateManager();
    psmMoon2body.setSwigOwnership(false()); 
    psmMoon2body.SetObject(sc);
    psmMoon2body.BuildState();

    % Finish the object connection
    fm.SetPropStateManager(psm);
    fm.SetState(psm.GetState());
    
    fmMoon2body.SetPropStateManager(psmMoon2body);
    fmMoon2body.SetState(psmMoon2body.GetState());

    % Assemble all of the objects together 
    gmat.gmat.Initialize();

    % Finish force model setup:
    % Map the spacecraft state into the model
    fm.BuildModelFromMap();
    fmMoon2body.BuildModelFromMap();

    % Load the physical parameters needed for the forces
    fm.UpdateInitialData();
    fmMoon2body.UpdateInitialData();
    
    gmat.gmat.Initialize(); % want to call this AFTER fm.UpdateInitialData() because otherwise srp was doing weird things
    
    % go back to original directory
    cd(autongcDir);
    
    % get initial cartesian state
    initialStateRVector = sc.GetCartesianState();
    cartInitialState = zeros(6,1);
    for i=1:6
        cartInitialState(i) = initialStateRVector.GetElement(i-1);
    end
    
%     % test out force model derivatives
%     fm.GetDerivatives(cartInitialState, 0.0);
%     dx = fm.GetDerivativeArray();
%     
%     fmMoon2body.GetDerivatives(cartInitialState, 0.0);
%     dx2body = fmMoon2body.GetDerivativeArray();
%     
%     % change mass
%     sc.SetField("DryMass", 100);
%     gmat.gmat.Initialize();
%     fm.GetDerivatives(cartInitialState, 0.0);
%     dx2 = fm.GetDerivativeArray();
%     
%     
%     % change mass back to original value
%     sc.SetField("DryMass", initialMass);
%     gmat.gmat.Initialize();
%     fm.GetDerivatives(cartInitialState, 0.0);
%     dx3 = fm.GetDerivativeArray();
%     
%     disp(dx(4:6))
%     disp(dx2(4:6))
%     disp(dx3(4:6))
% 
%     
%     % 2-body force from moon; seems to be in same frame as from gmat
%     % exactly the same as 2-body acceleration from gmat
%     dxAnalytical = -gravParamMoon / norm(cartInitialState(1:3))^3 * cartInitialState(1:3);
    
    %%
else
    % set things that the api would have set just for consistency
    sc = [];
    fm = [];
    fmMoon2body = [];
    
    % set up to use mice
    % add MICE to matlab path
    miceSrcPath = 'C:/Utilities/mice/src/mice';
    miceLibPath = 'C:/Utilities/mice/lib';
    addpath(genpath(miceSrcPath));
    addpath(genpath(miceLibPath));
    de430 = 'C:/Utilities/Universe/ephemeris_files/de430.bsp';
    
    cartInitialState = Kep2Cart(kepInitialState,gravParamMoon);
end

% Create  the interface and optimize
propagate = 0;
if propagate == 1
    ngcRunner = AutoNGCLOSKPropagateInterface(cartInitialState,initialMass,sc,fm,fmMoon2body);
    [z,F,xmul,Fmul,info] = ngcRunner.Optimize();
    ngcRunner.PlotSolution();
else
    ngcRunner = AutoNGCLOSKInterface(cartInitialState,initialMass,sc,fm,fmMoon2body);
    [z,F,xmul,Fmul,info] = ngcRunner.Optimize();
    ngcRunner.PlotSolution();
end
