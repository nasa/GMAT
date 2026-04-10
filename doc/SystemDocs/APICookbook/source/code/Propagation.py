#!/bin/python3

# The Propagation chapter example code from the GMAT API Cookbook

from load_gmat import gmat
import matplotlib.pyplot as plt


sat = gmat.Construct("Spacecraft", "LeoSat")
sat.SetField("DateFormat", "UTCGregorian")
sat.SetField("Epoch", "12 Mar 2020 15:00:00.000")
sat.SetField("CoordinateSystem", "EarthMJ2000Eq")
sat.SetField("DisplayStateType", "Keplerian")
sat.SetField("SMA", 7005)
sat.SetField("ECC", 0.008)
sat.SetField("INC", 28.5)
sat.SetField("RAAN", 75)
sat.SetField("AOP", 90)
sat.SetField("TA", 45)
sat.SetField("DryMass", 50)
sat.SetField("Cd", 2.2)
sat.SetField("Cr", 1.8)
sat.SetField("DragArea", 1.5)
sat.SetField("SRPArea", 1.2)

# Create the ODEModel container
fm = gmat.Construct("ForceModel", "TheForces")

# An 8x8 JGM-3 Gravity Model
earthgrav = gmat.Construct("GravityField")
earthgrav.SetField("BodyName","Earth")
earthgrav.SetField("Degree",8)
earthgrav.SetField("Order",8)
earthgrav.SetField("PotentialFile","JGM2.cof")

# Add the force into the ODEModel container
fm.AddForce(earthgrav)

# The Point Masses
moongrav = gmat.Construct("PointMassForce")
moongrav.SetField("BodyName","Luna")
sungrav = gmat.Construct("PointMassForce")
sungrav.SetField("BodyName","Sun")

# Drag using Jacchia-Roberts
jrdrag = gmat.Construct("DragForce")
jrdrag.SetField("AtmosphereModel","JacchiaRoberts")

# Build and set the atmosphere for the model
atmos = gmat.Construct("JacchiaRoberts")
jrdrag.SetReference(atmos)

# Add all of the forces into the ODEModel container
fm.AddForce(moongrav)
fm.AddForce(sungrav)
fm.AddForce(jrdrag)

# Build the propagation container that connect the integrator, force model, and spacecraft together
pdprop = gmat.Construct("Propagator","PDProp")

# Create and assign a numerical integrator for use in the propagation
gator = gmat.Construct("PrinceDormand78", "Gator")
pdprop.SetReference(gator)

# Set some of the fields for the integration
pdprop.SetField("InitialStepSize", 60.0)
pdprop.SetField("Accuracy", 1.0e-12)
pdprop.SetField("MinStep", 0.0)

# Assign the force model to the propagator
pdprop.SetReference(fm)

pdprop.AddPropObject(sat)

# Perform top level initialization
gmat.Initialize()
# Perform the integration subsystem initialization
pdprop.PrepareInternals()

# Refresh the integrator reference
gator = pdprop.GetPropagator()

# Take a 60 second step, showing the state before and after, and start buffering
# Buffers for the data
time = []
pos = []
vel = []

gatorstate = gator.GetState()
t = 0.0
r = []
v = []
for j in range(3):
    r.append(gatorstate[j])
    v.append(gatorstate[j+3])
time.append(t)
pos.append(r)
vel.append(v)

print("Starting state:\n", t, r, v)

# Take a step and buffer it
gator.Step(60.0)
gatorstate = gator.GetState()
t = t + 60.0
r = []
v = []
for j in range(3):
    r.append(gatorstate[j])
    v.append(gatorstate[j+3])
time.append(t)
pos.append(r)
vel.append(v)

print("Propped state:\n", t, r, v)

for i in range(360):
    # Take a step and buffer it
    gator.Step(60.0)
    gatorstate = gator.GetState()
    t = t + 60.0
    r = []
    v = []
    for j in range(3):
        r.append(gatorstate[j])
        v.append(gatorstate[j+3])
    time.append(t)
    pos.append(r)
    vel.append(v)

plt.rcParams['figure.figsize'] = (15, 5)
f1 = plt.figure(1)
positions = plt.plot(time, pos)
f2 = plt.figure(2)
velocities = plt.plot(time, vel)

f1.show()
f2.show()
print("Press Enter")
input()
