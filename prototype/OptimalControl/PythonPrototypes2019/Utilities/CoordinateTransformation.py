"""
Routines for transforming states based on Spice coordinate frames.
Revision log:
2018-12-07; Noble Hatten; Created
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import spiceypy

class CoordinateTransformation(object):
    """
    Class holds capabilities for rotating a state from one frame to another.
    TODO: Use additional Spice kernels
    """

    def __init__(self, tlsKernel = '', pckKernel = ''):
        """
        Constructor
        @param tlsKernel List of full paths and names of Spice leap second kernel(s)
        @param deKernel List of Full paths and names of Spice DE ephemeris kernel(s)
        """

        # set and load Spice kernels
        # TODO: error checking on the input string
        self.LoadTlsKernel(tlsKernel)
        self.LoadPckKernel(pckKernel)

        return

    def __del__(self):
        """
        Destructor
        """

        # unload spice kernels
        for kernel in self._tlsKernel:
            spiceypy.unload(kernel)
        for kernel in self._pckKernel:
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

    def LoadPckKernel(self, pckKernel):
        """
        Set and load Spice pck kernel
        @param pckKernel Full path and name of a Spice DE ephemeris kernel
        # TODO: error checking/handling
        """

        # set kernel to private variable
        self._pckKernel = pckKernel

        # furnish the kernel(s)
        for kernel in self._pckKernel:
            spiceypy.furnsh(kernel)
        return

    def TransformState(self, oldFrame, stateWrtOldFrame, newFrame, ET):
        """
        Given position and velocity state w.r.t. a frame recognizable by Spice,
        calculate position and velocity state w.r.t. another frame recognizable by Spice
        @param oldFrame Frame with respect to which the state of the spacecraft is known; must be a string recognizable by Spice
        @param stateWrtOldFrame 6-element list containing position (km) and velocity (km/s) vectors w.r.t. oldFrame at time ET
        @param newFrame Frame with respect to which the state of the spacecraft is desired; must be a string recognizable by Spice
        @param ET The time at which to perform the rotation in Spice ephemeris time
        @return stateWrtNewFrame 6-element list containing position (km) and velocity (km/s) vectors w.r.t. newFrame at time ET
        """

        transformationMatrix = spiceypy.sxform(oldFrame, newFrame, ET)
        stateWrtNewFrame = np.dot(transformationMatrix, stateWrtOldFrame)

        return stateWrtNewFrame