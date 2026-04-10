# helperData.py
#
# 2018-12-07; nhatten; created as a stop-gap solution for "knowing things"
# 2018-12-13; nhatten; made interface slightly more generic
# 2018-12-17; nhatten; added comments
#
# @author nhatten

class helperData(object):
    """
    Class holds data so it can be accessed elsewhere"""

    def __init__(self):
        """Constructor"""

        # gravitational parameters in km^3/s^2
        self.mu = {
            'earth' : 398600.4415,
            'sun' : 1.32712440018e+11,
            'moon' : 4.9028000661637961E+03}

        # spice kernels
        self.kernels = {
            'deKernel' : ['C:/Utilities/Universe/ephemeris_files/de430.bsp'], # used for coordinate translation
            'naifTlsKernel' : ['C:/Utilities/Universe/ephemeris_files/naif0012.tls'], # used for coordinate translation and rotation
            'pckKernel' : ['C:/Utilities/Universe/ephemeris_files/pck00010.tpc']} # used for coordinate rotation
        return

        

    def getMu(self, body):
        """
        Get gravitational parameter for a body
        @param body Name of body contained in the mu dictionary
        @return mu Gravitational parameter of body
        """
        return self.mu[body.lower()]

    def getKernel(self, kernel):
        """
        Return a string to a spice kernel
        @param kernel The generic name of the kernel
        @return list The full paths and names of the kernel(s)
        """
        return self.kernels[kernel]