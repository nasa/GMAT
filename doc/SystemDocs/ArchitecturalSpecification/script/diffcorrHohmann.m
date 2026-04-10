%-----------------------------------------------------------------
%------------------ Create core objects --------------------------
%-----------------------------------------------------------------
Create Spacecraft sat;

Create ForceModel DefaultProp_ForceModel;
GMAT DefaultProp_ForceModel.PrimaryBodies = {Earth};

Create Propagator DefaultProp;
GMAT DefaultProp.FM = DefaultProp_ForceModel;

Create ImpulsiveBurn TOI;
GMAT TOI.Axes = VNB;
Create ImpulsiveBurn GOI;
GMAT GOI.Axes = VNB;

%-----------------------------------------------------------------
%---------------- Create and Setup the Targeter ------------------
%-----------------------------------------------------------------
Create DifferentialCorrector DC;
GMAT DC.TargeterTextFile = targeter_DefaultDC.data;
GMAT DC.MaximumIterations = 25;
GMAT DC.UseCentralDifferences = false;

%-----------------------------------------------------------------
%------------------- Create and Setup a plot ---------------------
%-----------------------------------------------------------------
Create XYPlot watchTargeter;
GMAT watchTargeter.IndVar = sat.A1ModJulian;
GMAT watchTargeter.Add = {sat.RMAG};
GMAT watchTargeter.Grid = On;
GMAT watchTargeter.TargetStatus = On;

%*****************************************************************
%-------------------The Mission Sequence--------------------------
%*****************************************************************

%  The targeting sequences below demonstrates how to use a
%  differential corrector in GMAT to construct a Hohmann transfer
%  between two circular, co-planar orbits by targeting first one
%  maneuver to raise.apogee, and then a second maneuver to
%  circularize.

%  Start by spending some time in the initial orbit
Propagate DefaultProp(sat, {sat.ElapsedSecs = 86400});
Propagate DefaultProp(sat, {sat.Periapsis});

%  Target the apogee raising maneuver
Target DC;
   Vary DC(TOI.V = 0.5, {Pert = 0.0001, MaxStep = 0.2, Lower = 0, Upper = 3.14159});
   Maneuver TOI(sat);
   Propagate DefaultProp(sat, {sat.Apoapsis});
   Achieve DC(sat.Earth.RMAG = 42165, {Tolerance = 0.1});
EndTarget;  % For targeter DC

%  Propagate for 1.5 orbits on the transfer trajectory
Propagate DefaultProp(sat, {sat.Periapsis});
Propagate DefaultProp(sat, {sat.Apoapsis});

%  Target the circularizing maneuver
Target DC;
   Vary DC(TOI.V = 0.5, {Pert = 0.0001, MaxStep = 0.2, Lower = 0, Upper = 3.14159});
   Maneuver TOI(sat);
   Propagate DefaultProp(sat, {sat.Periapsis});
   Achieve DC(sat.Earth.SMA = 42165, {Tolerance = 0.1});
EndTarget;  % For targeter DC

%  Propagate for an additional day
Propagate DefaultProp(sat, {sat.ElapsedSecs = 86400});
