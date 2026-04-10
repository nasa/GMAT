# EphemData.py

"""
2019-02-14; nhatten; initial creation

@author: nhatten
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian


class EphemData(object):
    """
    Class to hold smoothed ephemeris data
    """

    def __init__(self, referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000',
                 nStates = 6, ET0 = 0.0, ETf = 31557600.0, h = 1.0, nRegions = 1, stepSizeType = 'nRegions'):
        """
        constructor
        @param referenceBody The body with respect to which the state of targetBody is calculated
        @param targetBody The body whose state is splined
        @param referenceFrame The reference frame in which the spline of targetBody w.r.t. referenceBody is calculated
        @param nStates The number of states splined
        @param ET0 The earliest spice ephemeris time at which spline data is available
        @param ETf The latest spice ephemeris time at which spline data is available
        @param h Step size between spline knots; assumed constant for now
        @param nRegions Number of spline regions, assumed to be equally spaced for now; number of knots is nRegions+1
        @param stepSizeType How should we decide on the step size? currently valid strings are 'h' and 'nRegions'
        """

        # create variables to hold state of referenceBody with respect to state of targetBody
        self.referenceBody = referenceBody
        self.targetBody = targetBody
        self.referenceFrame = referenceFrame
        self.nStates = nStates # the number of states splined
        self.ET0 = ET0 # the earliest spice ephemeris time at which spline data is available
        self.ETf = ETf # the latest spice ephemeris time at which spline data is available
        self.h = h # step size; assumed constant for now
        self.nRegions = nRegions # number of spline regions; the number of points used to create the spline is nRegions+1
        self.stepSizeType = stepSizeType

        # for t_i <= t < t_i+1: state_j(t) = a(i,j) + b(i,j) * (t - t_i) + c(i,j) * (t - t_i)^2 + d(i,j) * (t - t_i)^3
        self.a = np.empty((nRegions, nStates))
        self.b = np.empty((nRegions, nStates))
        self.c = np.empty((nRegions, nStates))
        self.d = np.empty((nRegions, nStates))

        self.ETs = np.empty(nRegions+1) # the spice ephemeris times corresponding to the knot points
        
        

        return


