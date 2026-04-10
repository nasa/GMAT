# -*- coding: utf-8 -*-
"""
Created on Wed Nov 14 10:46:09 2018
Revised 2018-11-28 by nhatten
nhatten; 2019-03-12; added individual Jacobian scaling/unscaling routines for scaling by function and by variable

@author: sphughe1
@author: nhatten
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian


class ScaleUtility(object):
    """ Helper class for scaling vectors and Jacobians.
        Vector scaling is performed using 
           x_nondim(i) = (x_dim(i) - unitShifts(i))/unitFactors(i)
           
           # TODO: Check that unitFactors elements are not zero because it will lead to a divide-by-zero

    """
    
    def __init__(self):
        """Constructor"""
        # Set commonly used units.
        # The user can also set new units through the AddUnitAndShift() method
        self._unitFactors = {
		                  "DU" : 1.0,
		                  "TU" : 1.0,
		                  "VU" : 1.0,
		                  "MU" : 1.0,
		                  "ACCU" : 1.0,
		                  "MFU" : 1.0}
       
        self._unitShifts = {
		                  "DU" : 0.0,
		                  "TU" : 0.0,
		                  "VU" : 0.0,
		                  "MU" : 0.0,
		                  "ACCU" : 0.0,
		                  "MFU" : 0.0}
        
        return

    def ValidateUnit(self, unitName):
        """Validate that a unitName is in the unitFactor and unitShift dictionaries"""
        # @param unitName The name of the unit
        # @return unitOK boolean; true if unit is in dictionaries, false otherwise
        unitOK = unitName in self._unitFactors and unitName in self._unitShifts

        if unitOK == False:
            print('ScaleUtility.ValidateUnit: unitName not in appropriate dictionaries')
            print('Not scaling this element.')
        return unitOK
    
    def SetUnit(self,unitName,unitValue):
        """Set an element of unitFactors."""
        # @param unitName The name of the unit
        # @param unitValue The value for scaling the unit

        if self.ValidateUnit(unitName):
            self._unitFactors[unitName] = unitValue
        return
       
    def SetShift(self,unitName,unitShift):
        """Set an element of unitShifts."""
        # @param unitName The name of the unit
        # @param unitShift The scale shift for the unit

        if self.ValidateUnit(unitName):
            self._unitShifts[unitName]  = unitShift
        return

    def SetUnitAndShift(self,unitName,unitValue,unitShift):
        """Set an element of unitFactors and the same element of unitShifts."""
        # This method can also be used 
        # @param unitName The name of the unit
        # @param unitValue The value for scaling the unit
        # @param unitShift The scale shift for the unit
        if self.ValidateUnit(unitName):
            self._unitFactors[unitName] = unitValue
            self._unitShifts[unitName]  = unitShift
        return

    def AddUnitAndShift(self,unitName,unitValue,unitShift):
        """Add an element of unitFactors and set the unitValue and unitShift."""
        # In python, this method is the same as calling SetUnitAndShift(). 
        # @param unitName The name of the unit
        # @param unitValue The value for scaling the unit
        # @param unitShift The scale shift for the unit
        self._unitFactors[unitName] = unitValue
        self._unitShifts[unitName]  = unitShift
        return
        
    def ScaleVector(self, vectorUnscaled, units):
        """Scale a vector given the unscaled vector and units"""
        # @param vectorUnscaled The vector of data to non-dimensionalize
        # @param units List of units of the data stored in vector
        # @return vectorScaled Scaled version of vectorUnscaled
        nUnits = len(units)
        if nUnits != len(vectorUnscaled):
            # Validate that vector has as many rows as units.
            print('ScaleUtility.ScaleVector: vectorUnscaled does not have the same number of rows as there are elements in units.')
            print('Scaling not performed.')
            vectorScaled = vectorUnscaled
        else:
            vectorScaled = np.empty((nUnits))
            for vecIdx in range(nUnits):
                unit = units[vecIdx]
                if self.ValidateUnit(unit):
                    vectorScaled[vecIdx] = (vectorUnscaled[vecIdx] - self._unitShifts[unit])/self._unitFactors[unit]
                else:
                    # no scaling
                    vectorScaled[vecIdx] = vectorUnscaled[vecIdx]
        return vectorScaled
    
    def UnscaleVector(self, vectorScaled, units):
        """Unscale a vector given the scaled vector and units"""
        # @param vectorScaled The vector of data to (re-)dimensionalize
        # @param units List of units of the data stored in vector
        # @return vectorUnscaled Unscaled version of vectorScaled
        
        nUnits = len(units)
        if nUnits != len(vectorScaled):
            # Validate that vector has as many rows as units.
            print('ScaleUtility.UnscaleVector: vectorScaled does not have the same number of rows as there are elements in units.')
            print('Scaling not performed.')
            vectorUnscaled = vectorScaled
        else:
            vectorUnscaled = np.empty((nUnits))
            for vecIdx in range(nUnits):
                unit = units[vecIdx]
                if self.ValidateUnit(unit):
                    vectorUnscaled[vecIdx] = vectorScaled[vecIdx] * self._unitFactors[unit] + self._unitShifts[unit]
                else:
                    # no unscaling
                    vectorUnscaled[vecIdx] = vectorScaled[vecIdx]
        return vectorUnscaled
    
    def ScaleJacobian(self, jacArrayUnscaled, funUnits, varUnits):
        """Scale a Jacobian given unscaled Jacobian array, function units, and variable units"""
        # @param jacArrayUnscaled The Jacobian to non-dimensionalize
        # @param funUnits List of units of the data stored in function (columns)
        # @param varUnits List of units of the data stored in variables (rows)
        # @return jacArray Scaled version of the input jacArray
        nFun = len(funUnits)
        nVar = len(varUnits)
        shapeJac = jacArrayUnscaled.shape
        if nFun != shapeJac[0]:
            # Validate that jacArray has as many rows as funUnits.
            jacArrayScaled = jacArrayUnscaled
            print('ScaleUtility.ScaleJacobian: jacArrayUnscaled does not have the same number of rows as there are elements in funUnits.')
            print('Scaling not performed.')
        elif nVar != shapeJac[1]:
            # Validate that jacArray has as many columns as varUnits
            jacArrayScaled = jacArrayUnscaled
            print('ScaleUtility.ScaleJacobian: jacArrayUnscaled does not have the same number of columns as there are elements in varUnits.')
            print('Scaling not performed.')
        else:
            jacArrayScaled = np.empty((nFun, nVar))
            for funIdx in range(nFun):
                funU = funUnits[funIdx]
                for varIdx in range(nVar):
                    varU = varUnits[varIdx]
                    if self.ValidateUnit(funU) and self.ValidateUnit(varU):
                        jacArrayScaled[funIdx,varIdx] = jacArrayUnscaled[funIdx,varIdx] * self._unitFactors[varU] / self._unitFactors[funU]
                    else:
                        # no scaling
                        jacArrayScaled[funIdx,varIdx] = jacArrayUnscaled[funIdx,varIdx]
                
        return jacArrayScaled

    def ScaleJacobianByVars(self, jacArrayUnscaled, varUnits):
        """
        Scale a Jacobian given unscaled Jacobian array and variable units.
        Scaling by function units is done separately
        """
        # @param jacArrayUnscaled The Jacobian to non-dimensionalize
        # @param varUnits List of units of the data stored in variables (rows)
        # @return jacArrayScaled Scaled version of the input jacArray
        nVar = len(varUnits)
        shapeJac = jacArrayUnscaled.shape
        nFun = shapeJac[0]
        if nVar != shapeJac[1]:
            # Validate that jacArray has as many columns as varUnits
            jacArrayScaled = jacArrayUnscaled
            print('ScaleUtility.ScaleJacobianByVars: jacArrayUnscaled does not have the same number of columns as there are elements in varUnits.')
            print('Scaling not performed.')
        else:
            jacArrayScaled = np.empty((nFun, nVar))
            for varIdx in range(nVar):
                varU = varUnits[varIdx]
                if self.ValidateUnit(varU):
                    jacArrayScaled[0:nFun,varIdx] = jacArrayUnscaled[0:nFun,varIdx] * self._unitFactors[varU]
                else:
                    # no scaling
                    jacArrayScaled[0:nFun,varIdx] = jacArrayUnscaled[0:nFun,varIdx]
                
        return jacArrayScaled

    def ScaleJacobianByFun(self, jacArrayUnscaled, funUnits):
        """
        Scale a Jacobian given unscaled Jacobian array and function units.
        Scaling by variable units is done separately
        """
        # @param jacArrayUnscaled The Jacobian to non-dimensionalize
        # @param funUnits List of units of the data stored in function (columns)
        # @return jacArrayScaled Scaled version of the input jacArray
        nFun = len(funUnits)
        shapeJac = jacArrayUnscaled.shape
        nVar = shapeJac[1]
        if nFun != shapeJac[0]:
            # Validate that jacArray has as many rows as funUnits
            jacArrayScaled = jacArrayUnscaled
            print('ScaleUtility.ScaleJacobianByFun: jacArrayUnscaled does not have the same number of rows as there are elements in funUnits.')
            print('Scaling not performed.')
        else:
            jacArrayScaled = np.empty((nFun, nVar))
            for funIdx in range(nFun):
                funU = funUnits[funIdx]
                if self.ValidateUnit(funU):
                    jacArrayScaled[funIdx,0:nVar] = jacArrayUnscaled[funIdx,0:nVar] / self._unitFactors[funU]
                else:
                    # no scaling
                    jacArrayScaled[funIdx,0:nVar] = jacArrayUnscaled[funIdx,0:nVar]
                
        return jacArrayScaled
    
    def UnscaleJacobian(self, jacArrayScaled, funUnits, varUnits):
        """ Unscales a Jacobian given scaled Jacobian array, function units, and variable units"""
        # @param jacArrayScaled The Jacobian to (re-)dimensionalize
        # @param funUnits List of units of the data stored in function (columns)
        # @param varUnits List of units of the data stored in variables (rows)
        # @return jacArrayUnscaled Unscaled version of the input jacArrayScaled
        nFun = len(funUnits)
        nVar = len(varUnits)
        shapeJac = jacArrayScaled.shape
        if nFun != shapeJac[0]:
            # Validate that jacArray has as many rows as funUnits.
            jacArrayUnscaled = jacArrayScaled
            print('ScaleUtility.UnscaleJacobian: jacArrayScaled does not have the same number of rows as there are elements in funUnits.')
            print('Scaling not performed.')
        elif nVar != shapeJac[1]:
            # Validate that jacArray has as many columns as varUnits
            jacArrayUnscaled = jacArrayScaled
            print('ScaleUtility.UnscaleJacobian: jacArrayScaled does not have the same number of columns as there are elements in varUnits.')
            print('Scaling not performed.')
        else:
            jacArrayUnscaled = np.empty((nFun, nVar))
            for funIdx in range(nFun):
                funU = funUnits[funIdx]
                for varIdx in range(nVar):
                    varU = varUnits[varIdx]
                    if self.ValidateUnit(funU) and self.ValidateUnit(varU):
                        jacArrayUnscaled[funIdx,varIdx] = jacArrayScaled[funIdx,varIdx] * self._unitFactors[funU] / self._unitFactors[varU]
                    else:
                        # no unscaling
                        jacArrayUnscaled[funIdx,varIdx] = jacArrayScaled[funIdx,varIdx]
         
        return jacArrayUnscaled

    def UnscaleJacobianByVars(self, jacArrayScaled, varUnits):
        """
        Unscale a Jacobian given scaled Jacobian array and variable units.
        Scaling by function units is done separately
        """
        # @param jacArrayScaled The Jacobian to dimensionalize
        # @param varUnits List of units of the data stored in variables (rows)
        # @return jacArrayUnscaled Unscaled version of the input jacArray
        nVar = len(varUnits)
        shapeJac = jacArrayScaled.shape
        nFun = shapeJac[0]
        if nVar != shapeJac[1]:
            # Validate that jacArray has as many columns as varUnits
            jacArrayUnscaled = jacArrayScaled
            print('ScaleUtility.UnscaleJacobianByVars: jacArrayUnscaled does not have the same number of columns as there are elements in varUnits.')
            print('Unscaling not performed.')
        else:
            jacArrayUnscaled = np.empty((nFun, nVar))
            for varIdx in range(nVar):
                varU = varUnits[varIdx]
                if self.ValidateUnit(varU):
                    jacArrayUnscaled[0:nFun,varIdx] = jacArrayScaled[0:nFun,varIdx] / self._unitFactors[varU]
                else:
                    # no scaling
                    jacArrayUnscaled[0:nFun,varIdx] = jacArrayScaled[0:nFun,varIdx]
                
        return jacArrayUnscaled

    def UnscaleJacobianByFun(self, jacArrayScaled, funUnits):
        """
        Unscale a Jacobian given scaled Jacobian array and function units.
        Unscaling by variable units is done separately
        """
        # @param jacArrayScaled The Jacobian to dimensionalize
        # @param funUnits List of units of the data stored in function (columns)
        # @return jacArrayUnscaled Unscaled version of the input jacArray
        nFun = len(funUnits)
        shapeJac = jacArrayScaled.shape
        nVar = shapeJac[1]
        if nFun != shapeJac[0]:
            # Validate that jacArray has as many rows as funUnits
            jacArrayUnscaled = jacArrayScaled
            print('ScaleUtility.UnscaleJacobianByFun: jacArrayScaled does not have the same number of rows as there are elements in funUnits.')
            print('Unscaling not performed.')
        else:
            jacArrayUnscaled = np.empty((nFun, nVar))
            for funIdx in range(nFun):
                funU = funUnits[funIdx]
                if self.ValidateUnit(funU):
                    jacArrayUnscaled[funIdx,0:nVar] = jacArrayScaled[funIdx,0:nVar] * self._unitFactors[funU]
                else:
                    # no scaling
                    jacArrayUnscaled[funIdx,0:nVar] = jacArrayScaled[funIdx,0:nVar]
                
        return jacArrayUnscaled