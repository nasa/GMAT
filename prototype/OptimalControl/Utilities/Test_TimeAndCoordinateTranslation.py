"""
Tester for TimeUtil and CoordinateTranslation
Revision log:
2018-12-04; Noble Hatten; Created
"""

import numpy as np
import spiceypy
from TimeUtil import TimeUtil
from CoordinateTranslation import CoordinateTranslation

if __name__ == "__main__":
    # what spice kernels do we want?
    de430Kernel = 'C:/Utilities/Universe/ephemeris_files/de430.bsp'
    tlsKernel = 'C:/Utilities/Universe/ephemeris_files/naif0012.tls'

    # testing CoordinateTranslation
    ref = 'J2000' # J2000 = EME2000; EclipJ2000 is also valid
    ET = 1353546.0 # ephemeris time at which to perform translation
    stateWrtEarth = np.array([12000.0, -3000.0, 131.0, -1.3, 9.01, 2.4]) # state of spacecraft w.r.t. Earth, km, km/s
    body1 = 'Earth' # old origin
    body2 = 'Sun' # new origin

    ct = CoordinateTranslation(tlsKernel=tlsKernel, deKernel=de430Kernel)
    stateWrtSun = ct.translateOrigin(oldOriginBody=body1, 
                                     stateWrtOldOrigin=stateWrtEarth, 
                                     newOriginBody=body2, 
                                     ET=ET, 
                                     referenceFrame=ref)

    print(stateWrtEarth)
    print(stateWrtSun)
    print('***')

    # testing TimeUtil
    tu = TimeUtil(tlsKernel)
    MJDA1 = 31491.50295105763
    dateForward = tu.MJDA1ToStrET(MJDA1)
    dateForward = dateForward + ' TDB'
    MJDA1Backward = tu.Str2MJDA1(dateForward)
    dateBackward = tu.MJDA1ToStrET(MJDA1Backward)
    
    print(MJDA1, MJDA1Backward, (MJDA1-MJDA1Backward)*86400.0)
    print(dateForward, dateBackward)

    print('Done!')