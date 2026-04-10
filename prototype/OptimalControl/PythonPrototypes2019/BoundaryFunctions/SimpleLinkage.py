# -*- coding: utf-8 -*-
"""
Created on Mon Nov 19 16:28:37 2018
2018-12-10; nhatten; replaced EvaluateFunction and renamed the previous version EvaluateFunction_old

@author: sphughe1
@author: nhatten
"""
    
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import GMATFunction

class LinkageConstraint(GMATFunction.GMATFunction):
    
    def __init__(self):
        """
        Constructor
        """
        
        # Call parent class init method
        super().__init__()
        return

    def EvaluateFunction(self):
        """
        Evaluate a simple linkage constraint
        @return funcValue The value of the constraint
        """

        # get the state and time at the 2 points that are supposed to be linked
        state1 = self.GetUnscaledStateForFun(0)
        state2 = self.GetUnscaledStateForFun(1)
        t1 = self.GetTime(0)
        t2 = self.GetTime(1)
        
        # create function vector
        funcValue = np.zeros(8)

        # populate function vector
        funcValue[0] = t2-t1
        funcValue[1] = state2[0] - state1[0]
        funcValue[2] = state2[1] - state1[1]
        funcValue[3] = state2[2] - state1[2]
        funcValue[4] = state2[3] - state1[3]
        funcValue[5] = state2[4] - state1[4]
        funcValue[6] = state2[5] - state1[5]
        funcValue[7] = state2[6] - state1[6]

        # scale function vector
        funcValue = self.funScaler.ScaleVector(funcValue, self.funConfig.Units)
            
        return funcValue

    def EvaluateAnalyticJacobian(self):
        """
        Evaluate Jacobian of simple linkage constraint
        @ return jacValue
        """

        # get the state and time at the 2 points that are supposed to be linked
        # point 0 is the "minus" point, and point 1 is the "plus" point in the math spec
        state1 = self.GetUnscaledStateForFun(0)
        state2 = self.GetUnscaledStateForFun(1)
        t1 = self.GetTime(0)
        t2 = self.GetTime(1)

        # number of functions and number of points on which the functions depend
        nFun = self.GetNumFunctions()
        nPoints = self.GetNumPoints()
        dimState = len(state1)

        # arrays to hold everything (row major: leftmost should change slowest)
        jacFuncWrtFuncVars = np.zeros((nPoints, nFun, dimState))
        partialFuncWrtTime = np.zeros((nPoints, nFun, 1))


        ####
        # derivatives w.r.t. the function's state and time at point 0
        ####

        # state
        jacFuncWrtFuncVars[0, 1:4, 0:3] = -np.identity(3)
        jacFuncWrtFuncVars[0, 4:7, 3:6] = -np.identity(3)
        jacFuncWrtFuncVars[0, 7, 6] = -1.0

        # time
        partialFuncWrtTime[0, 0, 0] = -1.0 # this is func / DAYS. scale to func / seconds for compatibility with spline partials
        partialFuncWrtTime[0, 0, 0] /= 86400.0

        ####
        # derivatives w.r.t. the function's state and time at point 1
        ####

        # state
        jacFuncWrtFuncVars[1, 1:4, 0:3] = np.identity(3)
        jacFuncWrtFuncVars[1, 4:7, 3:6] = np.identity(3)
        jacFuncWrtFuncVars[1, 7, 6] = 1.0

        # time
        partialFuncWrtTime[1, 0, 0] = 1.0 # this is func / DAYS. scale to func / seconds for compatibility with spline partials
        partialFuncWrtTime[1, 0, 0] /= 86400.0

        ####
        # chain back to the points' states and times
        ####
        jacFuncWrtPhaseVars = np.zeros((nPoints, nFun, dimState))
        jacFuncWrtTime = np.zeros((nPoints, nFun, 1))
        for pointIdx in range(nPoints):
            # get the jacobian of the transformation from decision variables to function: d z^F / d z^D
            jacFuncVarsWrtPhaseVars, jacFuncVarsWrtTime = self.GetUnscaledJacobianOfFunVarsWrtDecVarsAndTime(pointIdx)

            # chain rule to get derivatives of function w.r.t. decision variables and time
            jacFuncWrtPhaseVars[pointIdx, 0:nFun, 0:dimState] = np.dot(jacFuncWrtFuncVars[pointIdx, 0:nFun, 0:dimState], jacFuncVarsWrtPhaseVars)
            jacFuncWrtTime[pointIdx, 0:nFun, 0] = np.dot(jacFuncWrtFuncVars[pointIdx, 0:nFun, 0:dimState], jacFuncVarsWrtTime) + partialFuncWrtTime[pointIdx, 0:nFun, 0]


        ####
        # scaling
        ####
        jacFuncWrtPhaseVarsScaled = np.empty((nPoints, nFun, dimState))
        jacFuncWrtTimeScaled = np.empty((nPoints, nFun, 1))

        # loop through points to scale by variables
        for pointIdx in range(nPoints):
            # state
            jacFuncWrtPhaseVarsScaled[pointIdx, 0:nFun, 0:dimState] = self.phaseScalers[pointIdx].ScaleJacobianByVars(jacFuncWrtPhaseVars[pointIdx, 0:nFun, 0:dimState], self.phaseConfigs[pointIdx].Units)

            # time not currently scaled with variables

            # scale by function

            # state
            jacFuncWrtPhaseVarsScaled[pointIdx, 0:nFun, 0:dimState] = self.funScaler.ScaleJacobianByFun(jacFuncWrtPhaseVarsScaled[pointIdx, 0:nFun, 0:dimState], self.funConfig.Units)
            # time
            jacFuncWrtTimeScaled[pointIdx, 0:nFun] = self.funScaler.ScaleJacobianByFun(jacFuncWrtTime[pointIdx, 0:nFun], self.funConfig.Units)


        

        return jacFuncWrtPhaseVarsScaled, jacFuncWrtTimeScaled
        
    def EvaluateFunction_old(self, scale = 0):
        """
        Evaluate a simple linkage constraint
        """
        state1 = self.GetUnscaledState(0,stateType = "Cartesian");
        state2 = self.GetUnscaledState(1,stateType = "Cartesian");
        t1 = self.GetTime(0);
        t2 = self.GetTime(1);

        funcValue = np.zeros(8)
        funcValue[0] = t2-t1
        funcValue[1] = state2[0] - state1[0]
        funcValue[2] = state2[1] - state1[1]
        funcValue[3] = state2[2] - state1[2]
        funcValue[4] = state2[3] - state1[3]
        funcValue[5] = state2[4] - state1[4]
        funcValue[6] = state2[5] - state1[5]
        funcValue[7] = state2[6] - state1[6]
        
        if scale:
            funcValue = self.Scaler.ScaleVector(funcValue,self.funcUnits)
            
        return funcValue
    
        


