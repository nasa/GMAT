#!/usr/bin/env mpython

#=======================================================================
# Filename:     HohmannMonte.py
# Description:  A Hohmann Transfer performed in Monte
#
# Date         Author               Notes
# -----------  -------------------  -----------------------------------
# 09/22/2020   Darrel J. Conway     Initial Version
#
#=======================================================================


#=======================================================================
# Load modules and set preferences
#=======================================================================

import vista        # Useful for debugging

# Load a basic set of libraries to use 
import Monte as M
import mpy.io.data as defaultData
import mpy.traj.force.grav.basic as basicGrav
from mpy.units import *

from PlotOrbit import *

#=======================================================================
# Load trajectories
#=======================================================================

#boa.load( 'de421.boa' )   # Load the planets
boa = defaultData.load( "ephem/planet/de405" )


body = "spacecraft"

#=======================================================================
# Define an orbit
#=======================================================================

t0 = Epoch('1/1/2024 12:00:00 ET')
state0 = State( boa, body, "Earth",
                Conic.periapsisRange(7000*km),
                Conic.apoapsisRange(7100*km),
                Conic.inclination(3.2*deg),
                Conic.longitudeOfNode(0*deg),
                Conic.argumentOfPeriapsis(25*deg),
                Conic.trueAnomaly(90*deg),
                )

mass0 = 1000*kg

#=======================================================================
# Build the DivaTraj integrator
#=======================================================================

grav = Gravity( boa, body )
grav.insert( GravityNode( "", "", "Sun", 0*km ) )


#  Create an Earth system node as a child of the solar system barycenter
grav.insert( GravityNode( "", "", "Earth", 2.5e6*km ) )
#  Add the Moon as a child node of the Earth system node
grav.insert( GravityNode( "Earth", "", "Moon", 0*km ) )


# We'll keep this simple - no spherical harmonics, SRP, Drag, etc.

integState = IntegrationState()
integState.addState( t0, body, "Earth", "EME2000", state0 )
integState.addMass( t0, body, mass0 )
integState.setStateTol( 1e-10 )

#  Create the propagator
prop = DivaPropagator( boa, "DIVA", integState )

#  Configure the step size tolerances. 
prop.setMinStep( 1*sec )
prop.setMaxStep( 8*86400*sec )

forces = [
  GravityForce, ImpulseBurnForce, FiniteBurnForce,
  ]
for forceClass in forces:
   prop.addForce( forceClass( boa, body ) )

#  Creating the model links
prop.setupModels()

#=======================================================================
# Integrate the ballistic orbit
#=======================================================================

tf = t0 + 1*day

boa1 = BoaLoad()
prop.writeOutput( t0, tf, boa1 )

# plotOrbit( boa1, body, "Ballistic Case", t0, tf )

# Impulse Burn Managers
ibm = ImpulseBurnMgr( boa, body )

e1 = ApsisEvent( TrajQuery(boa,body,"Earth","EME2000"), ApsisEvent.PERIAPSIS )
e = CountEvent( EpochEvent(t0+0.3*day), e1, 1 )

# Define the VNB frame - for Monte, this is a BVN frame
BodyVelDirFrame( boa, "BVN Frame", "EME2000", TimeInterval(),
                 body, "Earth" )

e1 = ApsisEvent( TrajQuery(boa,body,"Earth","EME2000"), ApsisEvent.PERIAPSIS )
e = CountEvent( EpochEvent(t0+0.3*day), e1, 1 )

toiDV = Unit3Vec( ( 0*m/s, 2000*m/s, 0*m/s ) )
toiburn = ImpulseBurn( "TOI", e, "BVN Frame", toiDV, 300*sec )

a1 = ApsisEvent( TrajQuery(boa,body,"Earth","EME2000"), ApsisEvent.APOAPSIS )
a = CountEvent( EpochEvent(t0+0.3*day), a1, 1 )

goiDV = Unit3Vec( ( 0*m/s, 1400*m/s, 0*m/s ) )
goiburn = ImpulseBurn( "GOI", a, "BVN Frame", goiDV, 300*sec )

# Insert Burn-1 into the manager
ibm.clear()
ibm.insert( toiburn )
ibm.insert( goiburn )

# Integrate
prop.setupModels()
prop.writeOutput( t0, tf, boa1 )

#=======================================================================
# Plot the orbit with Burn-1
#=======================================================================

plotOrbit( boa1, body, "Basic Hohmann Transfer", t0, tf )

