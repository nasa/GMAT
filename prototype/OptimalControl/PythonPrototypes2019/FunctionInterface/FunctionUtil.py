# -*- coding: utf-8 -*-
"""
Created on Mon Nov 19 16:28:37 2018

@author: sphughe1
"""

# Generic optimization parameters
    # State reps
    # Control reps
    # Static Vars
    # Time Reps
# Generic function independent variables    
# Generic constraint functions
# Any coordinate system
# Multiple spacecraft

#class DecVecUtil(object):
    
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import AlgebraicFunction  

class LinkageConstraint(GMATFunction.GMATFunction):
    
    def __init__(self):
        
        #  Simulate the data being put on the object by CSALT
        # or by user config.. lots to clean up here.
        time1 = 0
        time2 = 1
        control1 = np.array([.2, .2, .2])
        contro12 = np.array([.3, .3, .3])
        stateData1 = np.array([7000, 0.0, 1300, 0, 7.35, 1, 3000])
        stateData2 = np.array([7001, 1.0, 1301, 1, 8.35, 2, 3001])
        deltaV = np.array([.1, .2, .3])
        decVecMap1 = ['Cartesian', 'TotalMass']
        decVecMap2 = ['Cartesian', 'TotalMass']
        
        
        # Put the data on the object
        self.stateData = []
        self.controlData = []
        self.timeData = []
        self.paramData = []
        self.decVecMap = [];
        self.stateData.append(stateData1)
        self.stateData.append(stateData2)
        self.controlData.append(control1)
        self.controlData.append(contro12)
        self.timeData.append(time1)
        self.timeData.append(time2)
        self.paramData.append(deltaV)
        self.decVecMap.append(decVecMap1)
        self.decVecMap.append(decVecMap2)
        
    def GetState(self,phaseIdx):

        return self.stateData[phaseIdx]
    
    def GetTime(self,phaseIdx):

        return self.timeData[phaseIdx]
        
    def EvaluateFunction(self):
        
        funcValue = 1.0;
        state1 = self.GetState(0);
        state2 = self.GetState(1)
        t1 = self.GetTime(0);
        t2 = self.GetTime(1);
        
        funcValue = [t2-t1,state2-state1]
        
        return funcValue
        

aLink = LinkageConstraint()

fValue = aLink.EvaluateFunction()
print(fValue)