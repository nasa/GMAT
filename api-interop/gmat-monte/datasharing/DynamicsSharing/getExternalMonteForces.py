#!/usr/bin/env mpython_iq

# ===========================================================================
#
# ===========================================================================

""" getExternalMonteForces.py:

    Description: script to retrive net forces acting over a spacecraft given
    a set of body ephemerides, the spacecraft state and Epoch (Frame and body
    center also  need to be defined, althouh, default values are asigned by
    Monte).

    by: 'Alex Campbell'
    contact: "alex.campbell@nasa.gov"

    lats edited: 08-26-2022

"""

# =============================================================================
# Imports:
# =============================================================================

import Monte as M
import mpy.io.data as defaultData
import mpy.traj.force.grav.basic as basicGrav
from mpy.units import *

from time import process_time

# =============================================================================
# Params Setup:
# =============================================================================
spacecraft = "mySC"
bodyCenter = "Earth"
inputFrame = "EME2000"

# =============================================================================
# Create Boa and load Monte default data:
# =============================================================================

# Define a project database (Boa).
boa = M.BoaLoad()

# Load phemeris and the default body and coordinate systems
# into the project database (boa).
boa = defaultData.load(
    [
        "time",
        "frame",
        "body",
        "ephem/planet/de430",
    ]
)

# =============================================================================
# Gravity Setup:
# =============================================================================

# create a gravity manager for spacecraft:
grav = M.Gravity(boa, spacecraft)
# Same as:  grav = GravityBoa.read( boa, spacecraft )

# #Build the gravity nodes connecting the spacecraft to the gravitational
basicGrav.add( boa, spacecraft, [ "Earth"])
#basicGrav.add(boa, spacecraft, ["Sun", "Earth", "Moon"])

# # Define which forces will act on the spacecraft during propagation.
forces = [
    M.GravityForce(boa, spacecraft),
]

gmatEpoch = "05-JAN-1941 12:00:00 TAI"
epochConverter = M.EpochDblCvt(gmatEpoch)

def setup(
    boa,
    spacecraft,
    center,
    inputFrame,
    forces,
    integFrame="EME2000",
    propName="DIVA"
):
    """Create propagator and set integration up."""

    tRef = M.Epoch("2000-01-01 12:00:00 ET")

    # --- IntegState
    # Set up state including stateInfo:
    # stateInfo = StateInfo(boa, tRef, spacecraft, center, inputFrame)
    scState = M.State(
         0.0 *km,
         0.0 *km,
         0.0 *km,
         0.0 *km/sec,
         0.0 *km/sec,
         0.0 *km/sec,
    )

    # Create the IntgState, then pass it to IntegSetup
    integState = M.IntegState(
        boa,
        tRef,
        tRef,
        [],
        spacecraft,
        center,
        inputFrame,
        integFrame,
        scState,
        forces,
        False,
        [],
        [],
    )

    # --- IntegSetup
    integSetup = M.IntegSetup(boa)

    # Add state to  propagation manager "IntegSetup"
    integSetup.add(integState)

    # --- Propagator
    # DIVA Propagator setup
    prop = M.DivaPropagator(boa, propName, integSetup)


# =============================================================================
# Main Function:
# =============================================================================
def getMForces(    
    t0,
    x0,    
    center="Earth",
    inputFrame="EME2000",    
    spacecraft="mySC",
    integFrame="EME2000",
    outputFrame=None,
    propName="DIVA",
    
):

    # Get propagator from boa
    if propName not in M.DivaPropagatorBoa.getAll(boa):
        setup(boa, spacecraft, center, inputFrame, forces, integFrame, propName)

    prop = M.DivaPropagatorBoa.read(boa, propName)

    # --- Setup new integration initial conditions
    # Set new time
    t0 = M.Epoch(t0)
    prop.setTime(t0)

    # Set new state
    prop.setState(
        spacecraft,
        center,
        inputFrame,
        M.State(*x0)
    )

    # Set integFrame
    if "EME2000" not in integFrame:
        prop.integSetup().findState(spacecraft).setIntegFrame(integFrame)

    # Clear propagator to allow changes to go into effect.
    # prop.setup() (called by prop.create()) will set everything up again
    prop.clear()

    # --- Propagate new traj
    # Create finite trajectory
    # dt = 0 is not allowed, add a delta time tDelta = 1.0*sec
    tDelta = prop.minStep()  
    prop.create(boa, t0, t0 + tDelta)

    # --- Query acceleration in outputFrame
    outputFrame = outputFrame or inputFrame
    state = M.TrajQuery(boa, spacecraft, center, outputFrame).state(t0,3)
    vel = state.vel()
    acc = state.acc()

    # --- Clean Boa
    # integSetup.eraseState(spacecraft)
    M.TrajLegBoa.eraseAll(boa, spacecraft)

    return [vel[0],vel[1],vel[2],acc[0],acc[1],acc[2]]


def GetForces(stateGmat ,tGmat, orderGmat):
    # =============================================================================
    # Define State and Epoch, retrieve Acccelerations:
    # =============================================================================
    exportEpoch = epochConverter.toEpoch(tGmat*86400)

    # Define a spacecraft state [km,km/s]:  (Use Geosynchronous Orbit as an example)    
    stateGmat = [ii * km for ii in stateGmat[0:3]] + [ii * km/s for ii in stateGmat[3:6]]

    ds = getMForces(exportEpoch, stateGmat)
    #print(ds)    

    return ds

def CustomEntryExample(s ,t, o):
    print("Used Custome Entry Example")
    return GetForces(s, t, o)
