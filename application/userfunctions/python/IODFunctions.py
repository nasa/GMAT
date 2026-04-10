"""
Created on Tue Dec 14 2021
Algorithms and equations sourced from Chapter 7 of "Fundamentals of Astrodynamics and Applications, 4th Edition" 
@author: jelarbee
"""
import numpy as np
from datetime import timedelta



#-------------------------------------------------------------------------------------------------------
# v2 = IODFunctions.CalculateIODGibbs(r1, r2, r3,mu = 3.986004415e5,verbose = False):
#-------------------------------------------------------------------------------------------------------
#*
#* Perform initial orbit determination using the Gibbs approach
#*
#* @param r1 - Position vector 1 (x,y,z) km
#* @param r2 - Position vector 2 (x,y,z) km
#* @param r3 - Position vector 3 (x,y,z) km
#* @param mu - Gravitational Parameter for the central body, default earth (km^3/s^2)
#* @param verbose - turn on for additional reporting (to be added as needed)
#*
#* @return the v2 vector
#* OR is Verbose
#* @return the v2 vector and a log string
#*
#-------------------------------------------------------------------------------------------------------

def CalculateIODGibbs(r1, r2, r3,mu = 3.986004415e5,verbose = False):
    r1 = np.array(r1)
    r2 = np.array(r2)
    r3 = np.array(r3)
    Z_12 = np.cross(r1,r2)
    Z_23 = np.cross(r2,r3)
    Z_31 = np.cross(r3,r1)
    r1mag = float(np.linalg.norm(r1))
    r2mag = float(np.linalg.norm(r2))
    r3mag = float(np.linalg.norm(r3))
    N = r1mag*Z_23 + r2mag*Z_31 + r3mag*Z_12
    D = Z_12 + Z_23 + Z_31
    S = (r2mag - r3mag)*r1 +(r3mag - r1mag)*r2 + (r1mag - r2mag)*r3
    B = np.cross(D,r2)
    L_g = np.sqrt(mu/np.dot(N,D))
    v2 = (L_g/r2mag)*B + L_g*S
    v2 = np.ndarray.tolist(v2) #Added so output can be parsed by GMAT
    message =  "Using the Gibbs Algorithm\n"
    message = message + "Solution found!\n"
    message = message + "v2 = [{:.5f}, {:.5f}, {:.5f}]".format(v2[0],v2[1],v2[2])
    if verbose:
        return v2,message
    else:
        return v2

#-------------------------------------------------------------------------------------------------------
# v2 = IODFunctions.CalculateIODHerrickGibbs(r1, r2, r3,t1,t2,t3,mu = 3.986004415e5,verbose = False):
#-------------------------------------------------------------------------------------------------------
#*
#* Perform initial orbit determination using the Herrick-Gibbs approach
#*
#* @param r1 - Position vector 1 (x,y,z) km
#* @param r2 - Position vector 2 (x,y,z) km
#* @param r3 - Position vector 3 (x,y,z) km
#* @param t1 - Time for position vector 1 (Mjd, or seconds/day)
#* @param t2 - Time for position vector 2 (Mjd, or seconds/day)
#* @param t3 - Time for position vector 3 (Mjd, or seconds/day)
#* @param mu - Gravitational Parameter for the central body, default earth (km^3/s^2)
#* @param verbose - turn on for additional reporting (to be added as needed)
#*
#* @return the v2 vector
#* OR is Verbose
#* @return the v2 vector and a log string
#*
#-------------------------------------------------------------------------------------------------------
		
def CalculateIODHerrickGibbs(r1, r2, r3,t1,t2,t3,mu = 3.986004415e5,verbose=False):
    if t3 > t2 and t2 > t1:
        r1 = np.array(r1)
        r2 = np.array(r2)
        r3 = np.array(r3)
        delT_31 = (t3-t1)*86400
        delT_32 = (t3-t2)*86400
        delT_21 = (t2-t1)*86400
        r1mag = float(np.linalg.norm(r1))
        r2mag = float(np.linalg.norm(r2))
        r3mag = float(np.linalg.norm(r3))
        v21 = -delT_32*((1/(delT_21*delT_31))+(mu/(12*r1mag*r1mag*r1mag)))*r1
        v22 = (delT_32-delT_21)*((1/(delT_21*delT_32))+(mu/(12*r2mag*r2mag*r2mag)))*r2
        v23 = delT_21*((1/(delT_32*delT_31))+(mu/(12*r3mag*r3mag*r3mag)))*r3
        v2 = v21 + v22 + v23
        v2 = np.ndarray.tolist(v2) #Added so output can be parsed by GMAT
        message =  "Using the Herrick-Gibbs Algorithm\n"
        message = message + "Solution found!\n"
        message = message + "v2 = [{:.5f}, {:.5f}, {:.5f}]".format(v2[0],v2[1],v2[2])
        
        if verbose:
            return v2,message
        else:
            return v2
           
