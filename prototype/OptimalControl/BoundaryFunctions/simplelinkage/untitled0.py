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
# Generic constriant functions
# Any coordinate system
# Multiple spacecraft

class LinkageConstraint(object):
    
    def __init__(self):
        
        #  Simulate the data being put on the object by CSALT
        time1 = 0
        time2 = 1
        control1 = [.2, .2, .2]
        contro12 = [.3, .3, .3]
        stateData1 = [7000, 0.0, 1300, 0, 7.35, 1, 3000]
        stateData2 = [7001, 1.0, 1301, 1, 1.35, 2, 2000, 500 , 501, 7000, 0.0, 1300, 0, 7.35, 1, 3000]
        deltaV = [.1, .2, .3]
        decVecMap1 = ['Cartesian', 'TotalMass']
        decVecMap2 = ['Cartesian', 'DryMass', 'TankMass_1', 'TankMass_2', 'Cartesian', 'TotalMass']
        
        
        self.stateData = []
        self.controlData = []
        self.timeData = []
        self.paramData = []
        self.decVecMap = [];
        
        self.stateData.append(stateData1);
        self.stateData.append(stateData2);
        self.controlData.append(control1);
        self.controlData.append(contro12);
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
        
        return funcValue
        
    

aLink = LinkageConstraint()

fValue = aLink.EvaluateFunction()