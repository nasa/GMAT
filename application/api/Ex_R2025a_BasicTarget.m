load_gmat();

GMATAPI.EchoLogFile();

sat = GMATAPI.Construct("Spacecraft", "Sat");
fm = GMATAPI.Construct("ForceModel", "FM");
prop = GMATAPI.Construct("Propagator", "Prop");
burn = GMATAPI.Construct("ImpulsiveBurn", "Burn1");
dc = GMATAPI.Construct("DifferentialCorrector", "DC");

GMATAPI.Command("Target", "DC");
GMATAPI.Command("Vary", "DC(Burn1.Element1 = 0.5, {Perturbation = 0.0001, MaxStep = 0.2})");
GMATAPI.Command("Maneuver", "Burn1(Sat)");
GMATAPI.Command("Propagate", "Prop(Sat) {Sat.Apoapsis}");
GMATAPI.Command("Achieve", "DC(Sat.RMAG = 42165.0, {Tolerance = 0.1})");
GMATAPI.Command("EndTarget");

GMATAPI.Initialize();

status = GMATAPI.Execute();
disp(status);
