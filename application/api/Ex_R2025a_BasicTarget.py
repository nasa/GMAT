from load_gmat import *
import sys
import csv
import os

gmat.EchoLogFile()

sat = gmat.Construct("Spacecraft", "Sat")
fm = gmat.Construct("ForceModel", "FM")
prop = gmat.Construct("Propagator", "Prop")
burn = gmat.Construct("ImpulsiveBurn", "Burn1")
dc = gmat.Construct("DifferentialCorrector", "DC")

gmat.Command("Target", "DC")
gmat.Command("Vary", "DC(Burn1.Element1 = 0.5, {Perturbation = 0.0001, MaxStep = 0.2})")
gmat.Command("Maneuver", "Burn1(Sat)")
gmat.Command("Propagate", "Prop(Sat) {Sat.Apoapsis}")
gmat.Command("Achieve", "DC(Sat.RMAG = 42165.0, {Tolerance = 0.1})")
gmat.Command("EndTarget")

gmat.Initialize()

status = gmat.Execute()
print(status)
