

%%  Define spacecraft orbit properties
Sat          = Spacecraft();
Sat.Epoch    = 21548.0299479207;
Sat.DryMass  = 850;
Sat.Attitude = 1;  % 1 for J2000, 2 for VNB
Sat.OrbitState = [-6515.97236231483
    931.372927012761
    -1066.34795707823
    -0.888689684465484
    -7.58686200413001
    -1.1949439525609];

%%  Define the spacecraft power configuration
Sat.PowerModel         = 'Solar';  % Options: Solar, Nuclear.
Sat.InitialMaxPower    = 1.2124;  %  kW
Sat.PowerInitialEpoch  = 21547.00000039794;
Sat.PowerDecayRate     = 5.123 ;  % Percent per year decay rate
Sat.PowerMargin        = 4.998;
Sat.ModelShadows       = 0;
Sat.BusCoeff1          = 0.32;
Sat.BusCoeff2          = 0.0001;
Sat.BusCoeff3          = 0.0001;
Sat.SolarCoeff1        =  1.33;
Sat.SolarCoeff2        = -0.11;
Sat.SolarCoeff3        = -0.12;
Sat.SolarCoeff4        =  0.11;
Sat.SolarCoeff5        = -0.02;
Sat.debugMath          = false();

%%  Configure the Thrusters
aThruster                    = ElectricThruster();
aThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ConstantThrustAndIsp';
aThruster.MaximumUsablePower = 7.4;
aThruster.MinimumUsablePower = 0.31;
aThruster.FixedEfficiency    = 0.654321;
aThruster.GravitationalAccel = 9.82;
aThruster.Isp                = 3219.12314;
aThruster.ConstantThrust     = 0.01243;
aThruster.ThrustCoordSys     = 3;
aThruster.ThrustDirection1   = 1;
aThruster.ThrustDirection2   = 1;
aThruster.ThrustDirection3   = 1;
aThruster.DutyCycle          = .98;
aThruster.ThrustScaleFactor  = .87;

%% Configure the finite burn object and spacecraft
aFiniteBurn              = FiniteBurn();
aFiniteBurn.debugMath    = false();
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
Force.SRPModel       = 0;  %  1 for sphere, 2 for SPAD
Force.UseFiniteBurn  = 1;
Force.srpScaleFactor = 1;
Force.Debug          = 0;
Force.bodyMu         = 398600.4415;
Force.RefEpoch       = Sat.Epoch;
Force.debugMath      = false();