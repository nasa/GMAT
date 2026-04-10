#!/usr/bin/env mpython

#=======================================================================
# Filename:     ManeuverPlanSharing_Setup.py
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

# Load the GMAT Modules
from load_gmat import *
import GmatFunctions as GF

# **********************************************************************
#
# ***   Monte Configuration
#
# **********************************************************************

#=======================================================================
# Using the default BOA, set up the solar system
#=======================================================================
boa = defaultData.load( "ephem/planet/de405" )

# The spacecraft is named MySat, so use this name during the configuration
satName = "MySat"

#=======================================================================
# Start from a LEO orbit for MySat
#=======================================================================
# Set the start epoch to the date this script was first edited
t0 = Epoch('10/2/2020 15:50:00 UTC')
# Define a LEO orbit, near equatorial, with the state set at perigee
state0 = State( boa, satName, "Earth",
                Conic.semiMajorAxis(6800*km),
                Conic.eccentricity(0.0011),
                Conic.inclination(3.2*deg),
                Conic.longitudeOfNode(90*deg),
                Conic.argumentOfPeriapsis(90*deg),
                Conic.trueAnomaly(180*deg),
                )

# Use a total mass of 1000 kg
mass0 = 1000*kg

# Set up the state that propagates through time and that received the maneuver
integState = IntegSetup(boa)
integState.addState( t0, satName, "Earth", "EME2000", state0 )
integState.addMass( t0, satName, mass0 )
integState.setStateTol( 1e-10 )


#=======================================================================
# Build the DivaTraj integrator
#=======================================================================

grav = Gravity( boa, satName )
grav.insert( GravityNode( "", "", "Sun", 0*km ) )


#  Create an Earth system node as a child of the solar system barycenter
grav.insert( GravityNode( "", "", "Earth", 2.5e6*km ) )
#  Add the Moon as a child node of the Earth system node
grav.insert( GravityNode( "Earth", "", "Moon", 0*km ) )



#  Create the propagator
divaProp = DivaPropagator( boa, "DIVA", integState )

#  Configure the step size tolerances. 
divaProp.setMinStep( 1*sec )
divaProp.setMaxStep( 8*86400*sec )


# **********************************************************************
#
# ***   GMAT Configuration
#
# **********************************************************************

#=======================================================================
# Basic Spacecraft Configuration
#=======================================================================
# Set up a spacecraft for the GMAT side of the simulation
mysat = gmat.Construct("Spacecraft","MySat")
mysat.SetField("DateFormat","UTCGregorian")
mysat.SetField("DisplayStateType","Keplerian")

# Start from a LEO orbit for MySat
mysat.SetField("Epoch","02 Oct 2020 15:50:00.000")
mysat.SetField("SMA",6800)
mysat.SetField("ECC",0.0011)
mysat.SetField("INC",3.2)
mysat.SetField("RAAN",90)
mysat.SetField("AOP",90)
mysat.SetField("TA",180)

# Add in mass data: 800 kg dry mass and 200 kg fuel, in a tank
mysat.SetField("DryMass",800)

tank = gmat.Construct("ChemicalTank","Tank")
tank.SetField("FuelMass", 200)
mysat.SetField("Tanks","Tank")

#=======================================================================
# Build the GMAT force model and integrator
#=======================================================================
fm = gmat.Construct("ForceModel","FM")
epm  = gmat.Construct("PointMassForce")
epm.SetField("BodyName","Earth")
spm  = gmat.Construct("PointMassForce")
spm.SetField("BodyName","Sun")
mpm  = gmat.Construct("PointMassForce")
mpm.SetField("BodyName","Luna")

fm.AddForce(epm)
fm.AddForce(spm)
fm.AddForce(mpm)

pdProp = gmat.Construct("Propagator","Prop")
pdProp.SetField("FM","FM")
pdProp.SetField("Type","PrinceDormand78")

gmat.Initialize()
