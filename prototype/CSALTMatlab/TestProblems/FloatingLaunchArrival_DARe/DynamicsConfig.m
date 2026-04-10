
%%  Define spacecraft orbit properties
Sat          = Spacecraft();
Sat.Epoch    = 29366.400011573;
Sat.DryMass  = 1064.2;
Sat.Attitude = 1;  % 1 for J2000, 2 for VNB
Sat.OrbitState = [-6515.97236231483
    931.372927012761
    -1066.34795707823
    -0.888689684465484
    -7.58686200413001
    -1.1949439525609];

%%  Define the spacecraft power configuration
Sat.PowerModel         = 'Solar';  % Options: Solar, Nuclear.
Sat.InitialMaxPower    = 17.3162;  %  kW
Sat.PowerInitialEpoch  = 29366.400011573;
Sat.PowerDecayRate     = 0;  % Percent per year decay rate
Sat.PowerMargin        = 15;
Sat.ModelShadows       = 0;
Sat.BusCoeff1          = 0.789;
Sat.BusCoeff2          = 0;
Sat.BusCoeff3          = 0;
Sat.SolarCoeff1        =  1.09351;
Sat.SolarCoeff2        = -0.0483642;
Sat.SolarCoeff3        = -0.145066;
Sat.SolarCoeff4        =  -0.118771;
Sat.SolarCoeff5        = 0.0183965;
Sat.debugMath          = false();

%%  Configure the Thrusters
aThruster                    = ElectricThruster();
aThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ThrustMassPolynomial';
aThruster.MaximumUsablePower = 7.36;
aThruster.MinimumUsablePower = 0.64;
aThruster.ThrustCoeff1       = 1.19388817E-02*1000;
aThruster.ThrustCoeff2       = 1.60989424E-02*1000;
aThruster.ThrustCoeff3       = 1.14181412E-02*1000;
aThruster.ThrustCoeff4       = -2.04053417E-03*1000;
aThruster.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
aThruster.MassFlowCoeff1     = 2.75956482E-06*1e6;
aThruster.MassFlowCoeff2     = -1.71102132E-06*1e6;
aThruster.MassFlowCoeff3     = 1.21670237E-06*1e6;
aThruster.MassFlowCoeff4     = -2.07253445E-07*1e6;
aThruster.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec
aThruster.FixedEfficiency    = 0.654321;
aThruster.GravitationalAccel = 9.82;
aThruster.Isp                = 3219.12314;
aThruster.ConstantThrust     = 0.01243;
aThruster.ThrustCoordSys     = 2;
aThruster.ThrustDirection1   = 1;
aThruster.ThrustDirection2   = 0;
aThruster.ThrustDirection3   = 0;
aThruster.DutyCycle          = .90;
aThruster.ThrustScaleFactor  = 1.0;

%% Configure the finite burn object and spacecraft
aFiniteBurn              = FiniteBurn();
aFiniteBurn.debugMath    = false();
aTank                    = FuelTank();
aTank.FuelMass           = 567.89;
aFiniteBurn.SetThrusters({aThruster});
Sat.SetThrusterSet(aFiniteBurn);
Sat.Tanks                = {aTank};

%% Configure the orbit dynamics model
orbitDynConfig.UseSTM        = 1;
orbitDynConfig.UseSRP        = 1;
orbitDynConfig.UseRelativity = 0;
orbitDynConfig.Debug         = 0;
orbitDynConfig.UseFiniteBurn = 1;
orbitDynConfig.ComputeAMatrix = 1;
orbitDynConfig.CentralBodyIdx = 11;
orbitDynConfig.PointMassIds = [];
orbitDynConfig.Drag          = 'None';


%%  Initialize and call the force model
SEPForces = ForceModel();
SEPForces.Initialize(orbitDynConfig,Sat)




