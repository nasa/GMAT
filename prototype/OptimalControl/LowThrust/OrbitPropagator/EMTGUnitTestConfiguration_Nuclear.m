

%%  Define spacecraft orbit properties
Sat          = Spacecraft();
Sat.Epoch    = 24402.28458575874 + 652320.00/86400;
Sat.DryMass  = 850;
Sat.Attitude = 1;  % 1 for J2000, 2 for VNB
Sat.OrbitState = [2341849.738043934
    3384138.610692263
    1969898.939753987
    -0.888689684465484
    -7.58686200413001
    -1.1949439525609];

%%  Define the spacecraft power configuration
Sat.PowerModel         = 'Nuclear';  % Options: Solar, Nuclear.
Sat.PowerInitialEpoch  = 24402.28458575874;
Sat.InitialMaxPower    = 5;  %  kW
Sat.PowerDecayRate     = 2 ;  % Percent per year decay rate
Sat.PowerMargin        = 15;
Sat.ModelShadows       = 0;
%  Polynomial is in Kw.
Sat.BusCoeff1          = 0.5;
Sat.BusCoeff2          = 0.01;
Sat.BusCoeff3          = 0.02;
Sat.debugMath          = true();

%%  Configure the Thrusters
aThruster                    = ElectricThruster();
aThruster.debugMath          = true();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ThrustMassPolynomial';
aThruster.MaximumUsablePower = 7;
aThruster.MinimumUsablePower = 0;

%  Polynomial outputs units of milli-Newton
aThruster.ThrustCoeff1       = 3.68945763E-03*1000;
aThruster.ThrustCoeff2       = 4.05432510E-02*1000;
aThruster.ThrustCoeff3       = -7.91621814E-03*1000;
aThruster.ThrustCoeff4       = 1.72548416E-03*1000;
aThruster.ThrustCoeff5       = -1.11563126E-04*1000; %  milli - Newton
%  
aThruster.MassFlowCoeff1     = 2.22052155E-06*1e6 ;
aThruster.MassFlowCoeff2     = -1.80919262E-07*1e6  ;
aThruster.MassFlowCoeff3     = 2.77715756E-08*1e6;
aThruster.MassFlowCoeff4     = 2.98873982E-08*1e6 ;
aThruster.MassFlowCoeff5     = -2.91399146E-09*1e6; %  milligrams/sec

aThruster.FixedEfficiency    = 0.7;
aThruster.GravitationalAccel = 9.80665;
aThruster.Isp                = 4000;
aThruster.ConstantThrust     = 0.25;
aThruster.ThrustCoordSys     = 1;
aThruster.ThrustDirection1   = 1;
aThruster.ThrustDirection2   = 0;
aThruster.ThrustDirection3   = 0;
aThruster.DutyCycle          = .90;
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



%%  Configure integrator and propagate
propDuration    = 1*86400;
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
odeOptions           = odeset('RelTol',1e-12,'AbsTol',1e-12);

%[timeHist, stateHist, data] = ode78_FM('OrbitForce', 0, propDuration, initialState, 1e-13, 0, Force, Sat);

[timeHist, stateHist, data] = ode113('OrbitForce', [0:.1:1]*propDuration, initialState,odeOptions,Force,Sat);

%SEPPropagationPlots
truth = [timeHist stateHist];
formatStr = '%16.8f %16.8f  %16.8f  %16.8f  %16.8f  %16.8f %16.8f %16.8f\n';
for rowIdx = 1:size(truth,1)
   data = sprintf(formatStr,truth(rowIdx,:));
   fprintf(fileId,'%s',data);
end
fclose(fileId);