#-------------------------------------------------------------------------------------------------------
# [v2, log] = IODFunctions.ThreePositionIOD(r1,r2,r3,t1=None,t2=None,t3=None,mu = 3.986004415e5,IODtype=None)
#-------------------------------------------------------------------------------------------------------
#*
#* Top level function to use IOD from 3 position vectors. If no override provided, will determine  
#* which algorithm to use based on the separation angle between the observation vectors,
#* choosing Gibbs if either angle is greater than 6 degrees, and Herrick-Gibbs if both are less than 6 degrees.
#* Will fail if the observations are not coplanar within 3 degrees.
#*
#* @param r1 - Position vector 1 (x,y,z) km
#* @param r2 - Position vector 2 (x,y,z) km
#* @param r3 - Position vector 3 (x,y,z) km
#* @param t1 - Time for position vector 1 (Mjd, or seconds/day) 
#* @param t2 - Time for position vector 2 (Mjd, or seconds/day) 
#* @param t3 - Time for position vector 3 (Mjd, or seconds/day) 
#* @param mu - Gravitational Parameter for the central body, default earth (km^3/s^2)
#* @param IODtype - Override for which IOD algorithm to use
#*
#* @return  [v2,log] - Velocity at r2/t2, and a string containing logging information
#*
#-------------------------------------------------------------------------------------------------------
def ThreePositionIOD(r1, r2, r3,t1,t2,t3,mu = 3.986004415e5,IODtype=None):
    v2 = np.array([0.0,0.0,0.0])
    r1 = np.array(r1)
    r2 = np.array(r2)
    r3 = np.array(r3)
    message = "Performing ThreePositionIOD\n"
    message = message + "Listing Inputs:\n"
    message = message + "r1 = [{:.5f}, {:.5f}, {:.5f}] km\n".format(r1[0],r1[1],r1[2])
    message = message + "r2 = [{:.5f}, {:.5f}, {:.5f}] km\n".format(r2[0],r2[1],r2[2])
    message = message + "r3 = [{:.5f}, {:.5f}, {:.5f}] km\n".format(r3[0],r3[1],r3[2])
    message = message + "t1 = {:.8f} (Mjd, or seconds/day)\n".format(t1)
    message = message + "t2 = {:.8f} (Mjd, or seconds/day)\n".format(t2)
    message = message + "t3 = {:.8f} (Mjd, or seconds/day)\n".format(t3)
    message = message + "mu = {:.9e} km^3/s^2 \n".format(mu)
    if IODtype != None:
        message = message + "Using Type override for type " + IODtype +"\n"
    if t3 > t2 and t2 > t1:
        Z_23 = np.cross(r2,r3)
        alpha_12 = np.degrees(np.arccos(np.dot(r1,r2)/(float(np.linalg.norm(r1))*float(np.linalg.norm(r2)))))
        alpha_23 = np.degrees(np.arccos(np.dot(r2,r3)/(float(np.linalg.norm(r2))*float(np.linalg.norm(r3)))))
        gibbs = (IODtype == None and (alpha_12 > 6.0 or alpha_23 > 6.0))
        herrickGibbs = (IODtype == None and alpha_12 <= 6.0 and alpha_23 <= 6.0)
        if IODtype == "Gibbs" or gibbs:
            message = message + "Angle between position 1 and 2 is {:.3f} degrees\n".format(alpha_12)
            message = message + "Angle between position 2 and 3 is {:.3f} degrees\n".format(alpha_23)
            alpha_cop = np.degrees(np.arcsin((np.dot(Z_23,r1))/(np.linalg.norm(Z_23)*float(np.linalg.norm(r1)))))
            if abs(alpha_cop) > 3.0:
                v2 = np.ndarray.tolist(v2)
                message = message + "Error: The points are not coplanar, but have an offset of {:.3g} degrees. The three points must be coplanar within 3 degrees to perform IOD\n".format(alpha_cop)
                message = message + "No Solution found. Returning zeroes for v2.\n"
            else:
                message = message + "Positions are coplanar with error of {:.3g} degrees\n".format(alpha_cop)
                v2, message2 = CalculateIODGibbs(r1, r2, r3,mu,True)
                message = message + message2
        elif IODtype == "HerrickGibbs" or herrickGibbs:
            message = message + "Angle between position 1 and 2 is {:.3f} degrees\n".format(alpha_12)
            message = message + "Angle between position 2 and 3 is {:.3f} degrees\n".format(alpha_23)
            alpha_cop = 90-np.degrees(np.arccos((np.dot(Z_23,r1))/(np.linalg.norm(Z_23)*float(np.linalg.norm(r1)))))
            if abs(alpha_cop) > 3.0:
                v2 = np.ndarray.tolist(v2)
                message = message + "Error: The points are not coplanar, but have an offset of {:.3g} degrees. The three points must be coplanar within 3 degrees to perform IOD\n".format(alpha_cop)
                message = message + "No Solution found. Returning zeroes for v2.\n"
            else:
                message = message + "Positions are coplanar with error of {:.3g} degrees\n".format(alpha_cop)
                v2, message2 = CalculateIODHerrickGibbs(r1, r2, r3,t1,t2,t3,mu,True)
                message = message + message2
    else:
        message = message  + "Error: The times {:.8f}, {:.8f}, and {:.8f} are not sequential, which is a requirement for Gibbs and Herrick-Gibbs\n".format(t1,t2,t3)
        message = message + "No Solution found. Returning zeroes for v2.\n"
        v2 = np.ndarray.tolist(v2)
    
    return  v2, message
    
    
    
