#!/usr/bin/python3

# Python script demonstrating ephemeris generation from the GMAT API
# 
# Coded 01/17/2020 at Thinking Systems, Inc.

# Load GMAT
import gmatpy as gmat

# Set up a spacecraft for propagation
sat = gmat.Construct("Spacecraft", "DemoSat")
sat.SetField("DateFormat", "UTCGregorian")
sat.SetField("Epoch", "17 Jan 2020 17:00:00.000")
sat.SetField("DisplayStateType", "Keplerian")
sat.SetField("SMA", 6800.0)
sat.SetField("ECC", 0.01)
sat.SetField("INC", 78.0)
sat.SetField("RAAN", 45.0)
sat.SetField("AOP", 87.5)
sat.SetField("TA", 0.0)

# Define the force model and propagator
earthgrav = gmat.Construct("GravityField", "Earth8x8")
earthgrav.SetField("Degree", 8)
earthgrav.SetField("Order", 8)
earthgrav.SetField("PotentialFile","JGM2.cof")


sungrav = gmat.Construct("PointMassForce", "SunPM")
sungrav.SetField("BodyName", "Sun")

moongrav = gmat.Construct("PointMassForce", "MoonPM")
moongrav.SetField("BodyName", "Luna")

srp = gmat.Construct("SolarRadiationPressure", "SRP")

drag = gmat.Construct("DragForce", "JRDrag")
drag.SetField("AtmosphereModel","JacchiaRoberts")
atmos = gmat.Construct("JacchiaRoberts")
drag.SetReference(atmos)

forces = gmat.Construct("ForceModel", "Forces")
forces.AddForce(earthgrav)
forces.AddForce(sungrav)
forces.AddForce(moongrav)
forces.AddForce(drag)
forces.AddForce(srp)

prop = gmat.Construct("Propagator", "Prop")
prop.SetField("FM", "Forces")

# Configure the ephemeris file
ephem = gmat.Construct("EphemerisFile", "Ephem")
ephem.SetField("Spacecraft", "DemoSat")
ephem.SetField("Filename", "DemoSat.bsp")
ephem.SetField("FileFormat", "SPK")

# Create and assign a numerical integrator for use in the propagation
gator = gmat.Construct("PrinceDormand78", "Gator")
prop.SetReference(gator)

# Assign the force model imported from BasicFM
prop.SetReference(forces)

# Setup the spacecraft that is propagated
psm = prop.GetPropStateManager()
psm.SetObject(sat)

# Initialize the system
gmat.Initialize()

#-------------------------------------------------------------------------------
# Propagator gorp that we need to hide
# Refresh the object references from the propagator clones
fm = prop.GetODEModel()
gator = prop.GetPropagator()

psm.BuildState()

# Pass the state manager to the dynamics model
fm.SetPropStateManager(psm)
fm.SetState(psm.GetState())

# Assemble all of the force model objects together 
fm.Initialize()

# Finish the force model setup
fm.BuildModelFromMap()
fm.UpdateInitialData()

# Initialize the Propagator components
prop.Initialize()
gator.Initialize()
#-------------------------------------------------------------------------------


# Drive the propagation
for i in range(1440):
   gator.Step(300)
