

%%  Set up prop epoch
propDuration    = 5*86400;
outputStepSize  = 86400;

%%  Define spacecraft properties

Sat.Cd = 2.2;
Sat.Cr = 1.2;
Sat.DragArea = 20;
Sat.SRPArea = 20;
Sat.DryMass = 1000;

%  for Earth Test Case
Sat.initialEpoch = 23158.00037076831 + 0*3600/86400;
initialState = ...
 [ -4453.783586
-5038.203756
-426.384456
 3.831888
-2.887221
-6.018232];            
Force.bodyMu = 398600.4415;

% %  For Jupiter Test Case
% Sat.initialEpoch = 23158.00037076831 + 0*3600/86400;
% initialState = ...
%  [75141.52946138382 
% -25189.02764719725
%  -10920.78002658486
% 13.65806041275883
% 34.29673840520864     
% 14.86945589954852];      
% Force.bodyMu       = 126712767.8578;

% %  For Sun Test Case
% Sat.initialEpoch = 23158.00037076831 + 0*3600/86400;
% initialState = ...
%  [46001199.99999995
% 0
% 0
% 0
% 67.63529390994103     
% 29.32348866855251];      
% Force.bodyMu       = 132712440017.99;



%%  Define the force model
Force.UseSRP        = 0;
Force.Debug         = 0;
Force.Drag          = 'None';
Force.UseRelativity = 1;
Force.deltaAT       = 32;
Force.xp           = -0.083901;
Force.yp           = 0.467420;
Force.deltaUT1     = -0.4709524;
Force.lod          = 0.0004155;
Force.flattening   = 0.0033527;
Force.bodyRadius   = 6378.1363;
Force.dPsi         = -0.054514;
Force.dEps         = -0.004820;
  

Force.speedOfLight = 299792.458;
Force.muSun        = 132712440017.99;
Force.radiusSun    = 695990;
Force.ExponentialDensity.refHeight   = 0;
Force.ExponentialDensity.refDensity  = 1217000000;
Force.ExponentialDensity.scaleHeight = 8.5;
Force.MSISE.F107A  = 150;
Force.MSISE.F107   = 150;
Force.MSISE.Ap     = 15*ones(1,7);
Force.flux         = 1367;
Force.AU           = 149597870.691;

%%  Configure integrator and propagate
odeOptions           = odeset('RelTol',1e-13,'AbsTol',1e-13);
[timeHist,stateHist] = ode113('OrbitForce',[0:86400:propDuration],...
                                     initialState, odeOptions, Force, Sat);

%%  Report the data
%plot3(stateHist(:,1),stateHist(:,2),stateHist(:,3))
[m,n] = size(stateHist);
stateHist(m,:)'
    

