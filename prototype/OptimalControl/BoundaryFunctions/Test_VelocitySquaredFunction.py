"""
Tester for changes being made to GMATFunction
Revision log:
2018-12-07; nhatten; Created
2018-12-11; nhatten; cleaning up
2018-12-13; nhatten; added setting function bounds
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from PhaseConfiguration import PhaseConfig
from ScaleUtil import ScaleUtility
from VelocitySquaredFunction import VelocitySquaredFunction

if __name__ == "__main__":
    # create and evaluate the following scenario:
    # a phase, whose state is in sun-centered, ecliptic j2000 frame, spherical Az/FPA coordinates
    # a constraint on the square of the velocity magnitude relative to earth-centered, j2000 frame

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

    pScaler = ScaleUtility() # the ScaleUtility object for the phase
    pScaler.SetUnit("DU",DUSun)
    pScaler.SetUnit("TU",TUSun)
    pScaler.SetUnit("VU",VUSun)
    pScaler.SetUnit("MU",MU)
    pScaler.SetUnit("MFU",MFUSun)
    pScaler.AddUnitAndShift("ANGU",ANGU,0.0)

    # configure the phase
    pConfig = PhaseConfig()
    pConfig.OrbitStateRep = 'SphericalAzFPA'
    pConfig.Origin = 'Sun'
    pConfig.Axes = 'EclipJ2000'
    pConfig.DecVecData = [['OrbitState', 6], ['TotalMass', 1]] # each inner list is a 'substate', described by its name and the number of elements
    pConfig.Units = ["DU", "ANGU", "ANGU", "VU", "ANGU", "ANGU", "MU"] # in the same order as DecVecData

    fScaler = ScaleUtility() # the ScaleUtility object for the function

    # set the values used for scaling in the ScaleUtility object
    fScaler.AddUnitAndShift("VUSquared",VUSquaredEarth,0.0)

    # configure the function
    fConfig = PhaseConfig()
    fConfig.Origin = 'Earth'
    fConfig.Axes = 'J2000'
    fConfig.Units = ["VUSquared"]
    fConfig.OrbitStateRep = 'Cartesian'

    # Create the GMATFunction object and set basic stuff
    vFun = VelocitySquaredFunction()
    vFun.SetNumPoints(1) # only one point
    vFun.SetNumFunctions(1) # only one function (i.e., a scalar)

    # define the phase configurations for the constraint
    vFun.SetPhaseConfig(0,pConfig) # phase 1
    vFun.SetFunConfig(fConfig) # the function

    # define the scaling utilities for the constraint
    vFun.SetPhaseScaler(0,pScaler) # phase 1
    vFun.SetFunScaler(fScaler) # the function

    # set bounds on the function
    funLB = np.array([7.5]) # unscaled
    funUB = np.array([20000.0])
    vFun.SetFunBounds(funLB, funUB)

    # add a custom setting
    vFun.SetCustomSetting('TargetVelocitySquared', 1000.5) # arguments are key and value

    # Simulate CSALT populating the function with data
    time = 30000.0
    vFun.SetData("Time",0,time)

    stateData = np.array([150.0e6/DUSun, 1.2/ANGU, -0.1/ANGU, 36.0/VUSun, 0.7/ANGU, 0.2/ANGU, 3000.0/MU]) # [RMAG, RA, DEC, VMAG, AZI, FPA] wrt sun
    vFun.SetData("State",0, stateData)

    # evaluate the function
    fValue = vFun.EvaluateFunction()
    print('vFun value: ', fValue)

    # extract bounds on the function
    funLBScaled, funUBScaled = vFun.GetFunBoundsScaled()
    print('vFun scaled lower bounds: ', funLBScaled)
    print('vFun scaled upper bounds: ', funUBScaled)

    print('Done!')
