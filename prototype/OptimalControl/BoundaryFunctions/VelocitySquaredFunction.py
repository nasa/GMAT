"""
A simple scalar function: the norm of the orbital velocity squared

2018-12-10; nhatten; created
2019-03-08; nhatten; started working on jacobian

@author: nhatten
"""
    
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian  
import GMATFunction

class VelocitySquaredFunction(GMATFunction.GMATFunction):
    
    def __init__(self):
        """
        Constructor
        """
        
        # Call parent class init method
        super().__init__()
        return

    def EvaluateFunction(self):
        """
        Evaluate the constraint
        @return funcValue The scaled value of the function
        """
        
        state = self.GetUnscaledStateForFun(0, returnElements=['OrbitState']) # get the state in the format desired by the function
        t = self.GetTime(0) # get the time (don't really need it, but just to show how)
        v = np.array(state[3:6]) # extract the velocity from the state

        # the target value of the square of the velocity is set using a custom setting
        target = self.GetCustomSetting('TargetVelocitySquared')

        funcValue = np.linalg.norm(v)**2 - target # create the function value (unscaled)

        # scale the function value
        funcValue = self.funScaler.ScaleVector([funcValue], self.funConfig.Units) # value needs to be passed in as an array because len() is used on it

        return funcValue

    def EvaluateFunctionWithInputs(self, time, state, control, params):
        """
        Evaluate the constraint. This version takes in explicit arguments so that it can be run against AD.
        @param time
        @param state
        @param control
        @param params
        @return funcValue The scaled value of the function
        """

        
        # generalize later, but, for now, there is only 1 phase, and only time and state need to be set
        self.SetData("Time", 0, time)
        self.SetData("State", 0, state)
        funcValue = self.EvaluateFunction()

        return funcValue

    def EvaluateJacobian(self):
        """
        Evaluate and return the Jacobian
        """

        state = self.GetUnscaledStateForFun(0) # get the state in the format desired by the function

        # number of functions and number of points on which the functions depend
        nFun = self.GetNumFunctions()
        nPoints = self.GetNumPoints()
        dimState = len(state)

        # arrays to hold everything (row major: leftmost should change slowest)
        jacFuncWrtFuncVars = np.zeros((nPoints, nFun, dimState))
        partialFuncWrtTime = np.zeros((nPoints, nFun, 1))

        v = np.array(state[3:6]) # extract the velocity from the state
        vmag = np.linalg.norm(v)

        # derivative of constraint w.r.t. function variables
        jacFuncWrtFuncVars[0, 0, 3:6] = 2.0 * v

        # derivative of constraint w.r.t. time
        partialFuncWrtTime[0, 0, 0] = 0.0

        ####
        # chain
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
        # scale
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


    
    
        


