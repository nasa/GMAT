"""
Tester for PatchedConicFlybyFunction
Revision log:
2018-12-04; nhatten; Created
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from PhaseConfiguration import PhaseConfig
from ScaleUtil import ScaleUtility
from PatchedConicFlybyFunction import PatchedConicFlybyFunction

if __name__ == "__main__":
    # create and evaluate the following scenario:
    # an Earth flyby
    # the states of the incoming and outgoing phases are known w.r.t. the Earth

    # values used for scaling that are common to all segments
    MU = 1000.0 # kg
    ANGU = 1.0 # rad

    # scaling for orbits about the sun
    muSun = 1.32712440018e+11 # km^3/s^2
    DUSun = 149597870.691 # km
    TUSun = 2.0*np.pi*np.sqrt(DUSun**3/muSun) # sec
    VUSun = DUSun/TUSun # km/s
    AUSun = VUSun/TUSun # km/s^2    
    MFUSun = MU/TUSun # kg/s    
    ANGVUSun = 1.0 / TUSun # rad/s

    # scaling for orbits about the earth
    muEarth = 398600.4415 # km^3/s^2
    DUEarth = 6378.1363 # km
    TUEarth = 2.0*np.pi*np.sqrt(DUEarth**3/muEarth) # sec
    VUEarth = DUEarth/TUEarth # km/s
    VUSquaredEarth = VUEarth * VUEarth # km^2/s^2
    AUEarth = VUEarth/TUEarth # km/s^2
    MFUEarth = MU/TUEarth # kg/s
    ANGVUEarth = 1.0 / TUEarth # rad/s

    p1Scaler = ScaleUtility() # the ScaleUtility object for the incoming phase
    p1Scaler.SetUnit("DU",DUEarth)
    p1Scaler.SetUnit("TU",TUEarth)
    p1Scaler.SetUnit("VU",VUEarth)
    p1Scaler.SetUnit("MU",MU)
    p1Scaler.SetUnit("MFU",MFUEarth)
    p1Scaler.AddUnitAndShift("ANGU",ANGU,0.0)

    # configure the incoming phase
    p1Config = PhaseConfig()
    p1Config.OrbitStateRep = 'SphericalAzFPA'
    p1Config.Origin = 'Earth'
    p1Config.Axes = 'J2000'
    p1Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]] # each inner list is a 'substate', described by its name and the number of elements
    p1Config.Units = ["DU", "ANGU", "ANGU", "VU", "ANGU", "ANGU", "MU"] # in the same order as DecVecData

    p2Scaler = ScaleUtility() # the ScaleUtility object for the outgoing phase (same as incoming)
    p2Scaler.SetUnit("DU",DUEarth)
    p2Scaler.SetUnit("TU",TUEarth)
    p2Scaler.SetUnit("VU",VUEarth)
    p2Scaler.SetUnit("MU",MU)
    p2Scaler.SetUnit("MFU",MFUEarth)
    p2Scaler.AddUnitAndShift("ANGU",ANGU,0.0)

    # configure the outgoing phase (same as incoming)
    p2Config = PhaseConfig()
    p2Config.OrbitStateRep = 'SphericalAzFPA'
    p2Config.Origin = 'Earth'
    p2Config.Axes = 'J2000'
    p2Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]] # each inner list is a 'substate', described by its name and the number of elements
    p2Config.Units = ["DU", "ANGU", "ANGU", "VU", "ANGU", "ANGU", "MU"] # in the same order as DecVecData

    fScaler = ScaleUtility() # the ScaleUtility object for the function
    fScaler.SetUnit("DU",DUSun)
    fScaler.SetUnit("MU",MU)
    fScaler.SetUnit("VU",VUEarth)
    fScaler.SetUnit("TU",TUEarth)

    # set the values used for scaling in the ScaleUtility object
    fScaler.AddUnitAndShift("VUSquared",VUSquaredEarth,0.0)

    # configure the function
    fConfig = PhaseConfig()
    fConfig.Origin = 'Sun'
    fConfig.Axes = 'EclipJ2000'
    fConfig.Units = ["DU", "DU", "DU", "DU", "DU", "DU", "VU", "MU", "TU", "DU"]
    fConfig.OrbitStateRep = 'Cartesian'

    # Create the GMATFunction object and set basic stuff
    flybyFun = PatchedConicFlybyFunction()
    flybyFun.SetNumPoints(2)
    flybyFun.SetNumFunctions(10)

    # define the phase configurations for the constraint
    flybyFun.SetPhaseConfig(0,p1Config) # incoming phase
    flybyFun.SetPhaseConfig(1,p2Config) # outgoing phase
    flybyFun.SetFunConfig(fConfig) # the function

    # define the scaling utilities for the constraint
    flybyFun.SetPhaseScaler(0,p1Scaler) # incoming phase
    flybyFun.SetPhaseScaler(1,p2Scaler) # outgoing phase
    flybyFun.SetFunScaler(fScaler) # the function

    # set bounds on the function
    funLB = np.array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 300.0]) # unscaled
    funUB = np.array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100000.0])
    flybyFun.SetFunBounds(funLB, funUB)

    # add custom settings
    flybyFun.SetCustomSetting('centralBody', 'Sun') # arguments are key and value
    flybyFun.SetCustomSetting('flybyBody', 'Earth') # arguments are key and value
    flybyFun.SetCustomSetting('flybyBodyMu', muEarth) # arguments are key and value

    # Simulate CSALT populating the function with data
    t1 = 30000.0
    t2 = 30001.0
    flybyFun.SetData("Time",0,t1)
    flybyFun.SetData("Time",1,t2)
    
    stateData1 = np.array([13000.0/DUEarth, 1.2/ANGU, -0.1/ANGU, 22.0/VUEarth, 0.7/ANGU, 0.2/ANGU, 3000.0/MU]) # [RMAG, RA, DEC, VMAG, AZI, FPA] wrt Earth
    stateData2 = np.array([13020.0/DUEarth, 1.24/ANGU, -0.01/ANGU, 22.5/VUEarth, 0.72/ANGU, 0.23/ANGU, 3005.0/MU]) # [RMAG, RA, DEC, VMAG, AZI, FPA] wrt Earth
    flybyFun.SetData("State",0, stateData1)
    flybyFun.SetData("State",1, stateData2)

    # evaluate the function
    fValue = flybyFun.EvaluateFunction()
    print('vFun value: ', fValue)

    # extract bounds on the function
    funLBScaled, funUBScaled = flybyFun.GetFunBoundsScaled()
    print('vFun scaled lower bounds: ', funLBScaled)
    print('vFun scaled upper bounds: ', funUBScaled)

    print('Done!')

