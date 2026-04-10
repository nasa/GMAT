"""
Tester for seeing if the simple linkage constraint jacobian performs as expected
Revision log:
2019-03-06; Noble Hatten; Created
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from PhaseConfiguration import PhaseConfig
from ScaleUtil import ScaleUtility
from SimpleLinkage import LinkageConstraint
from EphemSmoother import EphemSmoother

if __name__ == "__main__":
    # print-to-screen precision
    np.set_printoptions(precision=20)

    # set up the ephemeris spliner
    # paths for spice kernels
    de430Kernel = ['C:/Utilities/Universe/ephemeris_files/de430.bsp']
    tlsKernel = ['C:/Utilities/Universe/ephemeris_files/naif0012.tls']
    ephemSmoother = EphemSmoother(tlsKernel = tlsKernel, deKernel = de430Kernel, 
                                  defaultNRegions = 359, defaultH = 86400.0, defaultStepSizeType = 'h', defaultNRegionsPerRevolution = 359, defaultET0 = 0.0, defaultETf = 31557600.0)
    
    # shouldn't need to create the splines manually; should be done automatically when needed based on defaults

    # Set up the phase configurations. This would be done by GMAT and provided to the function 

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
    AUEarth = VUEarth/TUEarth # km/s^2
    MFUEarth = MU/TUEarth # kg/s
    ANGVUEarth = 1.0 / TUEarth # rad/s

    # create the different state representations.
    # the values for the states are taken from a GMAT run

    # phase type 1: cartesian, earth-centered, j2000
    p1Config = PhaseConfig()
    p1Config.OrbitStateRep = 'Cartesian'
    p1Config.Origin = 'Earth'
    p1Config.Axes = 'J2000'
    p1Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    p1Config.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"]

    p1Scaler = ScaleUtility()
    p1Scaler.SetUnit("DU",DUEarth)
    p1Scaler.SetUnit("TU",TUEarth)
    p1Scaler.SetUnit("VU",VUEarth)
    p1Scaler.SetUnit("MU",MU)
    p1Scaler.SetUnit("MFU",MFUEarth)

    p1StateData = np.array([7100.0/DUEarth, 1.0/DUEarth, 1300.0/DUEarth, 
                            0.1/VUEarth, 7.35/VUEarth, 1.0/VUEarth, 3000.0/MU])

    # phase type 2: cartesian, earth-centered, ecliptic j2000
    p2Config = PhaseConfig()
    #p2Config.Origin = 'Earth'
    p2Config.Origin = 'Sun'
    p2Config.OrbitStateRep = 'ClassicalKeplerian'
    p2Config.Axes = 'J2000'
    p2Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    p2Config.Units = ["DU", "NOSCALE", "ANGU", "ANGU", "ANGU", "ANGU", "MU"] # [SMA, ECC, INC, AOP, RAAN, TA, MASS]

    p2Scaler = ScaleUtility()
    #p2Scaler.SetUnit("DU",DUEarth)
    #p2Scaler.SetUnit("TU",TUEarth)
    #p2Scaler.SetUnit("VU",VUEarth)
    p2Scaler.SetUnit("DU",DUSun)
    p2Scaler.SetUnit("TU",TUSun)
    p2Scaler.SetUnit("VU",VUSun)
    p2Scaler.SetUnit("MU",MU)
    p2Scaler.SetUnit("MFU",MFUEarth)
    p2Scaler.AddUnitAndShift("ANGU",ANGU,0.0)
    p2Scaler.AddUnitAndShift("NOSCALE",1.0,0.0)

    #p2StateData = np.array([7193.23682930212/DUEarth, 0.03783210817193441, np.deg2rad(12.76879567109692)/ANGU, 
    #                        np.deg2rad(317.1930261219809)/ANGU, np.deg2rad(306.1121483087803)/ANGU, np.deg2rad(97.38328827516361)/ANGU, 3000.0/MU])
    p2StateData = np.array([7193.23682930212/DUSun, 0.03783210817193441, np.deg2rad(12.76879567109692)/ANGU, 
                        np.deg2rad(317.1930261219809)/ANGU, np.deg2rad(306.1121483087803)/ANGU, np.deg2rad(97.38328827516361)/ANGU, 3000.0/MU])


    # Simulate GMAT constructing the function and dimensionalize key data

    # fun type 1: cartesian, earth-centered, j2000 full-state linkage
    f1Config = PhaseConfig()
    f1Config.Origin = 'Earth'
    f1Config.Axes = 'J2000'
    f1Config.Units = ["TU", "DU", "DU", "DU", "VU", "VU", "VU", "MU"]
    f1Config.OrbitStateRep = 'Cartesian'

    f1Scaler = ScaleUtility()
    f1Scaler.SetUnit("DU",DUEarth)
    f1Scaler.SetUnit("TU",TUEarth)
    f1Scaler.SetUnit("VU",VUEarth)
    f1Scaler.SetUnit("MU",MU)
    f1Scaler.SetUnit("MFU",MFUEarth)

    # Create the GMATFunction object and set basic stuff
    aLink = LinkageConstraint()
    aLink.SetNumPoints(2)
    aLink.SetNumFunctions(8)
    funLB = np.zeros(8) # lower bounds are zero for simple linkage
    funUB = np.zeros(8) # upper bounds are also zero for simple linkage (equality constraint)
    # cannot actually set bounds, though (aLink.SetFunBounds()) until we call aLink.SetFunScaler()

    # set the EphemerisSmoother object
    aLink.SetEphemSmoother(ephemSmoother)
    aLink.SetEphemType('Spline')
    #aLink.SetEphemType('Spice')


    # define the phase configurations for the linkage constraint
    aLink.SetPhaseConfig(0,p1Config) # phase 1
    aLink.SetPhaseConfig(1,p2Config) # phase 2
    aLink.SetFunConfig(f1Config) # the function

    # define the scaling utilities for the linkage constraint
    aLink.SetPhaseScaler(0,p1Scaler) # phase 1
    aLink.SetPhaseScaler(1,p2Scaler) # phase 2
    aLink.SetFunScaler(f1Scaler) # the function
    aLink.SetFunBounds(funLB, funUB)

    # set the state data
    # Simulate CSALT populating the function with time data
    time1 = 30000.0
    time2 = 30000.0
    aLink.SetData("Time",0,time1)
    aLink.SetData("Time",1,time2)  

    aLink.SetData("State", 0, p1StateData)
    aLink.SetData("State", 1, p2StateData)

    # get the unscaled phase states for use in the function
    # (don't need to get this here to set the function value, this is just for debugging)
    stateUnscaled1 = aLink.GetUnscaledStateForFun(0)
    stateUnscaled2 = aLink.GetUnscaledStateForFun(1)

    # evaluate the function
    fValue = aLink.EvaluateFunction()

    # evaluate the jacobian
    StateJacAna, TimeJacAna = aLink.EvaluateAnalyticJacobian() # analytical jacobians

    # do a crude finite difference test on the derivatives             

    # perturb a time
    times = np.array([time1, time2])
    for i in range(2):
        dt = 1.0e-6 # perturbation in days
        aLink.SetData("Time", i, times[i]+dt, "overwrite") # set the new time
        stateUnscaledPerturbedTime = aLink.GetUnscaledStateForFun(i) # perturbed unscaled state
        fValuePerturbedTime = aLink.EvaluateFunction()
        TimeJacFD = (fValuePerturbedTime - fValue) / (dt * 86400.0) # finite difference approximation;
        print('Time')
        print(i)
        print(TimeJacFD)
        print(np.transpose(TimeJacAna[i]))
        print('')
        aLink.SetData("Time", i, times[i], "overwrite") # set the new time

    # now perturb a state
    states = np.array([p1StateData, p2StateData])
    for i in range(2):
        dLUnscaled = 0.5 # unscaled perturbation size
        dL = aLink.phaseScalers[i].ScaleVector([dLUnscaled], ["DU"]) # scale the perturbation
        dState = np.array([dL[0], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]) # set the perturbation vector
        aLink.SetData("State", i, states[i]+dState, "overwrite") # set the perturbed state
        stateUnscaledPerturbedState = aLink.GetUnscaledStateForFun(i) # perturbed unscaled state
        fValuePerturbedState = aLink.EvaluateFunction()
        StateJacFD = (fValuePerturbedState - fValue) / dL # finite difference approximation

        print('States')
        print(StateJacFD)
        print('')
        print(StateJacAna[i,:,0])
        print('')
        aLink.SetData("State", i, states[i], "overwrite") # set the new time

    print('Done!')



