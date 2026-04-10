import math
import sys

#-------------------------------------------------------------------------------
#
#   Two-body keplerian orbit
#
#   @param s - state vector (x, y, z, vx, vy, vz) km, km/sec
#   @param t - A1ModJulian epoch of state
#   @param d - state parameter names (list of strings)
#   @param o - order of derivative. 
#   
#   @return the Derivatives vector
#   
#-------------------------------------------------------------------------------

def GetDerivatives(s, t, d, o):
    
    r = s[0:3] 
    v = s[3:6]
    
    mu = 3.986004415e5
    
    # Distance of Spacecraft from origin (central body)
    
    R  = math.sqrt(r[0]**2 + r[1]**2 + r[2]**2)
    
    # Acceleration magnitude from eq. a = mu/R^2
    
    a  = mu/R**2                                     
    
    #
    #   Point-mass gravity force model derivatives; velocity components can be
    #   set to zero.
    #
    
    dpm = [0, 0, 0, -a*r[0]/R, -a*r[1]/R, -a*r[2]/R]
    
    #
    #   Return the point-mass gravity derivatives
    #       

    return dpm # (km/s, km/s, km/s, km/s^2, km/s^2, km/s^2)

