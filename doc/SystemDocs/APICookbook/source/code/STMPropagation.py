#!/bin/python3

# The Propagation chapter example code from the GMAT API Cookbook

from load_gmat import gmat
import matplotlib.pyplot as plt


# Define core objects

def ConfigureCoreObjects():
    ''' 
    Objects built in the earlier exercise
    '''
    # Configure the spacecraft
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

    return sat, pdprop

def StepAMinute(sat, propagator):
    propagator.Step(60.0)

def UpdateData(propagator, time, pos, vel):
    gatorstate = propagator.GetState()
    if len(time) == 0:
       t = 0.0
    else:
        t = time[len(time) - 1] + 60.0
    r = []
    v = []
    for j in range(3):
        r.append(gatorstate[j])
        v.append(gatorstate[j+3])
    time.append(t)
    pos.append(r)
    vel.append(v)

def ShowStateStmCov(sat):

    print(sat.GetName(), " State and matrix data:\n")
    print("Epoch:  ", sat.GetEpoch())
    print("Position:  ", sat.GetField("X"), ", ", sat.GetField("Y"), ", ", 
        sat.GetField("Z"))
    print("Velocity:  ", sat.GetField("VX"), ", ", sat.GetField("VY"), ", ", 
        sat.GetField("VZ"))

    # Access the state transition matrix data
    stm = sat.GetRmatrixParameter("STM")
    print("\nState Transition Matrix:\n")
    for i in range(6):
        cstr = ""
        for j in range(6):
            cstr = cstr + "    " + str(stm.GetElement(i,j))
        print(cstr)
    print()

    # Access the covariance matrix data
    cov = sat.GetCovariance().GetCovariance()
    print("\nCovariance:\n")
    for i in range(6):
        cstr = ""
        for j in range(6):
            cstr = cstr + "    " + str(cov.GetElement(i,j))
        print(cstr)
    print()


# Load the preliminaries
sat, pdprop = ConfigureCoreObjects()

# ------------------------------------------------------------------------------
# New code
# ------------------------------------------------------------------------------

# Initialize the covariance matrix
sat.SetRealParameter("Covariance", 0.000001, 0, 0)
sat.SetRealParameter("Covariance", 0.0000015, 1, 1)
sat.SetRealParameter("Covariance", 0.000001, 2, 2)
sat.SetRealParameter("Covariance", 2.5e-11, 3, 3)
sat.SetRealParameter("Covariance", 2.5e-11, 4, 4)
sat.SetRealParameter("Covariance", 2.5e-11, 5, 5)

# Setup STM and covariance propagation
psm = pdprop.GetPropStateManager()
psm.SetProperty("Covariance",sat)
psm.SetProperty("STM",sat)

# Perform top level and integrator initialization
gmat.Initialize()
pdprop.PrepareInternals()
propagator = pdprop.GetPropagator()

# Push the data into the spacecraft runtime elements
propagator.UpdateSpaceObject()

# Matplotlib graphics buffers
time = []
pos = []
vel = []

# Save initial state information into the graphics buffers
UpdateData(propagator, time, pos, vel)

# Show initial information
ShowStateStmCov(sat)

# Propagate 10 minutes and buffer data at each step
for i in range(10):
    StepAMinute(sat, propagator)
    # Push the propagation data onto the spacecraft
    propagator.UpdateSpaceObject()
    UpdateData(propagator, time, pos, vel)

# Show the results
plt.rcParams['figure.figsize'] = (15, 5)
f1 = plt.figure(1)
positions = plt.plot(time, pos)
f2 = plt.figure(2)
velocities = plt.plot(time, vel)

f1.show()
f2.show()

ShowStateStmCov(sat)

print("Press Enter")
input()