#-------------------------------------------------------------------------------------------------------
# v2 = IODFunctions.ThreePositionIODLean(r1,r2,r3,t1=None,t2=None,t3=None,mu = 3.986004415e5,IODtype=None)
#-------------------------------------------------------------------------------------------------------
#*
#* Top level function to use IOD from 3 position vectors. If no override provided, will determine  
#* which algorithm to use based on the separation angle between the observation vectors,
#* choosing Gibbs if either angle is greater than 6 degrees, and Herrick-Gibbs if both are less than 6 degrees.
#* Will fail if the observations are not coplanar within 3 degrees.
#* Lean version does not perform logging and will return only the v2 vector.
#*
#* @param r1 - Position vector 1 (x,y,z) km
#* @param r2 - Position vector 2 (x,y,z) km
#* @param r3 - Position vector 3 (x,y,z) km
#* @param t1 - Time for position vector 1 (Mjd, or seconds/day) 
#* @param t2 - Time for position vector 2 (Mjd, or seconds/day) 
#* @param t3 - Time for position vector 3 (Mjd, or seconds/day) 
#* @param mu - Gravitational Parameter for the central body, default earth (km^3/s^2)
#* @param IODtype - Override for which IOD algorithm to use
#*
#* @return the v2 vector from the determined algorithm
#*
#-------------------------------------------------------------------------------------------------------
def ThreePositionIODLean(r1, r2, r3,t1,t2,t3,mu = 3.986004415e5,IODtype=None):
    v2 = [0.0,0.0,0.0]
    if t3 > t2 and t2 > t1:
        Z_23 = np.cross(r2,r3)
        alpha_12 = np.degrees(np.arccos(np.dot(r1,r2)/(float(np.linalg.norm(r1))*float(np.linalg.norm(r2)))))
        alpha_23 = np.degrees(np.arccos(np.dot(r2,r3)/(float(np.linalg.norm(r2))*float(np.linalg.norm(r3)))))
        gibbs = (IODtype == None and (alpha_12 > 6.0 or alpha_23 > 6.0))
        herrickGibbs = (IODtype == None and alpha_12 <= 6.0 and alpha_23 <= 6.0)
        if IODtype == "Gibbs" or gibbs:
            alpha_cop = np.degrees(np.arcsin((np.dot(Z_23,r1))/(np.linalg.norm(Z_23)*float(np.linalg.norm(r1)))))
            if abs(alpha_cop) > 3.0:
                print("The points are not coplanar, but have an offset of " + str(alpha_cop) + " degrees. The three points must be coplanar within 3 degrees to perform IOD.")
            else:
                v2 = CalculateIODGibbs(r1, r2, r3,mu)
        elif IODtype == "HerrickGibbs" or herrickGibbs:
            
                alpha_cop = 90-np.degrees(np.arccos((np.dot(Z_23,r1))/(np.linalg.norm(Z_23)*float(np.linalg.norm(r1)))))
                if abs(alpha_cop) > 3.0:
                    print("The points are not coplanar, but have an offset of " + str(alpha_cop) + " degrees. The three points must be coplanar within 3 degrees to perform IOD.")
                else:
                    v2 = CalculateIODHerrickGibbs(r1, r2, r3,t1,t2,t3,mu)
    else:
        print("Error: The times {:.8f}, {:.8f}, and {:.8f} are not sequential, which is a requirement for Gibbs and Herrick-Gibbs\n".format(t1,t2,t3))
        v2 = np.ndarray.tolist(v2)
    return v2
 