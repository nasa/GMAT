# Test_EphemSmoother.py

"""
2019-02-14; nhatten; initial creation

@author: nhatten
"""

from EphemSmoother import EphemSmoother
from CubicSpline import CubicSpline

if __name__ == '__main__':

    # paths for spice kernels
    de430Kernel = ['C:/Utilities/Universe/ephemeris_files/de430.bsp']
    tlsKernel = ['C:/Utilities/Universe/ephemeris_files/naif0012.tls']

    # create ephemSmoother object
    ephemSmoother = EphemSmoother(tlsKernel = tlsKernel, deKernel = de430Kernel, 
                                  defaultNRegions = 359, defaultH = 86400.0, defaultStepSizeType = 'nRegions', defaultNRegionsPerRevolution = 359, defaultET0 = 0.0, defaultETf = 31557600.0)

    # create cubicSpline object
    cubicSpline = CubicSpline()

    # create a spline
    ET0 = 0.0
    ETf = 86400.0 * 365.25
    nRegions = 359
    h = 86400.0
    ephemSmoother.CreateSmoothedEphem(referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000', 
                                      ET0 = ET0, ETf = ETf, h = h, nRegions = nRegions, nRegionsPerRevolution = 1, stepSizeType = 'nRegions')

    # now, calculate a spline value at some time
    #ET = 22232.2
    #ySingle, dySingle, ddySingle = cubicSpline.EvaluateClampedCubicSplineVectorized(ephemSmoother.ephemDataList[0].a,
    #                                                                                ephemSmoother.ephemDataList[0].b,
    #                                                                                ephemSmoother.ephemDataList[0].c,
    #                                                                                ephemSmoother.ephemDataList[0].d,
    #                                                                                ephemSmoother.ephemDataList[0].ETs,
    #                                                                                ET)

    #ET = 2232.2
    #ySingle2, dySingle2, ddySingle2 = cubicSpline.EvaluateClampedCubicSplineVectorized(ephemSmoother.ephemDataList[0].a,
    #                                                                                ephemSmoother.ephemDataList[0].b,
    #                                                                                ephemSmoother.ephemDataList[0].c,
    #                                                                                ephemSmoother.ephemDataList[0].d,
    #                                                                                ephemSmoother.ephemDataList[0].ETs,
    #                                                                                ET)

    # now, use GetState to get the state at that time
    state, dstate, ddstate = ephemSmoother.GetState(referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000', ET = 222232.2)

    # now, request the state for something that doesn't exist and check to see if it is created
    state2, dstate2, ddstate2 = ephemSmoother.GetState(referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000', ET = 86400.0 * 365.25 * 5.0)

    # clean up
    ephemSmoother.__del__()


    print('Finished')