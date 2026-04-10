
%%  Define spacecraft orbit properties
Sat          = Spacecraft();

%%  Define the spacecraft power configuration
Sat.PowerModel         = 'Solar';  % Options: Solar, Nuclear.
Sat.InitialMaxPower    = 15;  %  kW
Sat.PowerInitialEpoch  = 29580;
Sat.PowerDecayRate     = 0;  % Percent per year decay rate
Sat.PowerMargin        = 0;
Sat.ModelShadows       = 0;
Sat.BusCoeff1          = .25;
Sat.BusCoeff2          = 0;
Sat.BusCoeff3          = 0;
Sat.SolarCoeff1        = 1;
Sat.SolarCoeff2        = 0;
Sat.SolarCoeff3        = 0;
Sat.SolarCoeff4        = 0;
Sat.SolarCoeff5        = 0;
Sat.debugMath          = false();

%%  Configure the Thrusters
aThruster                    = ElectricThruster();
aThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ThrustMassPolynomial';
aThruster.MaximumUsablePower = 7.1;
aThruster.MinimumUsablePower = 0.6;
aThruster.ThrustCoeff1       = 7.282;
aThruster.ThrustCoeff2       = 32.901;
aThruster.ThrustCoeff3       = 0;
aThruster.ThrustCoeff4       = 0;
aThruster.ThrustCoeff5       =  0; %  milli - Newton
aThruster.MassFlowCoeff1     = 1.066;
aThruster.MassFlowCoeff2     = .699;
aThruster.MassFlowCoeff3     = 0;
aThruster.MassFlowCoeff4     = 0;
aThruster.MassFlowCoeff5     = 0; %  milligrams/sec
aThruster.DutyCycle          = 1.0;
aThruster.ThrustScaleFactor  = 1.0;

%%  Configure the Thrusters
bThruster                    = ElectricThruster();
bThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
bThruster.ThrustModel        = 'ThrustMassPolynomial';
bThruster.MaximumUsablePower = 7.1;
bThruster.MinimumUsablePower = 0.6;
bThruster.ThrustCoeff1       = 7.282;
bThruster.ThrustCoeff2       = 32.901;
bThruster.ThrustCoeff3       = 0;
bThruster.ThrustCoeff4       = 0;
bThruster.ThrustCoeff5       =  0; %  milli - Newton
bThruster.MassFlowCoeff1     = 1.066;
bThruster.MassFlowCoeff2     = .699;
bThruster.MassFlowCoeff3     = 0;
bThruster.MassFlowCoeff4     = 0;
bThruster.MassFlowCoeff5     = 0; %  milligrams/sec
bThruster.DutyCycle          = 1.0;
bThruster.ThrustScaleFactor  = 1.0;

%% Configure the finite burn object and spacecraft
aFiniteBurn              = FiniteBurn();
aFiniteBurn.debugMath    = false();
aFiniteBurn.ThrottleLogic = 'HeavisideMaxNumberOfThrusters';
aFiniteBurn.throttleHeavisideSharpness = 10000;

aTank                    = FuelTank();
aTank.FuelMass           = 1554;
aFiniteBurn.SetThrusters({aThruster,bThruster});
%aFiniteBurn.SetThrusters({aThruster});
Sat.SetThrusterSet(aFiniteBurn);
Sat.Tanks                = {aTank};

%% Configure the orbit dynamics model
orbitDynConfig.UseSTM        = 1;
orbitDynConfig.UseSRP        = 0;
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




