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

# XYPlotting
from PlotOrbit import *


# Load the GMAT Modules
from load_gmat import *

# Load the GMAT-Monte connection functions
import GmatMonte as GM

# Run the Monte and GMAT configuration script
exec(open("ManeuverPlanSharing_Setup.py").read())

# Setup a GMAT Impulsive burn
toi = gmat.Construct("ImpulsiveBurn","TOI")
# Set the burn to deplete mass from the tank
toi.SetField("DecrementMass",True)
toi.SetField("Tank","Tank")
toi.SetField("Isp",3000)

# Propagate in GMAT to perigee
mu = 398600.4415
sma = mysat.GetNumber("SMA")
period = 2 * math.pi / sqrt(mu) * sqrt(sma**3)
print("Orbit Period:  ", period, " sec")

# Since we start at apogee, propagate a half period to perigee
GF.PropForInterval(mysat, pdProp, period / 2.0, True, True)

# Compute the delta-V needed to raise apogee to GEO distance.  We could target
# this in GMAT, but we'll just do it by hand here
dv = 1.1
toi.SetField("Element1", dv)

toi.SetReference(mysat)
gmat.Initialize()

# Initial position and velocity
pos_i = [mysat.GetNumber("X"), mysat.GetNumber("Y"), mysat.GetNumber("Z")]
vel_i = [mysat.GetNumber("VX"), mysat.GetNumber("VY"), mysat.GetNumber("VZ")]
print("\nBefore the Burn, the position and velocity are ", pos_i, vel_i)

# Apply the impulse
toi.Fire()

# Final position and velocity
pos_f = [mysat.GetNumber("X"), mysat.GetNumber("Y"), mysat.GetNumber("Z")]
vel_f = [mysat.GetNumber("VX"), mysat.GetNumber("VY"), mysat.GetNumber("VZ")]
print("After the Burn, the position and velocity are ", pos_f, vel_f)

print("\nThe spacecraft mass is now ", mysat.GetNumber("TotalMass"), " kg\n")

toiepoch = mysat.GetField("Epoch")
print("TOI occurs in GMAT at  ", toiepoch, " UTC")

# Pass this burn to Monte

# Monte Impulsive Burn Manager
ibm = ImpulseBurnMgr( boa, satName )

# Build the Monte representation of the burn
toiburn = GM.G2M_VnbBurn(toi, toiepoch, boa, satName)

# Insert Burn-1 into the manager
ibm.clear()
ibm.insert( toiburn )
# ibm.insert( goiburn )

tf = t0 + 17280 * sec

# Integrate
forces = [ GravityForce, ImpulseBurnForce, ]
for forceClass in forces:
   divaProp.addForce( forceClass( boa, satName ) )

boa1 = BoaLoad()
divaProp.setupModels()

print("Propagating in Diva from ", t0, " to ", tf, "\n")

divaProp.writeOutput( t0, tf, boa1 )

plotOrbit( boa1, satName, "Transfer Orbit Insertion from GMAT", t0, tf )

# Use Monte to build the second burn
a1 = ApsisEvent( TrajQuery(boa,satName,"Earth","EME2000"), ApsisEvent.APOAPSIS )
mEpoch = Epoch(toiepoch + " UTC")
a = CountEvent( EpochEvent(mEpoch), a1, 1 )

# Define the VNB frame - for Monte, this is a BVN frame
BodyVelDirFrame( boa, "BVN Frame", "EME2000", TimeInterval(),
                 satName, "Earth" )

goiDV = Unit3Vec( ( 0*m/s, 1400*m/s, 0*m/s ) )
goiburn = ImpulseBurn( "GOI", a, "BVN Frame", goiDV, 300*sec )

# Insert Burn-1 into the manager
ibm.insert( goiburn )
divaProp.setupModels()

print("Propagating in Diva from ", t0, " to ", tf, "\n")

divaProp.writeOutput( t0, tf, boa1 )

plotOrbit( boa1, satName, "TOI from GMAT, GOI from Monte", t0, tf )
