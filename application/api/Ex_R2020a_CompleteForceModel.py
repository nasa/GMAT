# GMAT Application Programmer's Interface Example
#
# Coded by D. Conway. Thinking Systems, Inc.
#
# This file configures a dynamics model using the GMAT API.  The model built
# here contains an Earth 8x8 gravity model, point mass gravity from the Sun and 
# Moon, Jacchia-Roberts drag, and solar radiation pressure providing one way of 
# solving the configuration exercises in the use case 1 tutorial in the GMAT API 
# User's Guide.

# Load GMAT into memory
from load_gmat import *

# Spacecraft configuration preliminaries
earthorb = gmat.Construct("Spacecraft", "EarthOrbiter")
earthorb.SetField("DateFormat", "UTCGregorian")
earthorb.SetField("Epoch", "20 Jul 2020 12:00:00.000")

earthorb.SetField("CoordinateSystem", "EarthMJ2000Eq")
earthorb.SetField("DisplayStateType", "Keplerian")

# Orbital state
earthorb.SetField("SMA", 6600)
earthorb.SetField("ECC", 0.05)
earthorb.SetField("INC", 78)
earthorb.SetField("RAAN", 45)
earthorb.SetField("AOP", 90)
earthorb.SetField("TA", 180)

# Spacecraft ballistic properties for the SRP model
earthorb.SetField("SRPArea", 2.5)
earthorb.SetField("Cr", 1.8)
earthorb.SetField("DryMass", 80)

# ODE Model settings
fm = gmat.Construct("ForceModel", "FM")
fm.SetField("CentralBody", "Earth")

# The 8x8 JGM-3 Gravity Model
earthgrav = gmat.Construct("GravityField", "Earth8x8")
earthgrav.SetField("BodyName","Earth")
earthgrav.SetField("PotentialFile","../data/gravity/earth/JGM3.cof")
earthgrav.SetField("Degree",8)
earthgrav.SetField("Order",8)

# The Point Masses
moongrav = gmat.Construct("PointMassForce", "MoonGrav")
moongrav.SetField("BodyName","Luna")
sungrav = gmat.Construct("PointMassForce", "SunGrav")
sungrav.SetField("BodyName","Sun")

# Drag using Jacchia-Roberts
jrdrag = gmat.Construct("DragForce", "JRDrag")
jrdrag.SetField("AtmosphereModel","JacchiaRoberts")

# Build and set the atmosphere for the model
atmos = gmat.Construct("JacchiaRoberts", "Atmos")
jrdrag.SetReference(atmos)

# Solar Radiation Pressure
srp = gmat.Construct("SolarRadiationPressure", "SRP")


# Add all of the forces into the ODEModel container
fm.AddForce(earthgrav)
fm.AddForce(moongrav)
fm.AddForce(sungrav)
fm.AddForce(srp)

# Note that the drag force is commented out because of a bug in the drag models 
# when the A-Matrix is computed.  Builds that have issue GMT-6950 resolved can 
# use the A-Matrix and drag together.
#fm.AddForce(jrdrag)

# Setup the state vector used for the force
psm = gmat.PropagationStateManager()
psm.SetObject(earthorb)

# Include the orbital A-matrix
psm.SetProperty("AMatrix")

psm.BuildState()

fm.SetPropStateManager(psm)
fm.SetState(psm.GetState())

# Assemble all of the objects together 
gmat.Initialize()

# Finish force model setup:
##  Map spacecraft state into the model
fm.BuildModelFromMap()
##  Load physical parameters needed for the forces
fm.UpdateInitialData()

# Now access state and get derivative data
pstate = earthorb.GetState().GetState()
print("State Vector: ", pstate)
print()

fm.GetDerivatives(pstate)
dv = fm.GetDerivativeArray()
print("Derivative:   ", dv)
print()

vec = fm.GetDerivativesForSpacecraft(earthorb)
print("SCDerivative: ", vec)
print()
