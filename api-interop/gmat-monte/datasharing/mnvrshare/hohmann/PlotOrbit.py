import vista        # Useful for debugging
import mpylab

from Monte import *
from math import *
from mpy.units import *


#=======================================================================
# Plot the ballistic orbit
#=======================================================================

# We're going to use this code a number of times, so we'll place it into
# a function

def plotOrbit( boa, body, title, t0, tf, fbm=None ):

    q = TrajQuery(boa,body,"Earth","EME2000")
    qM = MassBoa.read(boa,body)
    
    plotT = Epoch.range(t0,tf,60*sec)
    
    x = [ Cartesian.x( q.state(t) ).convert('km') for t in plotT ]
    y = [ Cartesian.y( q.state(t) ).convert('km') for t in plotT ]
    z = [ Cartesian.z( q.state(t) ).convert('km') for t in plotT ]
    R = [ Spherical.radius( q.state(t) ).convert('km') for t in plotT ]
    mass = [ qM.mass(t).convert('kg') for t in plotT ]
    
    # Add the radius of Earth to the XY plot
    theta = range(0,360)

    d2r = 3.14159263 / 180.0

    xEarth = [ 6378.1363*cos(thetaVal*d2r) for thetaVal in theta ]
    yEarth = [ 6378.1363*sin(thetaVal*d2r) for thetaVal in theta ]

    # Color any Finite Burns
    
    mpylab.figure()
    hb, = mpylab.plot( plotT, R, 'b', linewidth=3 )

    a = mpylab.axis()
    mpylab.plot( (a[0],a[1]), [6378.1363]*2, 'c', linewidth=2 )
    mpylab.xlabel('Time (UTC)', fontsize=14)
    mpylab.ylabel('Radius (km)', fontsize=14)
    mpylab.title('Orbital Radius: %s'%title, fontsize=18)
    mpylab.xticks(fontsize=12)
    mpylab.yticks(fontsize=12)
    mpylab.grid(True)
    if fbm is not None:
       for i in range(fbm.size()):
          burn_t0 = fbm[i].startTime()
          burn_tf = fbm[i].stopTime()
          tVals = Epoch.range(burn_t0,burn_tf,(burn_tf-burn_t0)/200.)
          tVals.append(burn_tf)
          R = [ Spherical.radius( q.state(t) ).convert('km') for t in tVals ]
          hb, = mpylab.plot( tVals, R, 'r', linewidth=4 )
    
    mpylab.figure()
    hb, = mpylab.plot( plotT, mass, 'b', linewidth=3 )
    mpylab.xlabel('Time (UTC)', fontsize=14)
    mpylab.ylabel('Mass (kg)', fontsize=14)
    mpylab.title('Mass: %s'%title, fontsize=18)
    mpylab.xticks(fontsize=12)
    mpylab.yticks(fontsize=12)
    mpylab.grid(True)
    if fbm is not None:
       for i in range(fbm.size()):
          burn_t0 = fbm[i].startTime()
          burn_tf = fbm[i].stopTime()
          tVals = Epoch.range(burn_t0,burn_tf,(burn_tf-burn_t0)/200.)
          tVals.append(burn_tf)
          mass = [ qM.mass(t).convert('kg') for t in tVals ]
          hb, = mpylab.plot( tVals, mass, 'r', linewidth=4 )
    
    mpylab.figure()
    hb, = mpylab.plot( x, y, 'b', linewidth=3 )

    mpylab.plot( xEarth, yEarth, 'c', linewidth=1 )
    mpylab.xlabel('X (km)', fontsize=14)
    mpylab.ylabel('Y (km)', fontsize=14)
    mpylab.title('Orbit: %s'%title, fontsize=18)
    mpylab.xticks(fontsize=12)
    mpylab.yticks(fontsize=12)
    mpylab.grid(True)
    mpylab.axis('equal')
    if fbm is not None:
       for i in range(fbm.size()):
          burn_t0 = fbm[i].startTime()
          burn_tf = fbm[i].stopTime()
          tVals = Epoch.range(burn_t0,burn_tf,(burn_tf-burn_t0)/200.)
          tVals.append(burn_tf)
          x = [ Cartesian.x( q.state(t) ).convert('km') for t in tVals ]
          y = [ Cartesian.y( q.state(t) ).convert('km') for t in tVals ]
          hb, = mpylab.plot( x, y, 'r', linewidth=4 )
    
    mpylab.show(block=False)


