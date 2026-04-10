%-----------------------------------------------------------------
%------------------ Create core objects --------------------------
%-----------------------------------------------------------------
Create Spacecraft Sat;
...
Create ForceModel DefaultProp_ForceModel;
...
Create Propagator DefaultProp;
GMAT DefaultProp.FM = DefaultProp_ForceModel;
...
Create ImpulsiveBurn dv1;
Create ImpulsiveBurn dv2;
...
%-----------------------------------------------------------------
%-----------------Create and Setup the Optimizer------------------
%-----------------------------------------------------------------
Create fminconOptimizer SQPfmincon
GMAT SQPfmincon.DiffMaxChange = 0.01;    % Real number
GMAT SQPfmincon.DiffMinChange = 0.0001;  % Real number
GMAT SQPfmincon.MaxFunEvals   = 1000;    % Real number
GMAT SQPfmincon.MaxIter       = 250;     % Real number
GMAT SQPfmincon.TolX          = 0.01;    % Real number
GMAT SQPfmincon.TolFun        = 0.0001;  % Real number
GMAT SQPfmincon.DerivativeCheck = Off;   % {On, Off}
GMAT SQPfmincon.Diagnostics     = On;    % {On, Off}
GMAT SQPfmincon.Display         = Iter   % {Iter, Off, Notify, Final}
GMAT SQPfmincon.GradObj         = Off;   % {On, Off}
GMAT SQPfmincon.GradConstr      = Off;   % {On, Off}

%*****************************************************************
%-------------------The Mission Sequence--------------------------
%*****************************************************************

%  The optimization sequence below demonstrates how to use an SQP
%  routine in GMAT to show that the Hohmann transfer is the optimal
%  transfer between two circular, co-planar orbits.
Optimize SQPfmincon

   %  Vary the initial maneuver using the optimizer, and apply the maneuver
   Vary SQPfmincon(dv1.Element1 = 0.4, {Upper = 2.0, Lower = 0.0, cm = 1, cf = 1});
   Maneuver  dv1(Sat);

   %  Vary the tranfer time of flight using the SQP optimizer
   Vary SQPfmincon( TOF = 3600 );
   Propagate DefaultProp(Sat, {Sat.ElapsedSecs = TOF});

   %  Vary the second maneuver using the optimizer, and apply the maneuver
   Vary SQPfmincon(dv2.Element1 = 0.4 , {Upper = 2.0, Lower = 0.0});
   Maneuver  dv2(Sat);

   %  Apply constraints on final orbit, and define cost function
   NonLinearConstraint SQPfmincon(Sat.SMA = 8000);
   NonLinearConstraint SQPfmincon(Sat.ECC = 0);
   Minimize SQPfmincon(dv1.Element1 + dv1.Element2);

EndOptimize
