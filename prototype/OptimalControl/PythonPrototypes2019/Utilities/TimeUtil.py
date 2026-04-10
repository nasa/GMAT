"""
Routines for translating times between coordinate systems.
Revision log:
2018-12-03; Noble Hatten; Created
2019-03-15; Noble Hatten; added TT<->TDB methods that don't depend on spice(ypy)
"""

import spiceypy
import autograd.numpy as np

class TimeUtil(object):
    """
    Class holds capabilities for converting between time systems.
    Primarily, this is needed to convert between GMAT's base MJD A.1
    and Spice's base ET (TDB seconds past J2000).
    """

    def __init__(self, tlsKernel=''):
        """
        Constructor
        @param tslKernel Full path and name of a Spice leap second kernel
        """
        
        # set and load Spice leap second kernel
        # TODO: error checking on the input string
        self.LoadTlsKernel(tlsKernel)

        # Set constants (private variables)
        self._secPerDay = 86400.0 # seconds per day

        self._TAI2A1 = 0.0343817 # offset between A.1 and TAI (sec): A.1 sec = TAI sec + offset
        self._A12TAI = -0.0343817 # offset between A.1 and TAI (sec): TAI sec = A.1 sec + offset
        self._TAI2A1FracOfDay = self._TAI2A1/self._secPerDay # offset between A.1 and TAI in fraction of a day: A.1 days = TAI days + offset
        self._A12TAIFracOfDay = self._A12TAI/self._secPerDay # offset between A.1 and TAI in fraction of a day: TAI days = A.1 days + offset

        self._MJDCommon2JD = 2400000.5 # offset between julian and modified julian dates (days): MJD = JD - offset, JD = MJD + offset (common definition, not gmat definition)
        self._MJDGMAT2MJDCommon = 29999.5 # offset between MJD (common) and MJD (GMAT): MJD_common = MJD_GMAT + offset
        self._JD2MJDCommon = -2400000.5 # offset between julian and modified julian dates (days): MJD = JD + offset, JD = MJD - offset (common definition, not gmat definition)
        self._MJDCommon2MJDGMAT = -29999.5 # offset between MJD (common) and MJD (GMAT): MJD_GMAT = MJD_Common + offset

        self._TAI2TDT = 32.184 # offset between TDT and TAI (sec): TDT = TAI + offset
        self._TAI2TDTFracOfDay = self._TAI2TDT/self._secPerDay # offset between TDT and TAI in fraction of a day: TDT = TAI + offset
        self._TDT2TAI = -32.184 # offset between TDT and TAI (sec): TAI = TDT + offset
        self._TDT2TAIFracOfDay = self._TDT2TAI/self._secPerDay # offset between TDT and TAI in fraction of a day: TAI = TDT + offset

        self._JDOfJ2000TDT = 2451545.0 # JD of J2000 epoch in TDT
        return

    def __del__(self):
        """
        Destructor
        """

        # unload spice kernel
        spiceypy.unload(self._tlsKernel)
        return

    def LoadTlsKernel(self, tlsKernel):
        """
        Set and load Spice leap second kernel
        @param tslKernel Full path and name of a Spice leap second kernel
        # TODO: error checking/handling
        """

        # set kernel to private variable
        self._tlsKernel = tlsKernel

        # furnish the kernel
        spiceypy.furnsh(self._tlsKernel)
        return


    #############################
    # MJD TAI <-> JED
    def JED2MJDTAI(self, JED):
        """
        Convert time from JED to MJDTAI
        @param JED Julian ephemeris date
        @return MJDTAI Modified Julian date (TAI)
        """
        JDTDT = spiceypy.unitim(JED, "JED", "JDTDT") # jed -> jdtdt
        MJDTDT = self.JD2MJD(JDTDT) # jdtdt -> mjtdt
        MJDTAI = MJDTDT + self._TDT2TAIFracOfDay # mjdtdt -> mjdtai
        return MJDTAI

    def MJDTAI2JED(self, MJDTAI):
        """
        Convert MJD in TAI to JED (TDB)
        @param MJDTAI Modified Julian date (TAI)
        @ return JED Julian ephemeris date
        """
        MJDTDT = MJDTAI + self._TAI2TDTFracOfDay # tai -> tdt
        JDTDT = self.MJD2JD(MJDTDT) # mjd -> jd
        JED = spiceypy.unitim(JDTDT, "JDTDT", "JED") # tdt -> jed
        return JED
    #############################

    #############################
    # JD <-> MJD
    def JD2MJD(self, JD):
        """
        Convert time from JD to MJD
        The input and output times are in the same system (TDB, A.1, etc.)
        Note: MJD uses GMAT's definition of MJD
        @param JD Julian date
        @return MJD mofidied Julian date
        """
        MJD = JD + self._JD2MJDCommon + self._MJDCommon2MJDGMAT
        return MJD

    def MJD2JD(self, MJD):
        """
        Convert time from MJD to JD
        The input and output times are in the same system (TDB, A.1, etc.)
        Note: MJD uses GMAT's definition of MJD
        @param MJD mofidied Julian date
        @return JD Julian date
        """
        JD = MJD + self._MJDCommon2JD + self._MJDGMAT2MJDCommon
        return JD
    #############################

    #############################
    # TAI <-> A.1
    def TAI2A1(self, TAI):
        """
        Convert TAI time in (M)JD to A.1 time in (M)JD
        @param TAI (Modified) Julian date in TAI
        @return A1 (Modified) Julian date in A.1
        """
        A1 = TAI + self._TAI2A1FracOfDay
        return A1

    def A12TAI(self, A1):
        """
        Convert A.1 (M)JD time to TAI (M)JD time
        @param A1 (Modified) Julian date in A.1
        @return TAI (Modified) Julian date in TAI
        """
        TAI = A1 + self._A12TAIFracOfDay
        return TAI
    #############################

    #############################
    # MJD A.1 <-> JED
    def MJDA12JED(self, MJDA1):
        """
        Convert time from MJD A.1 to JD ET (TDB)
        Note: MJD uses GMAT's definition of MJD
        @param MJDA1 Modified Julian date in A.1
        @return JED Julian ephemeris date
        """
        MJDTAI = self.A12TAI(MJDA1) # mjda1 -> mjdtai
        JED = self.MJDTAI2JED(MJDTAI) # jdtai -> jdtdt -> jed
        return JED

    def JED2MJDA1(self, JED):
        """
        Convert time from JD ET (TDB) to MJD A.1
        Note: MJD uses GMAT's definition of MJD
        @param JED Julian ephemeris date
        @return MJDA1 Modified Julian date in A.1
        """
        MJDTAI = self.JED2MJDTAI(JED) # jed -> jdtdt -> mjtdt -> jdtai
        MJDA1 = self.TAI2A1(MJDTAI) # mjdtai -> mjda1
        return MJDA1
    #############################

    #############################
    # ET <-> MJD A.1
    def MJDA12ET(self, MJDA1, useSpice = 1):
        """
        Convert time from GMAT MJDA1 to Spice ephemeris time (TDB)
        Note: MJDA1 uses GMAT's definition of MJD
        @param MJDA1 Modified Julian date in A.1
        @return Spice ephemeris time
        """
        if useSpice == 1:
            JED = self.MJDA12JED(MJDA1) # mjda1 -> jed
            ET = spiceypy.unitim(JED, "JED", "ET") # jed -> et
        elif useSpice == 0:
            ET = self.MJDA12ET_NoSpice(MJDA1)
        return ET

    def ET2MJDA1(self, ET):
        """
        convert spice ephemeris time to time in MJDA1
        Note: MJD uses GMAT's definition of MJD
        @param ET Spice ephemeris time
        @return Modified Julian date in A.1
        """    
        JED = spiceypy.unitim(ET, "ET", "JED") # et -> jed
    
        # julian date to A1 MJD
        MJDA1 = self.JED2MJDA1(JED) # jed -> mjda1
        return MJDA1

    def MJDA12ET_NoSpice(self, MJDA1):
        """
        Convert time from GMAT MJDA1 to Spice ephemeris time (TDB) without actually using spice
        Note: MJDA1 uses GMAT's definition of MJD
        @param MJDA1 Modified Julian date in A.1
        @return Spice ephemeris time
        """

        # step 1: get MJD in TAI
        MJDTAI = self.A12TAI(MJDA1)

        # step 2: get MJD in TDT
        MJDTDT = MJDTAI + self._TAI2TDTFracOfDay

        # step 3: get JD in TDT
        JDTDT = self.MJD2JD(MJDTDT)

        # step 4: get seconds past J2000 in TDT
        daysPastJ2000TDT = JDTDT - self._JDOfJ2000TDT
        secondsPastJ2000TDT = daysPastJ2000TDT * self._secPerDay

        # step 5: get seconds past J2000 in TDB
        T_TT = daysPastJ2000TDT / 36525.0
        M_E = 357.5277233 + 35999.05034 * T_TT
        ET = secondsPastJ2000TDT + 0.001658 * np.sin(np.deg2rad(M_E)) + 0.00001385 * np.sin(np.deg2rad(2.0 * M_E))

        return ET

    #############################
    # string <-> MJD A.1
    def Str2MJDA1(self, date):
        """
        convert date string (recognizable by spice) to time in MJDA1
        Note: MJD uses GMAT's definition of MJD
        @param date A date string recognizable by Spice
        @return Modified Julian date in A.1
        """

        ET = spiceypy.str2et(date) # string -> et
    
        JED = spiceypy.unitim(ET, "ET", "JED") # et -> jed
    
        # julian date to A1 MJD
        MJDA1 = self.JED2MJDA1(JED) # jed -> mjda1
        return MJDA1

    def MJDA1ToStrET(self, MJDA1):
        """
        Convert MJDA1 date to Gregorian string date in ET (TDB)
        Note: MJD uses GMAT's definition of MJD
        @param MJDA1 Modified Julian date in A.1
        @return A date string in the 'pictur' format
        """
        ET = self.MJDA12ET(MJDA1) # mjda1 -> et
        pictur = 'DD Mon YYYY HR:MN:SC.### ::TDB' # format specifier
        date = spiceypy.timout(ET, pictur) # et -> string
        return date
    #############################
