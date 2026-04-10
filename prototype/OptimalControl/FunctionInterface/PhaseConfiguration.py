# -*- coding: utf-8 -*-
"""
Created on Tue Dec  4 10:39:54 2018

2018-12-07; nhatten; Added comments

@author: sphughe1
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian

class PhaseConfig(object):
    
    
    def __init__(self):
        """
        Constructor
        """

        # Create object variables but do not populate until set methods are called
        self.OrbitStateRep = '';
        self.Origin = ''
        self.Axes = ''
        self.DecVecData = []
        self.Units = []
        
    def SetOrbitState(self,orbitStateRep):
        """
        Set orbit state representation
        @param orbitStateRep The orbit state representation. Acceptable values: 'Cartesian'
        """
        self.OrbitStateRep = orbitStateRep
        return
        
    def SetOrigin(self,origin):
        """
        Set origin of coordinate system used to define state of phase.
        @param origin The origin. Acceptable values: Celestial body names understood by Spice (for now, at least). E.g.: 'Earth'
        """
        self.Origin = origin
        return
        
    def SetAxes(self,axes):
        """
        Set axes of coordinate system used to define state of phase.
        @param axes The axes. Acceptable values: Axis names understood by Spice (for now, at least). E.g.: 'J2000'
        """
        self.Axes = axes
        return
        
    def SetDecVecData(self,decVecData):
        """
        Set decision vector data type for phase.
        @param decVecData A list of the decision vector data type. Acceptable values: 'Cartesian', 'SpherialAzFPA', 'ClassicalKeplerian', 'TotalMass'
        """
        self.DecVecData = decVecData
        return
        
    def SetUnits(self,units):
        """
        Set units of orbit state.
        @param units A lsit of units of the orbit state. Acceptable values: Units understood by the user's scaleUtility object
        """
        self.Units = units
        return
        