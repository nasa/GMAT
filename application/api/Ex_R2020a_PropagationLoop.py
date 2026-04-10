# GMAT Application Programmer's Interface Example
#
# Coded by D. Conway. Thinking Systems, Inc.
#
# This file configures a propagator using the GMAT API and uses it to advance an 
# initial state one day in 600 second increments.

# Load GMAT into the Python environment
from load_gmat import *
# Load a force model used for the propagation
from Ex_R2020a_BasicFM import *

# Build the propagation container class 
pdprop = gmat.Construct("Propagator","PDProp")

# Create and assign a numerical integrator for use in the propagation
gator = gmat.Construct("PrinceDormand78", "Gator")
pdprop.SetReference(gator)

# Assign the force model imported from BasicFM
pdprop.SetReference(fm)

# Set some of the fields for the integration
pdprop.SetField("InitialStepSize", 60.0)
pdprop.SetField("Accuracy", 1.0e-12)
pdprop.SetField("MinStep", 0.0)

# Perform top level initialization
gmat.Initialize()

# Setup the spacecraft that is propagated
pdprop.AddPropObject(earthorb)
pdprop.PrepareInternals()

# Refresh the 'gator reference
gator = pdprop.GetPropagator()

# Take a 600 second steps for 1 day, showing the state before and after
time = 0.0
step = 600.0
print(time, " sec, state = ", gator.GetState())

# Propagate for 1 day (via 144 10-minute steps)
for x in range(144):
   gator.Step(step)
   time = time + step
   print(time, " sec, state = ", gator.GetState())
