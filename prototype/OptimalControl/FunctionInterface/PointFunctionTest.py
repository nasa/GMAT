# PointFunctionTest.py

# Class for testing to see if things work as expected

# Revision history:
# 2018-11-13; Noble Hatten; Created
# 2018-11-14; Noble Hatten; Changed some returns

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import AlgebraicFunction

class PointFunctionTest(AlgebraicFunction.AlgebraicFunction):
    """PointFunctionTest class"""

    def __init__(self):
        """constructor"""

        # call base constructor
        super().__init__()
        return

    # public methods
    def EvaluateFunctions(self):
        """Method to evaluate the point functions"""
        # @return functions The evaluated function(s) as a list
        functions = []
        functions.append(self._stateData[0][1] + self._controlData[0][1] * self._paramData[0][1] + self._timeData[0])
        functions.append(self._stateData[0][2] * self._controlData[0][1] + self._paramData[0][2] * self._paramData[0][3])
        return functions

    def EvaluateAnalyticJacobian(self, varType, pointIdx):
        """Method to evaluate the analytic Jacobian"""
        # @param varType The variable type to get the Jacobian for
        # @param pointIdx The discretization point to get the Jacobian from
        # @return jacArray The Jacobian as a 2D numpy array
        # @return hasAnalyticJac True if the function has an analytic Jacobian
        # I *think* this is what it's supposed to look like
        hasAnalyticJac = True

        if (varType == "State"):
            jacArray = np.array([[0.0, 1.0, 0.0], [0.0, 0.0, self._controlData[pointIdx][1]]])
        if (varType == "Control"):
            jacArray = np.array([[0.0, self._paramData[pointIdx][1]], [0.0, self._stateData[pointIdx][2]]])
        if (varType == "Time"):
            jacArray = np.array([1.0, 0.0])
        if (varType == "Param"):
            jacArray = np.array([[0.0, self._controlData[pointIdx][1], 0.0, 0.0], [0.0, 0.0, self._paramData[pointIdx][3], self._paramData[pointIdx][2]]])
        return jacArray, hasAnalyticJac

    def SetFunctionBounds(self, functionLB, functionUB):
        """Method to set the bounds on the algebraic function"""
        # @param functionLB The function lower bounds (list)
        # @param functionUB The function upper bounds (list)
        self._unscaledLowerBounds = functionLB
        self._unscaledUpperBounds = functionUB
        self.ScaleFunctionBounds()
        return

    def ScaleFunctionBounds(self):
        """Method to scale the function bounds"""
        self._lowerBounds = self._unscaledLowerBounds
        self._upperBounds = self._unscaledUpperBounds
        self._lowerBounds[0] = self._unscaledLowerBounds[0] / self._DU
        self._upperBounds[0] = self._unscaledUpperBounds[0] / self._DU
        self._lowerBounds[1] = self._unscaledLowerBounds[1] / self._MU
        self._upperBounds[1] = self._unscaledUpperBounds[1] / self._MU
        return