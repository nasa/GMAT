# AlgebraicFunction.py

# Python implementation of CSALT's AlgebraicFunction.cpp/hpp

# Revision history:
# 2018-11-13; Noble Hatten; Created
# 2018-11-14; Noble Hatten; Changed some returns
# 2018-12-17; Noble Hatten; added some comments

from abc import ABC, abstractmethod # imports for abstract OOP
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian

class AlgebraicFunction(object):
    """AlgebraicFunction class."""

    def __init__(self):
        """constructor"""

        # variables and their default values

        # The number of discretization points that are inpependent variables
        self._numPoints = 0

        # The number of functions
        self._numFunctions = 0

        # Map of which discretization points have state dependency (boolean list)
        self._stateDepMap = []
        
        # Map of which discretization points have control dependency (boolean list)
        self._controlDepMap = []

        # Map of which discretization points have time dependency (boolean list)
        self._timeDepMap = []

        # Map of which discretization points have param dependency (boolean list)
        self._paramDepMap = []

        # States at each discretization point (will be populated as a list of lists)
        self._stateData = []

        # Controls at each discretization point (will be populated as a list of lists)
        self._controlData = []

        # Time at each discretization point (will be populated as a list)
        self._timeData = []

        # Static parameters (will be populated as a list of lists)
        self._paramData = []

        # Function upper bounds if built-in (will be populated as a list of lists)
        self._lowerBounds = []

        # Function lower bounds if built-in (will be populated as a list of lists)
        self._upperBounds = []

        # Used to stored the unscaled version of the function lower bounds (will be populated as a list of lists)
        self._unscaledLowerBounds = []

        # Used to stored the unscaled version of the function upper bounds (will be populated as a list of lists)
        self._unscaledUpperBounds = []

        # The reference epoch used in time scaling
        self._refEpoch = None

        # Time scale factor
        self._TU = 1.0

        # Distance scale factor
        self._DU = 1.0

        # Velocity scale factor
        self._VU = 1.0

        # Mass scale factor
        self._MU = 1.0

        return

    # public methods
    def HasDependency(self, varType, pointIdx):
        """Method to determine whether a discretization point has a dependency on state, control, time, or parameter data"""
        # @param varType The variable type dependency to check for
        # @return True if there is a dependency, fails otherwise
        self._ValidatePointIdx(pointIdx)

        if (varType == "State"):
            return self._stateDepMap(pointIdx)
        if (varType == "Control"):
            return self._controlDepMap(pointIdx)
        if (varType == "Time"):
            return self._timeDepMap(pointIdx)
        if (varType == "Param"):
            return self._paramDepMap(pointIdx)

        return

    def SetData(self, varType, pointIdx, theData, insertionType = "insert"):
        """* Method to set the data on a specified discretization point"""
        # @param varType The variable type to apply the data to
        # @param pointIdx The discretization point where the data is applied
        # @param theData The new input data (scaler or numpy array)
        # @param insertionType "insert" or "overwrite". If "insert", then data is inserted after the pointIdx (e.g., if adding a new point). If "overwrite", then the data of pointIdx is overwritten (it must already exist)
        self._ValidatePointIdx(pointIdx)
        if insertionType == "insert":
            if (varType == "State"):
                self._stateData.insert(pointIdx, theData)
            elif (varType == "Control"):
                self._controlData.insert(pointIdx, theData)
            elif (varType == "Time"):
                self._timeData.insert(pointIdx, theData)
            elif (varType == "Param"):
                self._paramData.insert(pointIdx, theData)
        elif insertionType == "overwrite":
            if (varType == "State"):
                self._stateData[pointIdx] = theData
            elif (varType == "Control"):
                self._controlData[pointIdx] = theData
            elif (varType == "Time"):
                self._timeData[pointIdx] = theData
            elif (varType == "Param"):
                self._paramData[pointIdx] = theData
        return

    def SetScaleFactors(self, referenceEpoch, timeUnit, distUnit, velUnit, massUnit):
        """Method to set scale factors that are used by derived classes"""
        # @param referenceEpoch The reference epoch
        # @param timeUnit The time scale factor
        # @param distUnit The distace scale factor
        # @param velUnit The velocity scale factor
        # @param massUnit The mas scale factor
        self._refEpoch = referenceEpoch
        self._TU = timeUnit
        self._DU = distUnit
        self._VU = velUnit
        self._MU = massUnit
        #self.ScaleFunctionBounds() # this was in the original c++, but i don't think it is necessary
        return

    def EvaluateJacobian(self, varType, pointIdx):
        """* Method to return the Jacobian array if a dependency for the input variable type exists, otherwise [] is returned"""
        # @param varType The variable type to get the Jacobian for
        # @param pointIdx The discretization point to get the Jacobian from
        # @return The Jacobian (numpy array or [])
        jacArray = []
        hasAnalyticJac = False
        if (not self.HasDependency(varType, pointIdx)):
            return jacArray

        jacArray, hasAnalyticJacobian = self.EvaluateAnalyticJacobian(varType, pointIdx)
        if (hasAnalyticJac):
            return jacArray

    def EvaluateFunctions(self):
        """Must be implemented in derived class"""
        raise Exception("EvaluateFunctions must be overridden by a derived class""")
        return

    def EvaluateAnalyticJacobian(self):
        """Must be implemented in derived class"""
        raise Exception("EvaluateAnalyticJacobian must be overridden by a derived class""")
        return

    def GetFunctionBounds(self):
        """Method to return the bounds on the algebraic function"""
        # @return functionLB The function lower bounds (numpy array)
        # @return functionUB The function upper bounds (numpy array)
        functionLB = self._lowerBounds
        functionUB = self._upperBounds
        return functionLB, functionUB

    def SetFunctionBounds(self, functionLB, functionUB):
        """Method to set the bounds on the algebraic function when a derived class implements this method, otherwise an error is returned"""
        # @param functionLB The function lower bounds (numpy array)
        # @param functionUB The function upper bounds (numpy array)
        raise Exception("The bounds on this algebraic function cannot be changed")
        return

    def ScaleFunctionBounds(self):
        """Must be implemented in derived class"""
        raise Exception("ScaleFunctionBounds must be overridden by a derived class""")
        return

    def GetNumFunctions(self):
        """Method to get the number of functions"""
        # @return _numFunctions The total number of functions
        return self._numFunctions

    def GetNumPoints(self):
        """Method to get the number of discretization points"""
        # @return numPoints The number of discretization points
        return self._numPoints

    # protected methods (begin with one underscore)
    def _ValidatePointIdx(self, pointIdx):
        """
        Method to check that an input discretization point index exists
        @param pointIdx Index to validate
        """

        if (pointIdx < 0 or pointIdx >= self._numPoints): # the self._numPoints comparator in c++ is just >, but i think it should be >= because of zero indexing
            raise Exception("Invalid algebraic function point index")
        return
       
    #  New below here
    def SetNumPoints(self,numPoints):
        """
        Sets the number of points on which the function is dependent
        @param numPoints The number of points on which the function is dependent
        """
        self._numPoints = numPoints
        
    def SetNumFunctions(self,numFunctions):
        """
        Sets the dimension of the function vector
        #@param numFunctions The dimension of the function vector
        """
        self._numFunctions = numFunctions;
        
    def SetDependencyMap(self, varType, theMap):
        """
        Set the dependency of the function on variables
        @param varType The variable with respect to which the function's dependency is to be set
        @param theMap The dependency map associated with varType
        """
        if (varType == "State"):
            self._stateDepMap = theMap
        elif (varType == "Control"):
            self._controlDepMap = theMap
        elif (varType == "Time"):
            self._timeDepMap = theMap
        elif (varType == "Param"):
            self._paramDepMap = theMap
        return

    def GetState(self,pointIdx):
        """
        Returns raw optimization state
        @param pointIdx Index of point at which state is to be returned
        @return state The raw optimization state at pointIdx
        """
        return self._stateData[pointIdx]
    
    def GetTime(self,phaseIdx):
        """
        Returns time (independent variable)
        @param phaseIdx Index of point at which time is to be returned
        @return time The time at phaseIdx
        """
        return self._timeData[phaseIdx]
    
    def GetControl(self,phaseIdx):
        """
        Returns control
        @param phaseIdx Index of point at which control is to be returned
        @return control The control at phaseIdx
        """
        return self._controlData[phaseIdx]
    
    def GetParams(self,phaseIdx):
        """
        Returns parameter vector
        @param pphaseIdx Index of point at which the parameter vector is to be returned
        @return params The parameter vector at phaseIdx
        """
        return self._paramData[phaseIdx]