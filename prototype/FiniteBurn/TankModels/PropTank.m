
%-----------------------------------------------
%     Set Up Properties of Tank 
%-----------------------------------------------
Pt = 400*6894.8;
DutyCycle = 30/360;
Tank.mdote = DutyCycle*20/220/Pt;
Tank.GasType     = 'Helium';
Tank.LiquidType  = 'N2O4';
Tank.ThermoModel = 'HeatTransfer';
Tank.Vt = 0.713;
Tank.Pt = Pt;
Tank.Ta = 0;
Tank.Tl = 294;
Tank.Tg = 294;
Tank.Tw = 294;
Tank.ml = 907;
Tank.mw = 13.6;
Tank.TPoly =   [-3.3221e-011  4.2147e-004 66.0750 ];
Tank.IspPoly = [2.6877e-011 -5.8779e-006  220.5800  ];

%-----------------------------------------------
%     Initialize the Tank 
%-----------------------------------------------
Tank = BlowDownTank_Initialize(Tank);

%-----------------------------------------------
%     Set Initial State Vector
%-----------------------------------------------
X0 = BlowDownTank_GetState(Tank);

%-----------------------------------------------
%     Propagate the Tank State
%-----------------------------------------------
ODEopt = odeset('RelTol', 1e-10, 'AbsTol', 1e-10);
[t,X] = ode45('TankSystemRHS',[0:10:350],X0,ODEopt,Tank);