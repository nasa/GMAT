# -*- coding: utf-8 -*-
"""
2019-03-20; nhatten; created


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

class TransformUtil(object):
    """
    Class holds capabilities for transforming states and Jacobians
    """

    def __init__(self):
        """
        constructor
        """

        self.helperData = helperData()
        tlsKernel = self.helperData.getKernel('naifTlsKernel')
        pckKernel = self.helperData.getKernel('pckKernel')
        deKernel = self.helperData.getKernel('deKernel')
        self.coordinateTransformation = CoordinateTransformation(tlsKernel=tlsKernel, pckKernel=pckKernel)
        self.coordinateTranslation = CoordinateTranslation(tlsKernel=tlsKernel, deKernel=deKernel)

        self.timeUtil = TimeUtil(tlsKernel) 

        return

    def TransformOrbitalState(self, stateIn, timeIn, originIn, axesIn, stateRepIn, originOut, axesOut, stateRepOut, ephemType = 'Spline', ephemSmoother = None):
        """
        Transform orbital state from one representation/frame/origin to another
        @param stateIn Input orbital state
        @param timeIn Input time in MJDA.1
        @param originIn Origin of coordinate system in which input state is defined. currently must be a celestial body recognizable by spice
        @param axesIn Reference frame in which the input state is defined.
        @param stateRepIn State representation of the input state
        @param originOut Origin of coordinate system in which output state is defined. currently must be a celestial body recognizable by spice
        @param axesOut Reference frame in which the output state is defined.
        @param stateRepOut State representation of the output state
        @param ephemType "Spice" or "Spline": how do you look up where planets are?
        @param ephemSmoother ephemSmoother object used to find where planets are
        @return stateOut Output orbital state
        """

        # convert the state orbit representation to Cartesian if necessary
        # it IS necessary if any of the following is true:
        # 1. the origins of the input and the output are different AND the input state rep is not cartesian
        # 2. the axes of the input and the output are different AND the input state rep is not cartesian
        # 3. the output state rep is Cartesian AND the input state rep is not cartesian

        stateOut = stateIn # start with this, and change it as we go

        if (originIn != originOut or # origins are different
            axesIn != axesOut): # axes are different
            if 'Cartesian' in stateRepIn: # state came in cartesian
                pass # don't do anything
            else: # state didn't come in in cartesian
                # convert phase state to cartesian
                inputEnum = self.StringtoEnum(stateRepIn)
                cartEnum = self.StringtoEnum('Cartesian')
                mu = self.helperData.getMu(originIn) # gravitational parameter of central body of phase
                stateOut = StateConv(stateOut, inputEnum, cartEnum, mu) # convert to Cartesian

            # set the representation of the orbital state of the input so we know to convert it to that of the output later, if necessary
            inputEnum = self.StringtoEnum('Cartesian')

            # we now have unscaled cartesian state w.r.t. the origin and axes of the input
        else:
            # set the representation of the orbital state of the input so we know to convert it to that of the output later, if necessary
            inputEnum = self.StringtoEnum(stateRepIn)

        if (originIn != originOut): # origins are different
            # translate the input's state to the output's origin (using spice)
            # convert current time (assumed to be in GMAT MJD A.1) to Spice ephemeris time  
            ET = self.timeUtil.MJDA12ET(timeIn, useSpice = 0)

            # translate state
            if ephemType == 'Spice':
                stateOut = self.coordinateTranslation.translateOrigin(originIn, stateOut, originOut, ET, axesIn)
            elif ephemType == 'Spline':
                stateOut = self.coordinateTranslation.translateOriginSmoothed(originIn, stateOut, originOut, ET, axesIn, ephemSmoother)


            # now have unscaled cartesian state w.r.t. the origin of the output in the axes of the input
        if (axesIn != axesOut): # axes are different
            # rotate the input's state to the output's axes (using spice or other)

            if (originIn == originOut): # if true, then we don't know ET yet; otherwise, we do from above
                ET = self.timeUtil.MJDA12ET(timeIn, useSpice = 0)

            # transform state
            stateOut = self.coordinateTransformation.TransformState(axesIn, stateOut, axesOut, ET)

            # now have unscaled cartesian state w.r.t. the origin of the output in the axes of the output

        # convert input state to output's representation

        # inputEnum already set appropriately above
        outputEnum = self.StringtoEnum(stateRepOut)

        mu = self.helperData.getMu(originOut) # gravitational parameter of central body of output
        stateOut = StateConv(stateOut, inputEnum, outputEnum, mu) # final state conversion

        return stateOut

    def CalculateOrbitalJacobian(self, stateIn, timeIn, originIn, axesIn, stateRepIn, originOut, axesOut, stateRepOut, ephemType = 'Spline', ephemSmoother = None):
        """
        Calculate Jacobian of transformation from one representation/frame/origin to another
        @param stateIn Input orbital state
        @param timeIn Input time in MJDA.1
        @param originIn Origin of coordinate system in which input state is defined. currently must be a celestial body recognizable by spice
        @param axesIn Reference frame in which the input state is defined.
        @param stateRepIn State representation of the input state
        @param originOut Origin of coordinate system in which output state is defined. currently must be a celestial body recognizable by spice
        @param axesOut Reference frame in which the output state is defined.
        @param stateRepOut State representation of the output state
        @param ephemType "Spice" or "Spline": how do you look up where planets are?
        @param ephemSmoother ephemSmoother object used to find where planets are
        @return JacWrtInputVars Jacobian w.r.t. stateIn
        @return JacWrtTime Jacobian w.r.t. time
        """

        stateOut = stateIn

        # derivative of transformation from input representation to cartesian representation
        # in the math spec, this corresponds to getting:
        # [d f^{S_C / S_D} (z^D)] / [d z^D] (for partials w.r.t. state)

        # before doing anything else, we need to get the derivatives of the 
        # input (before being transformed at all) w.r.t. time
        # I *think* the partial and total derivatives w.r.t. time will be zero here ...
        PartialOrbitStateInputVarsWrtTime = DecVecTimeDerivatives(stateIn,
                                                                    self.StringtoEnum(stateRepIn), axesIn, originIn)

        JacOrbitStateInputVarsTime = PartialOrbitStateInputVarsWrtTime


        # convert the state orbit representation to Cartesian if necessary
        # it IS necessary if any of the following is true:
        # 1. the origins of the input and the output are different AND the input state rep is not cartesian
        # 2. the axes of the input and the output are different AND the input state rep is not cartesian
        # 3. the output state rep is Cartesian AND the input state rep is not cartesian

        if (originIn != originOut or # origins are different
            axesIn != axesOut): # axes are different
            if 'Cartesian' in stateRepIn: # state came in cartesian
                # so, derivative of transformation to cartesian state w.r.t. state is identity
                JacTransformationToCartesianInputVars = np.identity(6)
                # derivative of transformation w.r.t. time is zero:
                # [\partial z'] / [\partial t]
                PartialTransformationToCartesianWrtTime = np.zeros(6)
            else: # state didn't come in in cartesian
                # convert phase state to cartesian
                inputEnum = self.StringtoEnum(stateRepIn)
                cartEnum = self.StringtoEnum('Cartesian')
                mu = self.helperData.getMu(originIn) # gravitational parameter of central body of input

                # not 100% sure this line is "necessary," but getting the state itself in cartesian will probably save time down
                # the road because we won't have to transform it over and over again later on
                stateOut = StateConv(stateOut, inputEnum, cartEnum, mu) # convert to Cartesian

                # partials of transformation to cartesian
                # note that the input STATE type is cartesian because we already used StateConv() to transform from input to cartesian
                JacTransformationToCartesianInputVars = StateConvJacobian(stateOut, cartEnum, inputEnum, cartEnum, mu)

                # for now, the time Jacobian of changing the state rep is always 0
                # [\partial z'] / [\partial t]
                PartialTransformationToCartesianWrtTime = np.zeros(6)

            # set the representation of the orbital state of the phase so we know to convert it to that of the function later, if necessary
            inputEnum = self.StringtoEnum('Cartesian') # because we went ahead and converted it to Cartesian if it didn't come in that way

            # we now have the jacobian of:
            # the unscaled state in cartesian coordinates using the axes and origin of the input
            # with respect to
            # the unscaled state in input coordinates using the axes and origin of the input
        else:
            # the origins of the input and output are the same
            # the axes of the input and output are the same
            # so, we don't have to pass through Cartesian necessarily
            JacTransformationToCartesianInputVars = np.identity(6)
            PartialTransformationToCartesianWrtTime = np.zeros(6)

            # set the representation of the orbital state of the input so we know to convert it to that of the output later, if necessary
            inputEnum = self.StringtoEnum(stateRepIn)

        if (originIn != originOut): # origins are different
            # translate the input's state to the output's origin (using spice)
            # need to calculate [d f^{O_F / O_D} (z')] / [d z'] (for partials w.r.t. state)
            # (derivative of transformation from origin of input frame to origin of output frame)
            # this is where we use EphemSmoother, which must be set before we can do this
            ET = self.timeUtil.MJDA12ET(timeIn, useSpice = 0)

            # this function call is only valid if the origins of both frames are celestial bodies
            # returns [d f^{O_F / O_D} (z')] / [d z'] and [\partial f^{O_F / O_D} (z')] / [\partial t]
            # also returns the transformed state itself: z''
            stateOut, JacOriginTransformationInputVars, PartialOriginTransformationWrtTime = self.coordinateTranslation.translateOriginSmoothedWithDerivative(originIn, 
                                                                                                                                                                                stateOut, 
                                                                                                                                                                                originOut, 
                                                                                                                                                                                ET, 
                                                                                                                                                                                axesIn, 
                                                                                                                                                                                ephemSmoother)
        else:
            # if the origins are the same, then the time jacobian is zero and the state jacobian is zero
            JacOriginTransformationInputVars = np.zeros((6,6))
            # [\partial f^{O_F / O_D} (z')] / [\partial t]:
            PartialOriginTransformationWrtTime = np.zeros(6)
        # we now have the jacobian of:
        # the unscaled state in cartesian coordinates using the axes of the input and origin of the output
        # with respect to
        # the unscaled state in cartesian coordinates using the axes and origin of the input

        if (axesIn != axesOut): # axes are different
            # derivative of transformation from input reference frame to output reference frame:
            # [d f^{F_F / F_D} (z'')] / [d z''] and
            # [\partial f^{F_F / F_D} (z'')] / [\partial t]
            if (originIn == originOut): # if true, then we don't know ET yet; otherwise, we do from above
                ET = self.timeUtil.MJDA12ET(timeIn, useSpice = 0)

            # transform state and get derivatives
            # for now, we are assuming the axes are the same though, so
            # THIS PART HAS NOT YET BEEN WRITTEN!
            JacAxesTransformationInputVars = np.identity(6)
            PartialAxesTransformationWrtTime = np.zeros(6)
        else:
            # input and output axes are the same, so time derivative is zero and state derivative is identity
            JacAxesTransformationInputVars = np.identity(6)
            PartialAxesTransformationWrtTime = np.zeros(6)

        # we now have the jacobian of:
        # the unscaled state in cartesian coordinates using the axes and origin of the output
        # with respect to
        # the unscaled state in cartesian coordinates using the axes of the input and the origin of the output

        # derivative of transformation from cartesian representation to output representation:
        # [d f^{S_F / S_C}] / [d z'''] and
        # [\partial f^{S_F / S_C}] / [\partial z''']
        # inputEnum already set appropriately above
        outputEnum = self.StringtoEnum(stateRepOut)
        mu = self.helperData.getMu(originOut) # gravitational parameter of central body of function

        # do the state conversion itself
        stateOut = StateConv(stateOut, inputEnum, outputEnum, mu) # final state conversion


        # now do the partials of the state conversion
        # stateInputType is the state for the output
        # jacInputType is: Cartesian if we had to convert to Cartesian earlier; or the input state type otherwise
        # jacOutputType is the output state type
        # output state is whatever the output state rep is
        JacTransformationToOutputRepInputVars = StateConvJacobian(stateOut, 
                                                                outputEnum, 
                                                                inputEnum, 
                                                                outputEnum, 
                                                                mu)

        # just assume time partials are zero for now
        PartialTransformationToOutputRepWrtTime = np.zeros(6)

        # now that we have the final state form (z^F), get the partials of it w.r.t. time
        PartialOrbitStateOutputVarsWrtTime = DecVecTimeDerivatives(stateOut,
                                                                    outputEnum, axesOut, originOut)

        # finally, combine all the pieces together appropriately

        # state
        jacOrbitStateOutputVarsWrtInputVars = np.dot(JacTransformationToOutputRepInputVars, np.dot(JacAxesTransformationInputVars, 
                                                    (np.dot(JacOriginTransformationInputVars, JacTransformationToCartesianInputVars) + 
                                                    JacTransformationToCartesianInputVars)))

        # time

        # parens temp vars go from inside to outside
        parens1 = np.dot(JacTransformationToCartesianInputVars, JacOrbitStateInputVarsTime) + PartialOrbitStateInputVarsWrtTime
        # not totally sure about PartialOriginTransformationWrtTime going here
        # math spec has time partial of z'', i put the time partial of the origin transformation
        parens2 = np.dot(JacOriginTransformationInputVars, parens1) + PartialTransformationToCartesianWrtTime + PartialOriginTransformationWrtTime
        parens3 = np.dot(JacAxesTransformationInputVars, parens2) + PartialAxesTransformationWrtTime
        jacOrbitStateOutputVarsWrtTime =  np.dot(JacTransformationToOutputRepInputVars, parens3) + PartialOrbitStateOutputVarsWrtTime

        # now, put the orbit state partials into the overall partials array
        #jacFuncVarsWrtPhaseVars[idxOrbStart:idxOrbEnd, idxOrbStart:idxOrbEnd] = jacOrbitStateFuncVarsWrtPhaseVars
        #jacFuncVarsWrtTime[idxOrbStart:idxOrbEnd] = jacOrbitStateFuncVarsWrtTime
        
        return jacOrbitStateOutputVarsWrtInputVars, jacOrbitStateOutputVarsWrtTime

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
