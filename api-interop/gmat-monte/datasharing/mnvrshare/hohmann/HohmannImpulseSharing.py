#!/usr/bin/env mpython

#=======================================================================
# Filename:     ManeuverPlanSharing_ImpulsiveBurns.py
# Description:  Python script used in the Impulsive Burn configuration 
#
# Date         Author               Notes
# -----------  -------------------  -----------------------------------
# 10/02/2020   Darrel J. Conway     Initial Version
#
#=======================================================================

#=======================================================================
# Load modules and set preferences
#=======================================================================

# Load a basic set of Monte modules to use 
import Monte as M
import mpy.io.data as defaultData
import mpy.traj.force.grav.basic as basicGrav
from mpy.units import *
import cristo

# XY Plotting
from PlotOrbit import *

# Load the GMAT Modules
from load_gmat import *

# Load the GMAT-Monte connection functions
import GmatMonte as GM

# Run the Monte and GMAT configuration script
exec(open("HohmannImpulseSharing_Setup.py").read())

###
### Model TOI in Monte
###

# Use Monte to find the second perigee
p1 = ApsisEvent( TrajQuery(boa,satName,"Earth","EME2000"), ApsisEvent.PERIAPSIS )
p = CountEvent( EpochEvent(t0), p1, 2 )

# Define the VNB frame - for Monte, this is a BVN frame
BodyVelDirFrame( boa, "BVN Frame", "EME2000", TimeInterval(),
                 satName, "Earth" )

# Set the transfer orbit delta-V
dv1 = 1400

# Build and model the TOI burn in Monte
toiDV = Unit3Vec( ( 0 * m/s, dv1 * m/s, 0 * m/s ) )
toiburn_m = ImpulseBurn( "TOI", p, "BVN Frame", toiDV, 300 * sec )

# Insert the TOI burn into the impulsive burn manager
ibm.clear()
ibm.insert( toiburn_m )

# Prop for one day
tf = t0 + 1*day

divaProp.setupModels()
divaProp.writeOutput( t0, tf, boa )

###
### Import Monte burn to GMAT
###

# Build the GMAT TOI burn using the Monte burn
toiBurn_g = GM.M2G_BvnBurn(toiburn_m, boa, "FuelTank")
toiBurn_g.SetReference(mysat)

# Find the time needed to propagate to TOI
toiEpoch = GM.M2G_BurnEpoch(boa, "TOI")
dt = GF.FindTimeDifference(startEpoch, toiEpoch)

# Setup the mission orbit insertion burn so only need to init GMAT once
moiBurn_g = gmat.Construct("ImpulsiveBurn","MOI")

# Set the burn to deplete mass from the tank
moiBurn_g.SetField("DecrementMass",True)
moiBurn_g.SetField("Tank","FuelTank")
moiBurn_g.SetField("Isp",300.0)
moiBurn_g.SetReference(mysat)

gmat.Initialize()

# Propagate, apply the impulse, and propagate to apogee
while dt > 0.001:    # Workaround a bug in GmatFunctions.PropForInterval
   if dt > 60.0:
      step = 60.0
   else:
      step = dt
   GF.PropForInterval(mysat, pdProp, step)
   dt -= 60.0

toiBurn_g.Fire()

# Calculate the prop time and the burn needed to circularize the orbit
sma = mysat.GetNumber("SMA")
# Step half an orbit period:
mu = 398600.4415
dt = math.pi / sqrt(mu) * sqrt(sma**3)
GF.PropForInterval(mysat, pdProp, dt)

# Calculate the burn needed to circularize the orbit
r = [mysat.GetNumber("X"), mysat.GetNumber("Y"), mysat.GetNumber("Z")]
v = [mysat.GetNumber("VX"), mysat.GetNumber("VY"), mysat.GetNumber("VZ")]

rmag = math.sqrt(r[0]**2 + r[1]**2 + r[2]**2)
vmag = math.sqrt(v[0]**2 + v[1]**2 + v[2]**2)

vf = math.sqrt(mu / rmag)
dv = vf - vmag
moiBurn_g.SetField("Element1", dv)

# Save the apogee epoch
moiEpoch = mysat.GetField("Epoch")

# Apply MOI and (optionally) propagate
moiBurn_g.Fire()

# Build the burn for Monte
moiburn_m = GM.G2M_VnbBurn(moiBurn_g, moiEpoch, boa, satName)

# Reset the burns in the impulsive burn manager
ibm.clear()
ibm.insert( toiburn_m )
ibm.insert( moiburn_m )

divaProp.setupModels()
divaProp.writeOutput( t0, tf, boa )

plotOrbit( boa, satName, "TOI from Monte, MOI from GMAT", t0, tf )

print("Event Summary from Monte:")
cristo.events(boa)