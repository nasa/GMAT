#!/usr/bin/env mpython

#=======================================================================
# Filename:     HohmannImpulseSharing_Setup.py
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

# GMAT-Monte Interface Functions
import GmatMonte as GM

######################## Monte Configuration ###########################

# Using the default BOA, set up the solar system
boa = defaultData.load( "ephem/planet/de405" )

# The spacecraft is named MySat, so use this name during the configuration
satName = "MySat"

# Set the start epoch to the date this script was first edited
startEpoch = "02 Oct 2020 15:50:00.000"
t0 = Epoch(startEpoch + ' UTC')
# Define a LEO orbit, near equatorial, with the state set at apogee
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


# Build the propagator using gravity from the Sun, Earth and Moon
grav = Gravity( boa, satName )

grav.insert( GravityNode( "", "", "Sun", 0*km ) )
grav.insert( GravityNode( "", "", "Earth", 2.5e6*km ) )
grav.insert( GravityNode( "Earth", "", "Moon", 0*km ) )

#  Create the propagator for the spacecraft
divaProp = DivaPropagator( boa, "DIVA", integState )

#  Configure the step size tolerances. 
divaProp.setMinStep( 1 * sec )
divaProp.setMaxStep( 3600 * sec )

# Setup the impulsive burn manager, but don't add burns yet
ibm = ImpulseBurnMgr( boa, satName )

# Add forces to the propagator
forces = [ GravityForce, ImpulseBurnForce, ]
for forceClass in forces:
   divaProp.addForce( forceClass( boa, satName ) )



######################## GMAT Configuration ###########################

# Spacecraft Configuration
mysat = gmat.Construct("Spacecraft","MySat")
GM.M2G_CopySpacecraft(state0, t0, mysat)

# Add in mass data: 800 kg fuel mass in a tank and 200 kg dry mass
mysat.SetField("DryMass", 200)

tank = gmat.Construct("ChemicalTank","FuelTank")
tank.SetField("FuelMass", 800)
mysat.SetField("Tanks","FuelTank")

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
