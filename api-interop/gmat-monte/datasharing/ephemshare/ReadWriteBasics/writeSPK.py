import Monte as M
import mpy.io.data as defaultData
import mpy.traj.force.grav.basic as basicGrav

# Load planetary ephemeris de405 from default data
boa = defaultData.load( "ephem/planet/de405" )

# Build the gravity nodes connecting the spacecraft to the gravitational
# bodies we want active.
scName = "mySc"
basicGrav.add( boa, scName, [ "Sun", "Earth", "Moon" ] )

# Define an initial state of a spacecraft using Conic elements
radius = 6778.0 * km
state = M.State(
   boa, scName, 'Earth',
   M.Conic.semiMajorAxis( radius ),
   M.Conic.eccentricity( 0.0 ),
   M.Conic.inclination( 5 * deg ),
   M.Conic.argumentOfLatitude( 0 * deg),
   M.Conic.longitudeOfNode( 0 * deg),
   M.Conic.trueAnomaly( 0 * deg)
   )

# Define which forces will act on the spacecraft during propagation.
forces = [
   M.GravityForce( boa, scName ),
   ]

# Set up the beginning and end times for our scenario.
beginTime = M.Epoch( "01-JAN-2000 00:00:00 ET" )
endTime = M.Epoch( "10-JAN-2000 00:00:00 ET" )

# Add the initial state to the "IntegState"
integState = M.IntegState(
   boa,         # Model database used in integration
   beginTime,   # Start time
   endTime,     # End time
   [],          # Events to trigger integration end (none)
   scName,      # Spacecraft name
   'Earth',     # Center body
   'EME2000',   # Input frame
   'EME2000',   # Integration frame
   state,       # State initial conditions
   forces,      # Forces which act on state
   False,       # Integrate only partial derivatives (false)
   [],          # Parameters to be used in partial derivative calculations (none)
   []           # Partials tolerance scale factors (allows different partial
                # derivatives to have different integration tolerances, none)
   )

# Add state to our propagation manager "IntegSetup"
integ = M.IntegSetup( boa )
integ.add( integState )

# Set up the propagator.
prop = M.DivaPropagator( boa, "DIVA", integ )

prop.create( boa, beginTime, endTime )

#with M.BoaFile( "traj.boa", BoaFile.TRUNCATE ) as output:
   #prop.writeOutput( beginTime, endTime, output )


# Write SPK
SpiceName.bodyInsert( -42, "mySc" )
import cristo
cristo.convert( boa, "traj.bsp" )

#cristo.convert( "traj.boa", "traj.bsp" )

# Convert for a custom time interval.
#cristo.convert( boa, "traj.bsp", body="mySc", begin="04-JAN-2000 00:00:00 ET",
               #end="08-JAN-2000 00:00:00 ET" )

