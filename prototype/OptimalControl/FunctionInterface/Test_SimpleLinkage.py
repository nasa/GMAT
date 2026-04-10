# -*- coding: utf-8 -*-
"""
Created on Mon Nov 26 14:36:37 2018

2018-12-07; nhatten; revisions

@author: sphughe1
@author: nhatten
"""
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from ScaleUtil import ScaleUtility
import SimpleLinkage
from PhaseConfiguration import PhaseConfig

# Set up the phase configurations. This would be done by GMAT and provided to the function 

# phase 1
p1Config = PhaseConfig()
p1Config.OrbitStateRep = 'Cartesian'
p1Config.Origin = 'Sun'
p1Config.Axes = 'EclipticJ2000'
p1Config.DecVecData = ['SphericalAzFPA', 'TotalMass']
p1Config.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"] # is this true? SphericalAzFPA first 3 elements are DU, ANGLE, ANGLE

p1Scaler = ScaleUtility() # the ScaleUtility object for phase 1

# values used for scaling (sun-centered orbit)
mu1 = 1.32712440018e+11 # km^3/s^2
DU1 = 149597870.691 # km
TU1 = 2.0*np.pi*np.sqrt(DU1**3/mu1) # sec
VU1 = DU1/TU1 # km/s
AU1 = VU1/TU1 # km/s^2
MU1 = 1000.0 # kg
MFU1 = MU1/TU1 # kg/s

# set the values used for scaling in the ScaleUtility object
p1Scaler.SetUnit("DU",DU1)
p1Scaler.SetUnit("TU",TU1)
p1Scaler.SetUnit("VU",VU1)
p1Scaler.SetUnit("MU",MU1)
p1Scaler.SetUnit("MFU",MFU1)

# phase 2
p2Config = PhaseConfig()
p2Config.OrbitStateRep = 'Cartesian'
p2Config.Origin = 'Earth'
p2Config.Axes = 'J2000'
p2Config.DecVecData = ['Cartesian', 'TotalMass']
p2Config.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"]

p2Scaler = ScaleUtility() # the ScaleUtility object for phase 2

# values used for scaling (earth-centered orbit)
mu2 = 398600.4415 # km^3/s^2
DU2 = 6378.1363 # km
TU2 = 2.0*np.pi*np.sqrt(DU2**3/mu2) # sec
VU2 = DU2/TU2 # km/s
AU2 = VU2/TU2 # km/s^2
MU2 = 1000.0 # kg
MFU2 = MU2/TU2 # kg/s

# set the values used for scaling in the ScaleUtility object
p2Scaler.SetUnit("DU",DU2)
p2Scaler.SetUnit("TU",TU2)
p2Scaler.SetUnit("VU",VU2)
p2Scaler.SetUnit("MU",MU2)
p2Scaler.SetUnit("MFU",MFU2)

#phaseConfigs = [p1Config, p2Config]

# Simulate GMAT constructing the function and dimensionalize key data
funConfig = PhaseConfig()
funConfig.OrbitStateRep = 'Cartesian';
funConfig.Origin = 'Earth'
funConfig.Axes = 'J2000'
funConfig.Units = ["TU" "DU", "DU", "DU", "VU", "VU", "VU", "MU"]

fScaler = ScaleUtility() # the ScaleUtility object for the function

# values used for scaling (earth-centered orbit)
mu3 = 398600.4415 # km^3/s^2
DU3 = 6378.1363 # km
TU3 = 2.0*np.pi*np.sqrt(DU3**3/mu3) # sec
VU3 = DU3/TU3 # km/s
AU3 = VU3/TU3 # km/s^2
MU3 = 1000.0 # kg
MFU3 = MU3/TU3 # kg/s

# set the values used for scaling in the ScaleUtility object
fScaler.SetUnit("DU",DU3)
fScaler.SetUnit("TU",TU3)
fScaler.SetUnit("VU",VU3)
fScaler.SetUnit("MU",MU3)
fScaler.SetUnit("MFU",MFU3)

# Construct the scale utility and populate with scale values...
# This would be done by GMAT and provided to the function

Scaler = ScaleUtility() # the ScaleUtility object

# values used for scaling
mu = 398600.4415
DU = 6378.1363
TU = 2.0*np.pi*np.sqrt(DU**3/mu)
VU = DU/TU
AU = VU/TU
MU = 1000.0
MFU = MU/TU

# set the values used for scaling in the ScaleUtility object
Scaler.SetUnit("DU",DU)
Scaler.SetUnit("TU",TU)
Scaler.SetUnit("VU",VU)
Scaler.SetUnit("MU",MU)
Scaler.SetUnit("MFU",MFU)

# create the linkage constraint object
aLink = SimpleLinkage.LinkageConstraint() # the linkage object
aLink.SetScaleUtility(Scaler) # pet the ScaleUtility object
aLink.SetNumPoints(2)
aLink.SetNumFunctions(8)

# Define the dependencies
aLink.SetDependencyMap("Control",[0,0])
aLink.SetDependencyMap("Time",[1,1])
aLink.SetDependencyMap("Param",[0,0])
aLink.SetDependencyMap("State",[1,1])

# define the phase configurations for the linkage constraint
aLink.SetPhaseConfig(0,p1Config) # phase 1
aLink.SetPhaseConfig(1,p2Config) # phase 2
aLink.SetFunConfig(funConfig) # the function

# Define the decision vector contents (used in state conversions)
# this should now be constained in the phaseConfig objects that are set
aLink.SetDecVecData(0,['SphericalAzFPA', 'TotalMass'])
aLink.SetDecVecData(1,['Cartesian', 'TotalMass'])
aLink.SetFunctionUnits(["TU", "DU", "DU", "DU", "VU", "VU", "VU", "MU"])
aLink.SetDecVecUnits(0,["DU", "DU", "DU", "VU", "VU", "VU", "MU"])
aLink.SetDecVecUnits(1,["DU", "DU", "DU", "VU", "VU", "VU", "MU"])

# define the scaling utilities for the linkage constraint
aLink.SetPhaseScaler(0,p1Scaler) # phase 1
aLink.SetPhaseScaler(1,p2Scaler) # phase 2
aLink.SetFunScaler(fScaler) # the function

# Simulate CSALT populating the function with data
time1 = 0.0
time2 = 1.0
aLink.SetData("Time",0,time1)
aLink.SetData("Time",1,time2)  
stateData1 = np.array([7000.0/DU1, 0.0/DU1, 1300.0/DU1, 0.0/VU1, 7.35/VU1, 1.0/VU1, 3000.0/MU1])
stateData2 = np.array([7001.0/DU2, 1.0/DU2, 1301.0/DU2, 1.0/VU2, 8.35/VU2, 2.0/VU2, 3001.0/MU2])

aLink.SetData("State",0, stateData1)
aLink.SetData("State",1, stateData2)

# Simulate CSALT calling the function for evaluation.
fValue = aLink.EvaluateFunction(scale = 1)
print(fValue)
