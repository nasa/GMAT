%% TODO:
%  Include mass flow rate in state vector
%  Update spacecraft and tank at integrator stages.
close all;
%%  Set up prop epoch
propDuration    = .2*86400;
outputStepSize  = 10;

%%  Define spacecraft orbit properties
Sat          = Spacecraft();
Sat.Epoch    = 21548.0299479207;
Sat.DryMass  = 6512;
Sat.Attitude = 1;  % 1 for J2000, 2 for VNB
Sat.OrbitState = [-6515.97236231483
    931.372927012761
    -1066.34795707823
    -0.888689684465484
    -7.58686200413001
    -1.1949439525609];

%%  Define the spacecraft power configuration
Sat.PowerModel         = 'Solar';  % Options: Solar, Nuclear.
Sat.InitialMaxPower    = 7.2124;  %  kW
Sat.PowerInitialEpoch  = 21547.00000039794;
Sat.PowerDecayRate     = 5.123 ;  % Percent per year decay rate
Sat.PowerMargin        = 4.998;
Sat.ModelShadows       = 1.0;
Sat.BusCoeff1          = 0.32;
Sat.BusCoeff2          = 0.0001;
Sat.BusCoeff3          = 0.0001;
Sat.SolarCoeff1        =  1.33;
Sat.SolarCoeff2        = -0.11;
Sat.SolarCoeff3        = -0.12;
Sat.SolarCoeff4        =  0.11;
Sat.SolarCoeff5        = -0.02;

%%  Configure the Thrusters
aThruster                    = ElectricThruster();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ConstantThrustAndIsp';
aThruster.MaximumUsablePower = 7.4;
aThruster.MinimumUsablePower = 0.81;
aThruster.ThrustCoeff1       = 1.92E-06 ;
aThruster.ThrustCoeff2       =  54.05382 ;
aThruster.ThrustCoeff3       = -14.41789;
aThruster.ThrustCoeff4       =  2.96519 ;
aThruster.ThrustCoeff5       = -0.19082; %  milli - Newton
aThruster.MassFlowCoeff1     = 2.13781  ;
aThruster.MassFlowCoeff2     = 0.03211  ;
aThruster.MassFlowCoeff3     =  -0.09956;
aThruster.MassFlowCoeff4     = 0.05717 ;
aThruster.MassFlowCoeff5     = -0.004776; %  milligrams/sec
aThruster.FixedEfficiency    = 0.654321;
aThruster.GravitationalAccel = 9.82;
aThruster.Isp                = 3219.12314;
aThruster.ConstantThrust     = .1243;
aThruster.ThrustCoordSys     = 2;
aThruster.ThrustDirection1   = 1;
aThruster.ThrustDirection2   = 0;
aThruster.ThrustDirection3   = 0;
aThruster.DutyCycle          = .98;
aThruster.ThrustScaleFactor  = .87;

%% Configure the finite burn object and spacecraft
aFiniteBurn              = FiniteBurn();
aTank                    = FuelTank();
aTank.FuelMass           = 567.89;
aFiniteBurn.SetThrusters({aThruster});
Sat.SetThrusterSet(aFiniteBurn);
Sat.Tanks                = {aTank};

%%  Configure the force model
Force.UseSRP         = 0;
Force.UseRelativity  = 0;
Force.STM            = 0;
Force.Drag           = 'None';
Force.SRPModel       = 1;  %  1 for sphere, 2 for SPAD
Force.UseFiniteBurn  = 1;
Force.srpScaleFactor = 1;
Force.Debug          = 0;
Force.bodyMu         = 398600.4415;
Force.RefEpoch       = Sat.Epoch;
Force.debugMath      = false();

%%  Configure integrator and propagate
if Force.STM  && ~Force.UseFiniteBurn
    initialState = [Sat.OrbitState; reshape(eye(6),[36,1])];
elseif Force.STM  && Force.UseFiniteBurn
    initialState = [Sat.OrbitState; Sat.GetTotalMass(); ...
        reshape(eye(6),[36,1])];
elseif ~Force.STM  && Force.UseFiniteBurn
    initialState = [Sat.OrbitState; Sat.GetTotalMass()];
else
    initialState = Sat.OrbitState;
end
odeOptions           = odeset('RelTol',1e-9,'AbsTol',1e-9);

[timeHist, stateHist, data] = ode78_FM('OrbitForce', 0, propDuration, initialState, 1e-11, 0, Force, Sat);

%[timeHist, stateHist, data] = ode113('OrbitForce', [0, propDuration], initialState,odeOptions,Force,Sat);

SEPPropagationPlots
