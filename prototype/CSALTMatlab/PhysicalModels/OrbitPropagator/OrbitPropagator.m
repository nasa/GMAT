

%%  Set up prop epoch
propDuration    = 0.5*86400;
outputStepSize  = 200;

%%  Define spacecraft properties
Sat.initialEpoch = 23158.00037076831 + 0*3600/86400;
Sat.Cd = 2.2;
Sat.Cr = 1.2;
Sat.DragArea = 20;
Sat.SRPArea = 20;
Sat.DryMass = 1000;
initialState = ...
 [-4453.783586000000 
-5038.203756000000
 -426.384456000000
3.831888000000
-2.887221000000     
-6.018232000000];                      

%%  Define the force model
Force.UseSRP        = 1;
Force.Debug         = 0;
Force.Drag          = 'None';
Force.UseRelativity = 0;
Force.deltaAT       = 32;
Force.xp            = -0.083901;
Force.yp            = 0.467420;
Force.deltaUT1      = -0.4709524;
Force.lod           = 0.0004155;
Force.CentralBodyMu = 398600.4415;
Force.flattening    = 0.0033527;
Force.bodyRadius    = 6378.1363;
Force.bodyMu        = 398600.4415;
Force.speedOfLight  = 299792.458;
Force.muSun         = 132712440017.99;
Force.radiusSun     = 695990;
Force.ExponentialDensity.refHeight   = 0;
Force.ExponentialDensity.refDensity  = 1217000000;
Force.ExponentialDensity.scaleHeight = 8.5;
Force.MSISE.F107A   = 150;
Force.MSISE.F107    = 150;
Force.MSISE.Ap      = 15*ones(1,7);
Force.flux          = 1367;
Force.AU            = 149597870.691;
Force.STM           = 0;
Force.RefEpoch = Sat.initialEpoch
Force.UseFiniteBurn = false();

%%  Configure integrator and propagate
odeOptions           = odeset('RelTol',1e-13,'AbsTol',1e-13);
[timeHist,stateHist] = ode113('OrbitForce',[0:300:propDuration],...
                                     initialState, odeOptions, Force, Sat);

%%  Report the data
%plot3(stateHist(:,1),stateHist(:,2),stateHist(:,3))
[m,n] = size(stateHist);
stateHist(m,:)'
    

