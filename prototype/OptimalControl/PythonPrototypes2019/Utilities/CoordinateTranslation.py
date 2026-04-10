"""
Routines for translating states based on the states of celestial bodies.
Revision log:
2018-12-03; Noble Hatten; Created
2019-03-01; Noble Hatten; Added smoothed capability
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import spiceypy

class CoordinateTranslation(object):
    """
    Class holds capabilities for translating a state with origin at one celestial body to another.
    TODO: Use additional Spice kernels
    """

    def __init__(self, tlsKernel = [], deKernel = []):
        """
        Constructor
        @param tlsKernel List of full paths and names of Spice leap second kernel(s)
        @param deKernel List of Full paths and names of Spice DE ephemeris kernel(s)
        """

        # set and load Spice kernels
        # TODO: error checking on the input string
        self.LoadTlsKernel(tlsKernel)
        self.LoadDeKernel(deKernel)

        return

    def __del__(self):
        """
        Destructor
        """

        # unload spice kernels
        for kernel in self._tlsKernel:
            spiceypy.unload(kernel)
        for kernel in self._deKernel:
            spiceypy.unload(kernel)
        return

    def LoadTlsKernel(self, tlsKernel):
        """
        Set and load Spice leap second kernel
        @param tlsKernel List of full paths and names of Spice leap second kernel(s)
        # TODO: error checking/handling
        """

        # set kernel to private variable
        self._tlsKernel = tlsKernel

        # furnish the kernel(s)
        for kernel in self._tlsKernel:
            spiceypy.furnsh(kernel)
        return

    def LoadDeKernel(self, deKernel):
        """
        Set and load Spice DE kernel
        @param deKernel List of full paths and names of Spice DE ephemeris kernel(s)
        # TODO: error checking/handling
        """

        # set kernel to private variable
        self._deKernel = deKernel

        # furnish the kernel
        for kernel in self._deKernel:
            spiceypy.furnsh(self._deKernel)
        return

    def translateOrigin(self, oldOriginBody, stateWrtOldOrigin, newOriginBody, ET, referenceFrame):
        """
        Given position and velocity state w.r.t. a body recognizable by Spice,
        calculate position and velocity state w.r.t. another body recognizable by Spice
        @param oldOriginBody Body with respect to which the state of the spacecraft is known; must be a string or body ID recognizable by Spice
        @param stateWrtOldOrigin 6-element list containing position (km) and velocity (km/s) vectors w.r.t. oldOriginBody in referenceFrame frame at time ET
        @param newOriginBody Body with respect to which the state of the spacecraft is desired; must be a string or body ID recognizable by Spice
        @param ET The time at which to perform the translation in Spice ephemeris time
        @param referenceFrame The reference frame of (1) stateWrtOldOrigin and (2) in which the state of newOriginBody w.r.t. oldOriginBody is calculated
        @return stateWrtNewOrigin 6-element list containing position (km) and velocity (km/s) vectors w.r.t. newOriginBody in referenceFrame frame at time ET
        """

        abcorr = 'None'

        # state of old origin wrt new origin
        oldOriginState, LT = spiceypy.spkezr(oldOriginBody, ET, referenceFrame, abcorr, newOriginBody)
        oldOriginWrtNewOrigin = np.array(oldOriginState) # convert to np array
        stateWrtNewOrigin = oldOriginWrtNewOrigin + stateWrtOldOrigin
        return stateWrtNewOrigin

    def translateOriginSmoothed(self, oldOriginBody, stateWrtOldOrigin, newOriginBody, ET, referenceFrame, ephemSmoother):
        """
        Given position and velocity state w.r.t. a body recognizable by Spice,
        calculate position and velocity state w.r.t. another body recognizable by Spice.
        This version uses an ephemSmoother object rather than direct spice calls.
        @param oldOriginBody Body with respect to which the state of the spacecraft is known; must be a string or body ID recognizable by Spice
        @param stateWrtOldOrigin 6-element list containing position (km) and velocity (km/s) vectors w.r.t. oldOriginBody in referenceFrame frame at time ET
        @param newOriginBody Body with respect to which the state of the spacecraft is desired; must be a string or body ID recognizable by Spice
        @param ET The time at which to perform the translation in Spice ephemeris time
        @param referenceFrame The reference frame of (1) stateWrtOldOrigin and (2) in which the state of newOriginBody w.r.t. oldOriginBody is calculated
        @param ephemSmoother An ephemSmoother object
        @return stateWrtNewOrigin 6-element list containing position (km) and velocity (km/s) vectors w.r.t. newOriginBody in referenceFrame frame at time ET
        """

        # state of old origin wrt new origin
        oldOriginState, dOldOriginState, ddOldOriginState = ephemSmoother.GetState(referenceBody = newOriginBody, targetBody = oldOriginBody, referenceFrame = referenceFrame, ET = ET)
        oldOriginWrtNewOrigin = np.array(oldOriginState)
        stateWrtNewOrigin = oldOriginWrtNewOrigin + stateWrtOldOrigin
        return stateWrtNewOrigin

    def translateOriginSmoothedWithDerivative(self, oldOriginBody, stateWrtOldOrigin, newOriginBody, ET, referenceFrame, ephemSmoother):
        """
        Given position and velocity state w.r.t. a body recognizable by Spice,
        calculate position and velocity state w.r.t. another body recognizable by Spice.
        This version uses an ephemSmoother object rather than direct spice calls.
        Also returns first derivative of the transformation w.r.t. the spacecraft state and time.
        Note that the derivative is just the derivative of the function f^{O_F/O_D} (z') in the math spec, meaning it does not differentiate z' itself.
        @param oldOriginBody Body with respect to which the state of the spacecraft is known; must be a string or body ID recognizable by Spice
        @param stateWrtOldOrigin 6-element list containing position (km) and velocity (km/s) vectors w.r.t. oldOriginBody in referenceFrame frame at time ET
        @param newOriginBody Body with respect to which the state of the spacecraft is desired; must be a string or body ID recognizable by Spice
        @param ET The time at which to perform the translation in Spice ephemeris time
        @param referenceFrame The reference frame of (1) stateWrtOldOrigin and (2) in which the state of newOriginBody w.r.t. oldOriginBody is calculated
        @param ephemSmoother An ephemSmoother object
        @return stateWrtNewOrigin 6-element array containing position (km) and velocity (km/s) vectors w.r.t. newOriginBody in referenceFrame frame at time ET
        @return dOldOriginWrtNewOriginDt 6-element array containing time derivatives of oldOriginWrtNewOrigin
        @return dOldOriginWrtNewOriginDState 6x6 array containing state derivatives of oldOriginWrtNewOrigin. All elements are zero because the states of the origins only depend on time
        """

        # state of old origin wrt new origin
        oldOriginState, dOldOriginState, ddOldOriginState = ephemSmoother.GetState(referenceBody = newOriginBody, targetBody = oldOriginBody, referenceFrame = referenceFrame, ET = ET)
        oldOriginWrtNewOrigin = np.array(oldOriginState) # convert to array
        dOldOriginWrtNewOriginDt = np.array(dOldOriginState) # convert to array
        stateWrtNewOrigin = oldOriginWrtNewOrigin + stateWrtOldOrigin
        dOldOriginWrtNewOriginDState = np.zeros((6,6))
        return stateWrtNewOrigin, dOldOriginWrtNewOriginDState, dOldOriginWrtNewOriginDt