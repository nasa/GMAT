# -*- coding: utf-8 -*-
"""
Created on Tue Nov 20 13:50:15 2018
2018-12-07; nhatten; revisions
2018-12-11; nhatten; added GetUscaledStateForFun() and GetUnscaledStateForFun_old()
2018-12-13; nhatten; added function upper and lower bound getter and setter. added custom setting getter and setter
2018-12-17; nhatten; added more comments and commented out deprecated attributes

@author: sphughe1
@author: nhatten
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from enum import Enum # requires python 3.4+
import AlgebraicFunction
from ScaleUtil import ScaleUtility
import StateConversions
from helperData import helperData
from StateConversions import StateConv, StateConvJacobian
from CubicSpline import CubicSpline
from CoordinateTranslation import CoordinateTranslation
from CoordinateTransformation import CoordinateTransformation
from TimeUtil import TimeUtil
from EphemSmoother import EphemSmoother
from DecVecDerivatives import DecVecTimeDerivatives
from stateTypes import stateTypes

#class stateTypes(Enum):
#    # enumerations for orbit state representations
#    Cartesian = 1
#    ClassicalKeplerian = 2
#    SphericalAzFPA = 4

class GMATFunction(AlgebraicFunction.AlgebraicFunction):

    def __init__(self):
        
        # Call parent class constructor
        super().__init__()
        
        # Initialize the dec vec data to empty
        self.decVecData = [] # holds the names of the data in the decision vector
        self.decVecUnits = [] # holds the units of the elements of decVecData
        self.phaseConfigs = [] # holds the phase phaseConfig object(s)
        self.phaseScalers = [] # holds the phase scaleUtility object(s)
        self.funScaler = [] # holds the function scaleUtility object
        self.funConfig = [] # holds the function phaseConfig object
        self.funLBScaled = [] # holds function lower bounds
        self.funUBScaled = [] # holds function upper bounds
        self.customSettings = {} # holds custom settings

        # we only need one of these objects:
        self.helperData = helperData()
        self.cubicSpline = CubicSpline()

        tlsKernel = self.helperData.getKernel('naifTlsKernel')
        pckKernel = self.helperData.getKernel('pckKernel')
        deKernel = self.helperData.getKernel('deKernel')
        self.coordinateTransformation = CoordinateTransformation(tlsKernel=tlsKernel, pckKernel=pckKernel)
        self.coordinateTranslation = CoordinateTranslation(tlsKernel=tlsKernel, deKernel=deKernel)

        self.timeUtil = TimeUtil(tlsKernel) 

        self.ephemSmoother = [] # holds an ephemSmoother object

        self.ephemType = 'Spice' # how should ephemerides by calculated?

        # deprecated:
        #self.Scaler = ScaleUtility()
        return

    def SetEphemType(self, ephemType):
        """
        Set how ephemerides should be calculated
        @param ephemType The ephemeris calculation type; meaningful values: 'Spice', 'Spline'
        Need Spline if taking derivatives
        """
        self.ephemType = ephemType
        return

    def SetEphemSmoother(self, ephemSmoother):
        """
        Set the ephemSmoother object
        """
        self.ephemSmoother = ephemSmoother
        return

    
    def SetPhaseConfig(self,pointIdx,phaseConfig,insertionType="insert"):
        """
        Set the phase configuration object for pointIdx
        # @param insertionType "insert" or "overwrite". If "insert", then data is inserted after the pointIdx (e.g., if adding a new point). If "overwrite", then the data of pointIdx is overwritten (it must already exist)
        """
        if insertionType == "insert":
            self.phaseConfigs.insert(pointIdx,phaseConfig)
        elif insertionType == "overwrite":
            self.phaseConfigs[pointIdx] = phaseConfig
        return

    def SetPhaseScaler(self,pointIdx,scaler,insertionType="insert"):
        """
        Set the scaleUtility object for pointIdx
        # @param insertionType "insert" or "overwrite". If "insert", then data is inserted after the pointIdx (e.g., if adding a new point). If "overwrite", then the data of pointIdx is overwritten (it must already exist)

        """
        if insertionType == "insert":
            self.phaseScalers.insert(pointIdx,scaler)
        elif insertionType == "overwrite":
            self.phaseScalers[pointIdx] = scaler
        return

    def SetFunConfig(self,funConfig):
        """
        Set the function configuration object
        """
        self.funConfig = funConfig
        return

    def SetFunScaler(self,scaler):
        """
        Set the scaleUtility object for the function
        """
        self.funScaler = scaler
        return

    def SetFunBounds(self, funLBUnscaled, funUBUnscaled):
        """
        Set upper and lower bounds on the constraint function
        @param funLBUnscaled Unscaled numpy array of lower bounds, in the same order as the elements of the function itself
        @param funUBUnscaled Unscaled numpy array of upper bounds, in the same order as the elements of the function itself
        """

        # scale
        self.funLBScaled = self.funScaler.ScaleVector(funLBUnscaled, self.funConfig.Units)
        self.funUBScaled = self.funScaler.ScaleVector(funUBUnscaled, self.funConfig.Units)

        return

    def GetFunBoundsScaled(self):
        """
        Return scaled upper and lower bounds on the constraint function
        @return funLBScaled Scaled numpy array of lower bounds, in the same order as the elements of the function itself
        @return funUBScaled Scaled numpy array of upper bounds, in the same order as the elements of the function itself
        """

        funLBScaled = self.funLBScaled
        funUBScaled = self.funUBScaled
        return funLBScaled, funUBScaled

    def SetCustomSetting(self, key, value):
        """
        Add a custom setting to the dictionary of custom settings
        @param key The name of the custom setting
        @param value The value of the custom setting
        """

        self.customSettings[key] = value
        return

    def GetCustomSetting(self, key):
        """
        Extract a custom setting value from its key
        @param key The name of the custom setting
        TODO: Check to see if key exists
        """
        return self.customSettings[key]
        
    def GetUnscaledStateForFun(self, pointIdx, returnElements = ['All']):
        """
        Get unscaled state in the format desired by the function.
        Here, "format" means origin and axes and representation.
        Gives the user the part of the state they ask for and only that part of the state.
        
        @param pointIdx The function point index at which to obtain the unscaled state
        @param returnElements List of which elements of the state to return; e.g., returnElements = ['All'], ['OrbitState'], ['TotalMass'], ['OrbitState', 'TotalMass'].
            If 'All' is specified, the states MUST be in the order they are specified in PhaseConfig.DecVecData are returned in the order listed in PhaseConfig.DecVecData
            If the list contains multiple strings, then the state is returned in the order which the states are listed in returnElements
        @return unscaledState The unscaled state in the format desired by the function
        """

        # get the entire scaled state at this point
        stateFull = self.GetState(pointIdx)

        # now, just extract the states and scaling information we care about
        phaseConfig = self.phaseConfigs[pointIdx] # get the phase configuration
        phaseScaler = self.phaseScalers[pointIdx] # get the phase scaling
        unitsFull = phaseConfig.Units
        orbitConversion = False # initially assume we don't need to do the orbital conversion
        if returnElements[0] == 'All':
            state = stateFull # we want everything
            units = unitsFull

            # figure out if we need to do the orbit state conversion
            j = 0
            for decVecData in phaseConfig.DecVecData:
                jNext = j + decVecData[1]
                if decVecData[0] == 'OrbitState':
                    idxOrbStart = j
                    idxOrbEnd = jNext
                    orbitConversion = True
                j = jNext
        else:
            state = []
            units = []
            for returnElement in returnElements: # loop through strings in returnElements
                j = 0
                for decVecData in phaseConfig.DecVecData:
                    jNext = j + decVecData[1] # how big is this substate?
                    if returnElement == decVecData[0]:      
                        state.extend(stateFull[j:jNext]) # append the substate
                        units.extend(unitsFull[j:jNext]) # append the units for the substate

                        # figure out if we need to do the orbit state conversion
                        if returnElement == 'OrbitState':
                            idxOrbStart = len(state)-decVecData[1]
                            idxOrbEnd = len(state)
                            orbitConversion = True
                    j = jNext

            # error check
            if state == []:
                print('GMATFunction.GetUnscaledStateForFun: invalid returnElements input')
            else:
                state = np.array(state)

        # unscale the state at this point (only the parts we care about)
        unscaledState = phaseScaler.UnscaleVector(state, units) # unscale the state

        if orbitConversion:
            # convert the state orbit representation to Cartesian if necessary
            # it IS necessary if any of the following is true:
            # 1. the origins of the phase and the function are different AND the phase state rep is not cartesian
            # 2. the axes of the phase and the function are different AND the phase state rep is not cartesian
            # 3. the function state rep is Cartesian AND the phase state rep is not cartesian
            funConfig = self.funConfig # get the function configuration

            if (phaseConfig.Origin != funConfig.Origin or # origins are different
                phaseConfig.Axes != funConfig.Axes): # axes are different
                if 'Cartesian' in phaseConfig.OrbitStateRep: # state came in cartesian
                    pass # don't do anything
                else: # state didn't come in in cartesian
                    # import state conversion tool
                    #from StateConversions import StateConv

                    # convert phase state to cartesian
                    inputEnum = self.StringtoEnum(phaseConfig.OrbitStateRep)
                    cartEnum = self.StringtoEnum('Cartesian')

                    # import tool for knowing stuff about central bodies
                    #from helperData import helperData
                    #helper = helperData()
                    mu = self.helperData.getMu(phaseConfig.Origin) # gravitational parameter of central body of phase
                    unscaledState[idxOrbStart:idxOrbEnd] = StateConv(unscaledState[idxOrbStart:idxOrbEnd], inputEnum, cartEnum, mu) # convert to Cartesian

                # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
                inputEnum = self.StringtoEnum('Cartesian')

                # we now have unscaled cartesian state w.r.t. the origin and axes of the phase
            else:
                # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
                inputEnum = self.StringtoEnum(phaseConfig.OrbitStateRep)

            if (phaseConfig.Origin != funConfig.Origin): # origins are different
                # translate the phase's state to the function's origin (using spice)
                #from CoordinateTranslation import CoordinateTranslation
                #from helperData import helperData
                #helper = helperData()
                # the helper knows where the spice kernels are located
                #tlsKernel = self.helperData.getKernel('naifTlsKernel')
                #deKernel = self.helperData.getKernel('deKernel')
                #translator = CoordinateTranslation(tlsKernel=tlsKernel, deKernel=deKernel)

                # convert current time (assumed to be in GMAT MJD A.1) to Spice ephemeris time
                #from TimeUtil import TimeUtil
                #timeConverter = TimeUtil(tlsKernel)  
                t = self.GetTime(pointIdx)
                ET = self.timeUtil.MJDA12ET(t, useSpice = 0)

                # translate state
                if self.ephemType == 'Spice':
                    unscaledState[idxOrbStart:idxOrbEnd] = self.coordinateTranslation.translateOrigin(phaseConfig.Origin, unscaledState[idxOrbStart:idxOrbEnd], funConfig.Origin, ET, phaseConfig.Axes)
                elif self.ephemType == 'Spline':
                    unscaledState[idxOrbStart:idxOrbEnd] = self.coordinateTranslation.translateOriginSmoothed(phaseConfig.Origin, unscaledState[idxOrbStart:idxOrbEnd], funConfig.Origin, ET, phaseConfig.Axes, self.ephemSmoother)


                # now have unscaled cartesian state w.r.t. the origin of the function in the axes of the phase
            if (phaseConfig.Axes != funConfig.Axes): # axes are different
                # rotate the phase's state to the function's axes (using spice or other)
                #from CoordinateTransformation import CoordinateTransformation
                #from helperData import helperData
                #helper = helperData()
                # the helper knows where the spice kernels are located
                #tlsKernel = self.helperData.getKernel('naifTlsKernel')
                #pckKernel = self.helperData.getKernel('pckKernel')
                #transformer = CoordinateTransformation(tlsKernel=tlsKernel, pckKernel=pckKernel)

                if (phaseConfig.Origin == funConfig.Origin): # if true, then we don't know ET yet; otherwise, we do from above
                    # convert current time (assumed to be in GMAT MJD A.1) to Spice ephemeris time
                    #from TimeUtil import TimeUtil
                    #timeConverter = TimeUtil(tlsKernel)
                    t = self.GetTime(pointIdx)
                    ET = self.timeUtil.MJDA12ET(t, useSpice = 0)

                # transform state
                unscaledState[idxOrbStart:idxOrbEnd] = self.coordinateTransformation.TransformState(phaseConfig.Axes, unscaledState[idxOrbStart:idxOrbEnd], funConfig.Axes, ET)

                # now have unscaled cartesian state w.r.t. the origin of the function in the axes of the function

            # convert phase state to function's representation

            # import state conversion tool
            #from StateConversions import StateConv
            # inputEnum already set appropriately above
            outputEnum = self.StringtoEnum(funConfig.OrbitStateRep)

            # import tool for knowing stuff about central bodies
            # everything is w.r.t. function configuration now
            #from helperData import helperData
            #helper = helperData()
            mu = self.helperData.getMu(funConfig.Origin) # gravitational parameter of central body of function
            unscaledState[idxOrbStart:idxOrbEnd] = StateConv(unscaledState[idxOrbStart:idxOrbEnd], inputEnum, outputEnum, mu) # final state conversion

        return unscaledState

    def GetUnscaledJacobianOfFunVarsWrtDecVarsAndTime(self, pointIdx, returnElements = ['All']):
        """
        Get the Jacobian of the unscaled state in the format desired by the function w.r.t. the unscaled state in the format desired by the decision vector and the unscaled time.
        Here, "format" means origin and axes and representation.
        
        @param pointIdx The function point index at which to obtain the Jacobian
        @param returnElements List of which elements of the state are to be differentiated; e.g., returnElements = ['All'], ['OrbitState'], ['TotalMass'], ['OrbitState', 'TotalMass'].
            If 'All' is specified, the states MUST be in the order they are specified in PhaseConfig.DecVecData and are returned in the order listed in PhaseConfig.DecVecData
            If the list contains multiple strings, then the rows of the Jacobian are returned in the order which the states are listed in returnElements
        @return unscaledJacobianWrtDecVars The unscaled Jacobian of the state in the format desired by the function w.r.t. the unscaled state in the format desired by the decision vector
        @return unscaledJacobianWrtTime The unscaled Jacobian of the state in the format desired by the function w.r.t. the unscaled time
        """

        # start with decision vector in representation, origin, and frame of decision vector

        # get the entire scaled state at this point
        stateFull = self.GetState(pointIdx)

        # now, just extract the states and scaling information we care about
        phaseConfig = self.phaseConfigs[pointIdx] # get the phase configuration
        phaseScaler = self.phaseScalers[pointIdx] # get the phase scaling
        unitsFull = phaseConfig.Units
        orbitConversion = False # initially assume we don't need to do the orbital conversion
        massPresent = False # initially assume no mass until proven otherwise
        if returnElements[0] == 'All':
            state = stateFull # we want everything
            units = unitsFull

            # figure out if we need to do the orbit state conversion
            j = 0
            for decVecData in phaseConfig.DecVecData:
                jNext = j + decVecData[1]
                if decVecData[0] == 'OrbitState':
                    idxOrbStart = j
                    idxOrbEnd = jNext
                    orbitConversion = True

                # also need to know where the total mass is
                if decVecData[0] == 'TotalMass':
                    idxTotalMass = j
                    massPresent = True
                j = jNext
        else:
            state = []
            units = []
            for returnElement in returnElements: # loop through strings in returnElements
                j = 0
                for decVecData in phaseConfig.DecVecData:
                    jNext = j + decVecData[1] # how big is this substate?
                    if returnElement == decVecData[0]:      
                        state.extend(stateFull[j:jNext]) # append the substate
                        units.extend(unitsFull[j:jNext]) # append the units for the substate

                        # figure out if we need to do the orbit state conversion
                        if returnElement == 'OrbitState':
                            idxOrbStart = len(state)-decVecData[1]
                            idxOrbEnd = len(state)
                            orbitConversion = True

                        # also need to know where the total mass is
                        if decVecData[0] == 'TotalMass':
                            idxTotalMass = j
                            massPresent = True
                    j = jNext

            # error check
            if state == []:
                print('GMATFunction.GetUnscaledStateForFun: invalid returnElements input')
            else:
                state = np.array(state)

        # unscale the state at this point (only the parts we care about)
        unscaledState = phaseScaler.UnscaleVector(state, units) # unscale the state

        # allocate for jacobians
        jacFuncVarsWrtPhaseVars = np.zeros((len(unscaledState), len(unscaledState)))
        jacFuncVarsWrtTime = np.zeros(len(unscaledState))

        # do mass up front because it is easy
        # only have to change one element of the jacobian w.r.t. phase variables,
        # and no elements w.r.t. time
        if massPresent == True:
            jacFuncVarsWrtPhaseVars[idxTotalMass, idxTotalMass] = 1.0
        

        # derivative of transformation from decision vector representation to cartesian representation
        # in the math spec, this corresponds to getting:
        # [d f^{S_C / S_D} (z^D)] / [d z^D] (for partials w.r.t. state)
        if orbitConversion:
            # before doing anything else, we need to get the derivatives of the 
            # decision vector (before being transformed at all) w.r.t. time
            # I *think* the partial and total derivatives w.r.t. time will be zero here ...
            PartialOrbitStatePhaseVarsWrtTime = DecVecTimeDerivatives(unscaledState[idxOrbStart:idxOrbEnd],
                                                                      self.StringtoEnum(phaseConfig.OrbitStateRep), phaseConfig.Axes, phaseConfig.Origin)

            JacOrbitStatePhaseVarsTime = PartialOrbitStatePhaseVarsWrtTime


            # convert the state orbit representation to Cartesian if necessary
            # it IS necessary if any of the following is true:
            # 1. the origins of the phase and the function are different AND the phase state rep is not cartesian
            # 2. the axes of the phase and the function are different AND the phase state rep is not cartesian
            # 3. the function state rep is Cartesian AND the phase state rep is not cartesian
            funConfig = self.funConfig # get the function configuration

            if (phaseConfig.Origin != funConfig.Origin or # origins are different
                phaseConfig.Axes != funConfig.Axes): # axes are different
                if 'Cartesian' in phaseConfig.OrbitStateRep: # state came in cartesian
                    # so, derivative of transformation to cartesian state w.r.t. state is identity
                    JacTransformationToCartesianDecVars = np.identity(6)
                    # derivative of transformation w.r.t. time is zero:
                    # [\partial z'] / [\partial t]
                    PartialTransformationToCartesianWrtTime = np.zeros(6)
                else: # state didn't come in in cartesian
                    # import state conversion tool
                    #from StateConversions import StateConv, StateConvJacobian

                    # convert phase state to cartesian
                    inputEnum = self.StringtoEnum(phaseConfig.OrbitStateRep)
                    cartEnum = self.StringtoEnum('Cartesian')

                    # import tool for knowing stuff about central bodies
                    #from helperData import helperData
                    #helper = helperData()
                    mu = self.helperData.getMu(phaseConfig.Origin) # gravitational parameter of central body of phase

                    # not 100% sure this line is "necessary," but getting the state itself in cartesian will probably save time down
                    # the road because we won't have to transform it over and over again later on
                    unscaledState[idxOrbStart:idxOrbEnd] = StateConv(unscaledState[idxOrbStart:idxOrbEnd], inputEnum, cartEnum, mu) # convert to Cartesian

                    # partials of transformation to cartesian
                    # note that the input STATE type is cartesian because we already used StateConv() to transform from decision variables to cartesian
                    JacTransformationToCartesianDecVars = StateConvJacobian(unscaledState[idxOrbStart:idxOrbEnd], cartEnum, inputEnum, cartEnum, mu)

                    # for now, the time Jacobian of changing the state rep is always 0
                    # [\partial z'] / [\partial t]
                    PartialTransformationToCartesianWrtTime = np.zeros(6)

                # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
                inputEnum = self.StringtoEnum('Cartesian') # because we went ahead and converted it to Cartesian if it didn't come in that way

                # we now have the jacobian of:
                # the unscaled state in cartesian coordinates using the axes and origin of the phase
                # with respect to
                # the unscaled state in phase coordinates using the axes and origin of the phase
            else:
                # the origins of the phase and constraint are the same
                # the axes of the phase and constraint are the same
                # so, we don't have to pass through Cartesian necessarily
                JacTransformationToCartesianDecVars = np.identity(6)
                PartialTransformationToCartesianWrtTime = np.zeros(6)

                # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
                inputEnum = self.StringtoEnum(phaseConfig.OrbitStateRep)

            if (phaseConfig.Origin != funConfig.Origin): # origins are different
                # translate the phase's state to the function's origin (using spice)
                # need to calculate [d f^{O_F / O_D} (z')] / [d z'] (for partials w.r.t. state)
                # (derivative of transformation from origin of decision vector frame to origin of function frame)
                # this is where we use EphemSmoother, which must be set before we can do this
                t = self.GetTime(pointIdx)
                ET = self.timeUtil.MJDA12ET(t, useSpice = 0)

                # this function call is only valid if the origins of both frames are celestial bodies
                # returns [d f^{O_F / O_D} (z')] / [d z'] and [\partial f^{O_F / O_D} (z')] / [\partial t]
                # also returns the transformed state itself: z''
                unscaledState[idxOrbStart:idxOrbEnd], JacOriginTransformationDecVars, PartialOriginTransformationWrtTime = self.coordinateTranslation.translateOriginSmoothedWithDerivative(phaseConfig.Origin, 
                                                                                                                                                                                   unscaledState[idxOrbStart:idxOrbEnd], 
                                                                                                                                                                                   funConfig.Origin, 
                                                                                                                                                                                   ET, 
                                                                                                                                                                                   phaseConfig.Axes, 
                                                                                                                                                                                   self.ephemSmoother)
            else:
                # if the origins are the same, then the time jacobian is zero and the state jacobian is zero
                JacOriginTransformationDecVars = np.zeros((6,6))
                # [\partial f^{O_F / O_D} (z')] / [\partial t]:
                PartialOriginTransformationWrtTime = np.zeros(6)
            # we now have the jacobian of:
            # the unscaled state in cartesian coordinates using the axes of the phase and origin of the function
            # with respect to
            # the unscaled state in cartesian coordinates using the axes and origin of the phase

            if (phaseConfig.Axes != funConfig.Axes): # axes are different
                # derivative of transformation from decision vector reference frame to function reference frame:
                # [d f^{F_F / F_D} (z'')] / [d z''] and
                # [\partial f^{F_F / F_D} (z'')] / [\partial t]
                if (phaseConfig.Origin == funConfig.Origin): # if true, then we don't know ET yet; otherwise, we do from above
                    # convert current time (assumed to be in GMAT MJD A.1) to Spice ephemeris time
                    t = self.GetTime(pointIdx)
                    ET = self.timeUtil.MJDA12ET(t, useSpice = 0)

                # transform state and get derivatives
                # for now, we are assuming the axes are the same though, so
                # THIS PART HAS NOT YET BEEN WRITTEN!
                JacAxesTransformationDecVars = np.identity(6)
                PartialAxesTransformationWrtTime = np.zeros(6)
            else:
                # phase and function axes are the same, so time derivative is zero and state derivative is identity
                JacAxesTransformationDecVars = np.identity(6)
                PartialAxesTransformationWrtTime = np.zeros(6)

            # we now have the jacobian of:
            # the unscaled state in cartesian coordinates using the axes and origin of the function
            # with respect to
            # the unscaled state in cartesian coordinates using the axes of the phase and the origin of the function

            # derivative of transformation from cartesian representation to function representation:
            # [d f^{S_F / S_C}] / [d z'''] and
            # [\partial f^{S_F / S_C}] / [\partial z''']
            # inputEnum already set appropriately above
            outputEnum = self.StringtoEnum(funConfig.OrbitStateRep)
            mu = self.helperData.getMu(funConfig.Origin) # gravitational parameter of central body of function

            # do the state conversion itself
            unscaledState[idxOrbStart:idxOrbEnd] = StateConv(unscaledState[idxOrbStart:idxOrbEnd], inputEnum, outputEnum, mu) # final state conversion


            # now do the partials of the state conversion
            # stateInputType is the state for the func
            # jacInputType is: Cartesian if we had to convert to Cartesian earlier; or the phase state type otherwise
            # jacOutputType is the function state type
            # output state is whatever the function state rep is
            #JacTransformationToFuncRepDecVars = StateConvJacobian(unscaledState[idxOrbStart:idxOrbEnd], 
            #                                                      outputEnum, 
            #                                                      self.StringtoEnum(phaseConfig.OrbitStateRep), 
            #                                                      outputEnum, 
            #                                                      mu)

            JacTransformationToFuncRepDecVars = StateConvJacobian(unscaledState[idxOrbStart:idxOrbEnd], 
                                                                  outputEnum, 
                                                                  inputEnum, 
                                                                  outputEnum, 
                                                                  mu)

            # just assume time partials are zero for now
            PartialTransformationToFuncRepWrtTime = np.zeros(6)

            # now that we have the final state form (z^F), get the partials of it w.r.t. time
            PartialOrbitStateFuncVarsWrtTime = DecVecTimeDerivatives(unscaledState[idxOrbStart:idxOrbEnd],
                                                                      outputEnum, funConfig.Axes, funConfig.Origin)

            # finally, combine all the pieces together appropriately

            # state
            jacOrbitStateFuncVarsWrtPhaseVars = np.dot(JacTransformationToFuncRepDecVars, np.dot(JacAxesTransformationDecVars, 
                                                       (np.dot(JacOriginTransformationDecVars, JacTransformationToCartesianDecVars) + 
                                                        JacTransformationToCartesianDecVars)))

            # time

            # parens temp vars go from inside to outside
            parens1 = np.dot(JacTransformationToCartesianDecVars, JacOrbitStatePhaseVarsTime) + PartialOrbitStatePhaseVarsWrtTime
            # not totally sure about PartialOriginTransformationWrtTime going here
            # math spec has time partial of z'', i put the time partial of the origin transformation
            parens2 = np.dot(JacOriginTransformationDecVars, parens1) + PartialTransformationToCartesianWrtTime + PartialOriginTransformationWrtTime
            parens3 = np.dot(JacAxesTransformationDecVars, parens2) + PartialAxesTransformationWrtTime
            jacOrbitStateFuncVarsWrtTime =  np.dot(JacTransformationToFuncRepDecVars, parens3) + PartialOrbitStateFuncVarsWrtTime

            # now, put the orbit state partials into the overall partials array
            jacFuncVarsWrtPhaseVars[idxOrbStart:idxOrbEnd, idxOrbStart:idxOrbEnd] = jacOrbitStateFuncVarsWrtPhaseVars
            jacFuncVarsWrtTime[idxOrbStart:idxOrbEnd] = jacOrbitStateFuncVarsWrtTime
        else:
            # the orbit was not requested
            jacOrbitStateFuncVarsWrtPhaseVars = np.identity(6)
            jacOrbitStateFuncVarsWrtTime = np.zeros(6)

        


        return jacFuncVarsWrtPhaseVars, jacFuncVarsWrtTime
    
    def StringtoEnum(self,stateType):
        """
        this defeats the purpose of the enum, but the rest of my code currently uses strings
        passed in through the user interface and am not ready to make all of that code use the
        enum... SPH
        @param stateType String of name of orbit state representation
        @return enumeration The enum value associated with the string
        """
        
        if stateType == "Cartesian":
           return stateTypes.Cartesian.value;
        elif stateType == "SphericalAzFPA":
           return stateTypes.SphericalAzFPA.value;
        elif stateType == "ClassicalKeplerian":
           return stateTypes.ClassicalKeplerian.value;
       
            
    # deprecated:
    
    
    #def SetDecVecData(self,pointIdx,decVecMap):
    #    """
    #    Sets the decision vector contents given a point index 
    #    @param pointIdx index of point at which to set decision vector data name
    #    @param decVecMap
    #    """
    #    self.decVecData.insert(pointIdx,decVecMap)
    #    return
        
    #def SetFunctionUnits(self,funcUnits):
    #    """Sets the list of function units"""
    #    self.funcUnits = funcUnits
    #    return
        
    #def SetDecVecUnits(self,pointIdx,units):
    #    self.decVecUnits.insert(pointIdx,units)
    #    return

    #def GetState_Rep(self, pointIdx, repName):
    #    """Returns state in desired state representation.
    #       In C++, this function would be overloade with rep as in input type"""
    #    #if (repName in self.decVecData(pointIdx)):

            
    #def SetScaleUtility(self,ScaleUtil):
    #    self.Scaler = ScaleUtil;

    #    return

    #def GetUnscaledState(self,pointIdx, stateType = 'Cartesian'):
        
    #    # Unscale the state
    #    state = self.GetState(pointIdx)
    #    units = self.decVecUnits[pointIdx]
    #    unScaledState = self.Scaler.UnscaleVector(state,units)
        
    #    # Convert state represenation
    #    if self.decVecData[pointIdx].__contains__(stateType):
    #        # The decicion vector state type is the same as
    #        # constraint...no conversion required
    #        return unScaledState
    #    else:
    #        from StateConversions import StateConv
    #        self.StringtoEnum(self.decVecData[pointIdx][0])
    #        inputEnum = self.StringtoEnum(self.decVecData[pointIdx][0])
    #        outputEnum = self.StringtoEnum(stateType)
    #        outPutState = StateConv(unScaledState[0:6], inputEnum, outputEnum , mu = 398600.4415)
    #        unScaledState[0] = outPutState[0]
    #        unScaledState[1] = outPutState[1]
    #        unScaledState[2] = outPutState[2]
    #        unScaledState[3] = outPutState[3]
    #        unScaledState[4] = outPutState[4]
    #        unScaledState[5] = outPutState[5]
    #        return unScaledState

    #def GetUnscaledStateForFun_old(self, pointIdx):
    #    """
    #    Get unscaled state in the format desired by the function.
    #    Here, "format" means origin and axes and representation.
    #    TODO: assumes orbit state is first 6 elements of state vector. is that ok?
        
    #    @param pointIdx The function point index at which to obtain the unscaled state
    #    @return unscaledState The unscaled state in the format desired by the function
    #    """

    #    # get the scaled state at this point
    #    state = self.GetState(pointIdx)

    #    # unscale the state at this point
    #    phaseConfig = self.phaseConfigs[pointIdx] # get the phase configuration
    #    phaseScaler = self.phaseScalers[pointIdx] # get the phase scaling
    #    units = phaseConfig.Units
    #    unscaledState = phaseScaler.UnscaleVector(state, units) # unscale the state

    #    # convert the state orbit representation to Cartesian if necessary
    #    # it IS necessary if any of the following is true:
    #    # 1. the origins of the phase and the function are different AND the phase state rep is not cartesian
    #    # 2. the axes of the phase and the function are different AND the phase state rep is not cartesian
    #    # 3. the function state rep is Cartesian AND the phase state rep is not cartesian
    #    funConfig = self.funConfig # get the function configuration

    #    if (phaseConfig.Origin != funConfig.Origin or # origins are different
    #        phaseConfig.Axes != funConfig.Axes): # axes are different
    #        if 'Cartesian' in phaseConfig.OrbitStateRep: # state came in cartesian
    #            pass # don't do anything
    #        else: # state didn't come in in cartesian
    #            # import state conversion tool
    #            from StateConversions import StateConv

    #            # convert phase state to cartesian
    #            inputEnum = self.StringtoEnum(phaseConfig.OrbitStateRep)
    #            cartEnum = self.StringtoEnum('Cartesian')

    #            # import tool for knowing stuff about central bodies
    #            from helperData import helperData
    #            helper = helperData()
    #            mu = helper.getMu(phaseConfig.Origin) # gravitational parameter of central body of phase
    #            unscaledState[0:6] = StateConv(unscaledState[0:6], inputEnum, cartEnum, mu) # convert to Cartesian

    #        # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
    #        inputEnum = self.StringtoEnum('Cartesian')

    #        # we now have unscaled cartesian state w.r.t. the origin and axes of the phase
    #    else:
    #        # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
    #        inputEnum = self.StringtoEnum(phaseConfig.OrbitStateRep)

    #    if (phaseConfig.Origin != funConfig.Origin): # origins are different
    #        # translate the phase's state to the function's origin (using spice)
    #        from CoordinateTranslation import CoordinateTranslation
    #        from helperData import helperData
    #        helper = helperData()
    #        # the helper knows where the spice kernels are located
    #        tlsKernel = helper.getKernel('naif0012')
    #        deKernel = helper.getKernel('de430')
    #        translator = CoordinateTranslation(tlsKernel=tlsKernel, deKernel=deKernel)

    #        # convert current time (assumed to be in GMAT MJD A.1) to Spice ephemeris time
    #        from TimeUtil import TimeUtil
    #        timeConverter = TimeUtil(tlsKernel)  
    #        t = self.GetTime(pointIdx)
    #        ET = timeConverter.MJDA12ET(t)

    #        # translate state
    #        unscaledState[0:6] = translator.translateOrigin(phaseConfig.Origin, unscaledState[0:6], funConfig.Origin, ET, phaseConfig.Axes)

    #        # now have unscaled cartesian state w.r.t. the origin of the function in the axes of the phase
    #    if (phaseConfig.Axes != funConfig.Axes): # axes are different
    #        # rotate the phase's state to the function's axes (usingspice)
    #        from CoordinateTransformation import CoordinateTransformation
    #        from helperData import helperData
    #        helper = helperData()
    #        # the helper knows where the spice kernels are located
    #        tlsKernel = helper.getKernel('naif0012')
    #        pckKernel = helper.getKernel('pck00010')
    #        transformer = CoordinateTransformation(tlsKernel=tlsKernel, pckKernel=pckKernel)

    #        if (phaseConfig.Origin == funConfig.Origin): # if true, then we don't know ET yet; otherwise, we do from above
    #            # convert current time (assumed to be in GMAT MJD A.1) to Spice ephemeris time
    #            from TimeUtil import TimeUtil
    #            timeConverter = TimeUtil(tlsKernel)
    #            t = self.GetTime(pointIdx)
    #            ET = timeConverter.MJDA12ET(t)

    #        # transform state
    #        unscaledState[0:6] = transformer.TransformState(phaseConfig.Axes, unscaledState[0:6], funConfig.Axes, ET)

    #        # now have unscaled cartesian state w.r.t. the origin of the function in the axes of the function

    #    # convert phase state to function's representation

    #    # import state conversion tool
    #    from StateConversions import StateConv
    #    # inputEnum already set appropriately above
    #    outputEnum = self.StringtoEnum(funConfig.OrbitStateRep)

    #    # import tool for knowing stuff about central bodies
    #    # everything is w.r.t. function configuration now
    #    from helperData import helperData
    #    helper = helperData()
    #    mu = helper.getMu(funConfig.Origin) # gravitational parameter of central body of function
    #    unscaledState[0:6] = StateConv(unscaledState[0:6], inputEnum, outputEnum, mu) # final state conversion

    #    return unscaledState

                 
            
           
        
    
        
        
        
        

        