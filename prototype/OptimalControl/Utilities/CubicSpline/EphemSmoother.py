# EphemSmoother.py

"""
2019-02-13; nhatten; initial creation

@author: nhatten
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import spiceypy
from CoordinateTranslation import CoordinateTranslation
from EphemData import EphemData
from CubicSpline import CubicSpline
import StateConversions
from helperData import helperData

class EphemSmoother(object):
    """
    Class for creating and retrieving smooth ephemerides
    """

    def __init__(self, tlsKernel = [], deKernel = [], defaultNRegionsPerRevolution = 359, defaultNRegions = 359, defaultH = 86400.0, defaultStepSizeType = 'h', defaultET0 = 0.0, defaultETf = 31557600.0):
        """
        constructor
        @param tlsKernel List of full paths and names of Spice leap second kernel(s)
        @param deKernel List of Full paths and names of Spice DE ephemeris kernel(s)
        @param defaultNRegionsPerRevolution The default number of time regions to use per revolution of the target body about its central body for computing a spline
        @param defaultNRegions The default number of time regions to use for computing a spline
        @param defaultH The default step size (in ET seconds) to use when computing a spline
        @param defaultStepSizeType The default method for calculating the time between knots: 'h' or 'nRegions' or 'nRegionsPerRevolution'
        @param defaultET0 The default opening time of the spline domain (in ET seconds)
        @param defaultETf The default closing time of the spline domain (in ET seconds)
        """

        # uses a single CoordinateTranslation object
        self.coordinateTranslation = CoordinateTranslation(tlsKernel = tlsKernel, deKernel = deKernel)

        # uses a single CubicSpline object
        self.cubicSpline = CubicSpline()

        # use a single helperData object
        self.helperData = helperData()

        # create an array of EphemData objects, to be populated
        self.ephemDataList = []

        # default values
        self.defaultNRegions = defaultNRegions
        self.defaultNRegionsPerRevolution = defaultNRegionsPerRevolution
        self.defaultH = defaultH
        self.defaultStepSizeType = defaultStepSizeType
        self.defaultET0 = defaultET0
        self.defaultETf = defaultETf

        # need a dictionary of possible target bodies and their corresponding central bodies, based on integer ids recognizable by spice
        # the key is the target body, the value is the central body
        self.centralBodiesOfTargetBodies = {}
        self.CreateDictionaryOfCentralBodiesOfTargetBodies() # create initial population
        return

    def __del__(self):
        """
        destructor
        """

        # call the coordinateTranslation destructor to unload the spice kernels
        self.coordinateTranslation.__del__()

        return

    def CreateDictionaryOfCentralBodiesOfTargetBodies(self):
        """
        populate self.centralBodiesOfTargetBodies
        """

        # planets and planet barycenters: central body is the sun
        # anything from x01 to x98 orbits planet x (x99)
        for i in range(1,10):
            self.centralBodiesOfTargetBodies[i] = 10
            self.centralBodiesOfTargetBodies[99 + i * 100] = 10
            for j in range(1,99):
                self.centralBodiesOfTargetBodies[j + i * 100] = i * 100 + 99

        return

    def AddToDictionaryOfCentralBodiesOfTargetBodies(self, centralBodyID, targetBodyID):
        """
        Add an individual targetBody/centralBody pair to the centralBodiesOfTargetBodies dictionary
        @param centralBodyID Spice ID of central body
        @param targetBodyID Spice ID of target body
        """
        self.centralBodiesOfTargetBodies[targetBodyID] = centralBodyID
        return

    def GetState(self, referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000', ET = 0.0):
        """
        return the interpolated state of targetBody w.r.t. referenceBody
        @param referenceBody The body with respect to which the state of targetBody is calculated
        @param targetBody The body whose state is splined
        @param referenceFrame The reference frame in which the spline of targetBody w.r.t. referenceBody is calculated
        @param ET The time at which to obtain the state in Spice ephemeris time
        @return y State at ET
        @return dy First time derivative of state at ET
        @return ddy Second time derivative of state at ET
        """

        # check to see if the request is valid
        # TODO: target body w.r.t. central body also needs to be valid if using NRegionsPerRevolution
        i = self.ValidateInterpolationRequest(referenceBody = referenceBody, targetBody = targetBody, referenceFrame = referenceFrame, ET = ET)

        # if the request is not valid, then create a new spline to make it valid
        if i == -1:
            if ET < self.defaultET0:
                ET0 = ET - 365.25 * 86400.0
                ETf = self.defaultETf
            elif ET > self.defaultETf:
                ET0 = self.defaultET0
                ETf = ET + 365.25 * 86400.0
            else:
                ET0 = self.defaultET0
                ETf = self.defaultETf
            self.CreateSmoothedEphem(referenceBody = referenceBody, targetBody = targetBody, referenceFrame = referenceFrame, 
                                     ET0 = ET0, ETf = ETf, h = self.defaultH, nRegions = self.defaultNRegions, stepSizeType = self.defaultStepSizeType)
            i = len(self.ephemDataList)-1


        # if the request is valid, then evaluate the interpolation and return
        state, dstate, ddstate = self.cubicSpline.EvaluateClampedCubicSplineVectorized(self.ephemDataList[i].a,
                                                                                       self.ephemDataList[i].b,
                                                                                       self.ephemDataList[i].c,
                                                                                       self.ephemDataList[i].d,
                                                                                       self.ephemDataList[i].ETs,
                                                                                       ET)
        return state, dstate, ddstate

    def ValidateInterpolationRequest(self, referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000', ET = 0.0):
        """
        check to see if an interpolation request is valid
        @param referenceBody The body with respect to which the state of targetBody is calculated
        @param targetBody The body whose state is splined
        @param referenceFrame The reference frame in which the spline of targetBody w.r.t. referenceBody is calculated
        @param ET The time at which to obtain the state in Spice ephemeris time
        @return validIdx Boolean: If interpolation data already exists, validIdx is its index. Otherwise, returns -1
        """

        # assume false, then try to be proven wrong
        validIdx = -1
        # first, see if there are even any elements in self.ephemDataList
        if self.ephemDataList:
            # then there is at least one element in existence

            # now, check to see if: reference body, target body, reference frame, and time are ok

            i = 0
            for ephem in self.ephemDataList:
                if ephem.referenceBody == referenceBody:
                    if ephem.targetBody == targetBody:
                        if ephem.referenceFrame == referenceFrame:
                            if ET >= ephem.ET0 and ET <= ephem.ETf:
                                validIdx = i
                                break
                i += 1


        return validIdx

    def GetAvailableSmoothedSphems(self):
        """
        @return self.ephemDataList The currently available ephemeris data objects
        """
        return self.ephemDataList

    def RemoveEphem(self, idx):
        """
        Remove an ephemeris index from the list of smoothed ephemerities
        @param idx Index of ephemeris to remove
        """
        if idx >= 0 and idx < len(self.ephemDataList):
            del self.ephemDataList[idx]
        return

    def SetHForNRegionsPerRevolution(self, referenceBody, targetBody, referenceFrame, ET, nRegionsPerRevolution):
        """
        ***NOT WORKING!***
        Set a constant step size to produce a specified number of spline regions (knot-1) per revolution of the splined body about its central body
        @param referenceBody The body with respect to which the state of targetBody is calculated
        @param targetBody The body whose state is splined
        @param referenceFrame The reference frame in which the spline of targetBody w.r.t. referenceBody is calculated
        @param ET The ephemeris time at which orbital periods are calculated
        @param nRegionsPerRevolution Number of desired spline regions (knot-1) per revolution of the splined body about its central body
        @return h The step size in ET seconds
        """

        # determine the central body of the targetBody; requires getting the spice ID of the targetBody
        if isinstance(targetBody, int):
            targetBodyID = targetBody
            targetBodySTring = spiceypy.bodc2s(targetBodyID)
        else:
            # convert from string to spice id
            targetBodyID = spiceypy.bodn2c(targetBody)
            targetBodyString = targetBody

        # get the spice ID of the referenceBody
        # also, get mu of the central body from helperData for calculating orbital period
        if isinstance(referenceBody, int):
            referenceBodyID = referenceBody
            referenceBodyString = spiceypy.bodc2s(referenceBodyID)
        else:
            # convert from string to spice id
            referenceBodyID = spiceypy.bodn2c(referenceBody)
            referenceBodyString = referenceBody
            

        stateWrtOldOrigin = np.zeros(6)
        # check if targetBodyID is in the dictionary
        if targetBodyID in self.centralBodiesOfTargetBodies:
            # in this case, there is already a central body set for this target body
            # don't need to do anything else
            # get the spice ID of the centralBody
            centralBodyID = self.centralBodiesOfTargetBodies[targetBodyID]

            centralBodyString = spiceypy.bodc2s(centralBodyID)
            muCentralBody = self.helperData.getMu(centralBodyString)

            # determine the period of the targetBody about the central body
            # use the state of the targetBody at ET0 to calculate the orbital period
            stateTargetWrtCentral = self.coordinateTranslation.translateOrigin(targetBody, stateWrtOldOrigin, centralBodyString, ET, referenceFrame)
            stateTargetWrtCentral = StateConv(stateTargetWrtCentral, 'Cartesian', 'ClassicalKeplerian', muCentralBody) # from StateConversions
            smaTargetWrtCentral = stateTargetWrtCentral[0]
            periodTargetWrtCentral = 2.0 * np.pi * np.sqrt(smaTargetWrtCentral**3 / muCentralBody)
            h = periodTargetWrtCentral / np.float(nRegionsPerRevolution)
        # check to see if the reference body is in the dictionary
        elif referenceBodyID in self.centralBodiesOfTargetBodies:
            # in this case, 
            pass
        else:
            # in this case, there is not a central body set for this target body,
            # so we need to figure out and set the central body for this target body
            # based on the CreateDictionaryOfCentralBodiesOfTargetBodies() call in the constructor,
            # we know the targetBody is not a planet or a moon
            # it could be the Sun or something else.
            # we could be requesting orbit of sun w.r.t. a planet or moon, or the orbit of an asteroid/comet w.r.t. something
            if targetBodyID == 10:
                # if the target is the sun
                # in this case, we don't necessarily want to add to the dictionary
                # because there is not one "central body" for the sun
                # instead, just set it locally
                # we'll use the orbit of the reference body (rather than the orbit of the target body) to get the step size
                # (the reference body is orbiting the target body)
                centralBodyID = self.centralBodiesOfTargetBodies[referenceBodyID]

                centralBodyString = spiceypy.bodc2s(centralBodyID)
                muCentralBody = self.helperData.getMu(centralBodyString)

                # determine the period of the targetBody about the central body
                # use the state of the targetBody at ET0 to calculate the orbital period
                stateReferenceWrtCentral = self.coordinateTranslation.translateOrigin(referenceBody, stateWrtOldOrigin, centralBodyString, ET, referenceFrame)
                stateReferenceWrtCentral = StateConv(stateReferenceWrtCentral, 'Cartesian', 'ClassicalKeplerian', muCentralBody) # from StateConversions
                smaReferenceWrtCentral = stateReferenceWrtCentral[0]
                periodReferenceWrtCentral = 2.0 * np.pi * np.sqrt(smaReferenceWrtCentral**3 / muCentralBody)
                h = periodReferenceWrtCentral / np.float(nRegionsPerRevolution)
            if referenceBodyID == 10:
                # if the reference body is the sun
                # assume that the target (which is not a planet or a moon) orbits the sun
                # add it to the dictionary
                self.AddToDictionaryOfCentralBodiesOfTargetBodies(referenceBodyID, targetBodyID)
                centralBodyID = self.centralBodiesOfTargetBodies[targetBodyID]
        
                centralBodyString = spiceypy.bodc2s(centralBodyID)
                muCentralBody = self.helperData.getMu(centralBodyString)

                # determine the period of the targetBody about the central body
                # use the state of the targetBody at ET0 to calculate the orbital period
                stateTargetWrtCentral = self.coordinateTranslation.translateOrigin(targetBody, stateWrtOldOrigin, centralBodyString, ET, referenceFrame)
                stateTargetWrtCentral = StateConv(stateTargetWrtCentral, 'Cartesian', 'ClassicalKeplerian', muCentralBody) # from StateConversions
                smaTargetWrtCentral = stateTargetWrtCentral[0]
                periodTargetWrtCentral = 2.0 * np.pi * np.sqrt(smaTargetWrtCentral**3 / muCentralBody)
                h = periodTargetWrtCentral / np.float(nRegionsPerRevolution)

        

        return h

    def CreateSmoothedEphem(self, referenceBody = 'Sun', targetBody = 'Earth', referenceFrame = 'J2000', 
                            ET0 = 0.0, ETf = 31557600.0, h = 1.0, nRegions = 1, nRegionsPerRevolution = 1, stepSizeType = 'h'):
        """
        Create spline coefficients
        @param referenceBody The body with respect to which the state of targetBody is calculated
        @param targetBody The body whose state is splined
        @param referenceFrame The reference frame in which the spline of targetBody w.r.t. referenceBody is calculated
        @param nStates The number of states splined
        @param ET0 The earliest spice ephemeris time at which spline data is available
        @param ETf The latest spice ephemeris time at which spline data is available
        @param h Step size between spline knots (ET seconds); assumed constant for now
        The final time can be greater than the requested ETf if (ETf-ET0)/h is not an integer
        @param nRegions Number of spline regions, assumed to be equally spaced for now; number of knots is nRegions+1
        @param defaultNRegionsPerRevolution The default number of time regions to use per revolution of the target body about its central body for computing a spline
        @param stepSizeType How should we decide on the step size? currently valid strings are 'h' and 'nRegions' and 'nRegionsPerRevolution'
        """

        # assuming 6 states
        nStates = 6

        # calculate the spline coefficients
        if stepSizeType == 'nRegions':
            h = (ETf - ET0) / np.float(nRegions)
            nKnots = nRegions + 1
        elif stepSizeType == 'nRegionsPerRevolution':
            # NOT CURRENTLY WORKING, DO NOT USE
            #h = self.SetHForNRegionsPerRevolution(referenceBody, targetBody, referenceFrame, ET0, nRegionsPerRevolution)
            #nRegions = (ETf - ET0) / h
            #nKnots = nRegions + 1
            pass
            
        elif stepSizeType == 'h':
            # even though we are stepping with h, we still need to know how many regions there are so that we can appropriately dimension the coefficient arrays
            nRegions = np.int(np.ceil(np.abs((ETf - ET0) / h)))
            nKnots = nRegions + 1

        # create a new EphemData object and append it to our list of EphemData objects
        self.ephemDataList.append(EphemData(referenceBody = referenceBody, targetBody = targetBody, referenceFrame = referenceFrame, 
                                            nStates = nStates, ET0 = ET0, ETf = ETf, h = h, nRegions = nRegions, stepSizeType = stepSizeType))
            

        # loop through number of points at which to get data
        stateWrtOldOrigin = np.zeros(6)
        #ETs = np.empty(nKnots)
        states = np.empty((nKnots,nStates))
        for i in range(nKnots):
            self.ephemDataList[-1].ETs[i] = ET0 + h * np.float(i) # save the current time
            states[i,:] = self.coordinateTranslation.translateOrigin(targetBody, stateWrtOldOrigin, referenceBody, self.ephemDataList[-1].ETs[i], referenceFrame)

        # approximate the derivatives at the end points so we can do a clamped cubic spline
        # using a 4th order finite difference scheme at each end, which requires 5 points, in addition to the end point
        # take all 4 points to be equally spaced between the end point and the first knot
        dydx0 = np.empty(nStates)
        dydxf = np.empty(nStates)
        hFD = h / 1.0 # internal step size
        ETsFDStart = np.empty(5)
        ETsFDEnd = np.empty(5)
        ETsFDStart[0] = self.ephemDataList[-1].ETs[0] # already know we want to start with the starting time
        ETsFDEnd[4] = self.ephemDataList[-1].ETs[nRegions] # already know we want to end with the ending time
        statesFDStart = np.empty((5, nStates))
        statesFDEnd = np.empty((5, nStates))
        statesFDStart[0,:] = states[0,:] # already know we want to start with the starting state
        statesFDEnd[4,:] = states[nRegions,:] # already know we want to end with the ending state

        # calculate the intermediate states
        for i in range(1,5):
            ETsFDStart[i] = ET0 + hFD * np.float(i)
            ETsFDEnd[4-i] = ETf - hFD * np.float(i)
            statesFDStart[i,:] = self.coordinateTranslation.translateOrigin(targetBody, stateWrtOldOrigin, referenceBody, ETsFDStart[i], referenceFrame)
            statesFDEnd[4-i,:] = self.coordinateTranslation.translateOrigin(targetBody, stateWrtOldOrigin, referenceBody, ETsFDEnd[4-i], referenceFrame)

        # do the finite differencing
        for i in range(nStates):
            dydx0[i] = self.cubicSpline.FiniteDifferenceAtEdge(ETsFDStart, statesFDStart[:,i], 'left')
            dydxf[i] = self.cubicSpline.FiniteDifferenceAtEdge(ETsFDEnd, statesFDEnd[:,i], 'right')

        # once we have all the data, loop through the states to spline
        for i in range(nStates):
            self.ephemDataList[-1].a[:,i], self.ephemDataList[-1].b[:,i], self.ephemDataList[-1].c[:,i], self.ephemDataList[-1].d[:,i] =  self.cubicSpline.CalculateClampedCubicSplineCoefficients(self.ephemDataList[-1].ETs, states[:,i], dydx0[i], dydxf[i])
        return


