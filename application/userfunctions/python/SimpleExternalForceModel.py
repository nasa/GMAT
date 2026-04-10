import math
import sys

GmatApiPath = r'..\..\api'

if not GmatApiPath in sys.path:
    sys.path.insert(0, GmatApiPath)
    
GmatBinPath = r'..\..\bin'

if not GmatBinPath in sys.path:
    sys.path.insert(0, GmatBinPath)

import gmatpy as gmat

#-------------------------------------------------------------------------------
#
#   Two-body keplerian orbit with SRP
#
#   @param s - state vector (x, y, z, vx, vy, vz) km, km/sec
#   @param t - A1ModJulian epoch of state
#   @param d - state parameter names (list of strings)
#   @param o - order of derivative. 
#   
#   @return the Derivatives vector
#   
#-------------------------------------------------------------------------------

def GetDerivativesSRP(s, t, d, o):
    
    r = s[0:3] 
    v = s[3:6]
    
    mu = 3.986004415e5
    
    # Distance of Spacecraft from origin (central body)
    
    R  = math.sqrt(r[0]**2 + r[1]**2 + r[2]**2)
    
    # Acceleration magnitude from eq. a = mu/R^2
    
    a  = mu/R**2                                     
    
    #
    #   Point-mass gravity force model derivatives
    #
    
    dpm = [v[0], v[1], v[2], -a*r[0]/R, -a*r[1]/R, -a*r[2]/R]
    
    #
    #   SRP acceleration
    #

    sun = gmat.GetObject('Sun')    
    sunpos = sun.GetMJ2000Position(t)

    r_vs = [r[0] - sunpos[0], r[1] - sunpos[1], r[2] - sunpos[2]]
    r_vs_mag = math.sqrt(r_vs[0]**2 + r_vs[1]**2 + r_vs[2]**2)
   
    sat = gmat.GetRuntimeObject('SatExternal')
    
    nu = 1.0                                # eclipse factor
    ps = 1367.0 / 299792.458                # solar flux at 1 AU divided by the speed of light
    au = 149597870.691                      # 1 AU in km
    cr = sat.GetNumber('Cr')                # coefficient of SRP
    A  = sat.GetNumber('SRPArea') / 1.0e6   # spacecraft area (sq. km)
    m  = sat.GetNumber('TotalMass')         # spacecraft mass (kg)
        
    #
    #   SRP derivatives
    #
    
    dsrp = [
        0.0, 0.0, 0.0,
        nu * ps * au**2 * (cr * A / m) * (r_vs[0] / r_vs_mag**3),
        nu * ps * au**2 * (cr * A / m) * (r_vs[1] / r_vs_mag**3),
        nu * ps * au**2 * (cr * A / m) * (r_vs[2] / r_vs_mag**3),
    ]
        
    #
    #   Return the sum of point-mass and SRP derivatives; velocity components
    #   can be set to zero.
    #
    
    ds = [0.0, 0.0, 0.0, dpm[3]+dsrp[3], dpm[4]+dsrp[4], dpm[5]+dsrp[5]]

    return ds # (km/s, km/s, km/s, km/s^2, km/s^2, km/s^2)

