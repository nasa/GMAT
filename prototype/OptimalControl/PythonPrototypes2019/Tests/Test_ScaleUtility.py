# -*- coding: utf-8 -*-
"""
Created on Wed Nov 14 15:31:08 2018

@author: sphughe1
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from ScaleUtil import ScaleUtility

#=====  Test Scaling and unscaling a vector
ScaleUtil = ScaleUtility();
mu = 398600.4415
DU = 6378.1363
TU = 2.0*np.pi*np.sqrt(DU**3/mu)
VU = DU/TU
ACCU = VU/TU
MU = 1000.0
MFU = MU/TU
ScaleUtil.SetUnitAndShift("DU",DU,2.0)
ScaleUtil.SetUnitAndShift("TU",TU,25685.000)
ScaleUtil.SetUnit("VU",VU)
ScaleUtil.SetUnit("MU",MU)
ScaleUtil.SetUnit("MFU",MFU)

stateVec = np.array([25689.000, 7000, 0, 1300, 0, 7.35, 1.3, 9000])
units = ['TU' , 'DU','DU','DU','VU','VU','VU','MU'] # in python, units needs to be a list even if it only has one element
print(stateVec)
ndState = ScaleUtil.ScaleVector(stateVec,units);
rdState = ScaleUtil.UnscaleVector(ndState,units);
print(rdState)
print((stateVec-rdState).max())

#=====  Test scaling and unscaling a Jabobian
jacArray = np.array([[7000, 9.5, 1500],[9000, -9.5, 1200]])
# fake custom function units here: 
ScaleUtil.AddUnitAndShift("F1U",12.345,0.0)
ScaleUtil.AddUnitAndShift("F2U",6.345,0.0)
ScaleUtil.AddUnitAndShift("F3U",123.9,0.0)
funUnits = ['F1U','F2U'] # in python, funUnits needs to be a list even if it only has one element
varUnits = ["DU","VU","MU"] # in python, varUnits needs to be a list even if it only has one element
print(jacArray)
ndJac = ScaleUtil.ScaleJacobian(jacArray,funUnits,varUnits)
rdJac = ScaleUtil.UnscaleJacobian(ndJac,funUnits,varUnits)
print(rdJac)
print((jacArray-rdJac).max())