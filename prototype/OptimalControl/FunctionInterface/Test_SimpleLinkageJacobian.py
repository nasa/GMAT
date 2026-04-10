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
    #p2Config.OrbitStateRep = 'Cartesian'
    #p2Config.Origin = 'Earth'
    #p2Config.Axes = 'EclipJ2000'
    p2Config.OrbitStateRep = 'ClassicalKeplerian'
    p2Config.Origin = 'Sun'
    p2Config.Axes = 'J2000'
    p2Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    #p2Config.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"]
    p2Config.Units = ["DU", "NOSCALE", "ANGU", "ANGU", "ANGU", "ANGU", "MU"] # [SMA, ECC, INC, AOP, RAAN, TA, MASS]

    p2Scaler = ScaleUtility()
    p2Scaler.SetUnit("DU",DUSun)
    p2Scaler.SetUnit("TU",TUSun)
    p2Scaler.SetUnit("VU",VUSun)
    p2Scaler.SetUnit("MU",MU)
    p2Scaler.SetUnit("MFU",MFUSun)
    p2Scaler.AddUnitAndShift("ANGU",ANGU,0.0)
    p2Scaler.AddUnitAndShift("NOSCALE",1.0,0.0)

    #p2StateData = np.array([7100.0/DUEarth, 518.0277847504348/DUEarth, 1192.32890354405/DUEarth, 
    #                        0.1/VUEarth, 7.141270312179305/VUEarth, -2.006180033891897/VUEarth, 3000.0/MU])
    #p2StateData = np.array([7193.23682930212/DUEarth, 0.03783210817193441, np.deg2rad(12.76879567109692)/ANGU, 
    #                        np.deg2rad(317.1930261219809)/ANGU, np.deg2rad(306.1121483087803)/ANGU, np.deg2rad(97.38328827516361)/ANGU, 3000.0/MU])
    p2StateData = np.array([7193.23682930212/DUSun, 0.03783210817193441, np.deg2rad(12.76879567109692)/ANGU, 
                            np.deg2rad(317.1930261219809)/ANGU, np.deg2rad(306.1121483087803)/ANGU, np.deg2rad(97.38328827516361)/ANGU, 3000.0/MU])

    # phase type 3: cartesian, sun-centered, j2000
    p3Config = PhaseConfig()
    p3Config.OrbitStateRep = 'Cartesian'
    p3Config.Origin = 'Sun'
    p3Config.Axes = 'J2000'
    p3Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    p3Config.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"]

    p3Scaler = ScaleUtility()
    p3Scaler.SetUnit("DU",DUSun)
    p3Scaler.SetUnit("TU",TUSun)
    p3Scaler.SetUnit("VU",VUSun)
    p3Scaler.SetUnit("MU",MU)
    p3Scaler.SetUnit("MFU",MFUSun)

    p3StateData = np.array([-134478044.8581583/DUSun, 56783698.460654/DUSun, 24617365.35096143/DUSun, 
                            -12.83186955622664/VUSun, -17.5919042143133/VUSun, -9.813226457385817/VUSun, 3000.0/MU])

    # phase type 4: cartesian, sun-centered, ecliptic j2000
    p4Config = PhaseConfig()
    p4Config.OrbitStateRep = 'Cartesian'
    p4Config.Origin = 'Sun'
    p4Config.Axes = 'EclipJ2000'
    p4Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    p4Config.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"]

    p4Scaler = ScaleUtility()
    p4Scaler.SetUnit("DU",DUSun)
    p4Scaler.SetUnit("TU",TUSun)
    p4Scaler.SetUnit("VU",VUSun)
    p4Scaler.SetUnit("MU",MU)
    p4Scaler.SetUnit("MFU",MFUSun)

    p4StateData = np.array([-134478044.8581583/DUSun, 61890250.33143775/DUSun, -1266.950863603503/DUSun, 
                            -12.83186955622664/VUSun, -20.04373386496753/VUSun, -2.0058016202667/VUSun, 3000.0/MU])

    # phase type 5: classical keplerian, earth-centered, j2000
    p5Config = PhaseConfig()
    p5Config.OrbitStateRep = 'ClassicalKeplerian'
    p5Config.Origin = 'Earth'
    p5Config.Axes = 'J2000'
    p5Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    p5Config.Units = ["DU", "NOSCALE", "ANGU", "ANGU", "ANGU", "ANGU", "MU"] # [SMA, ECC, INC, AOP, RAAN, TA, MASS]
     
    p5Scaler = ScaleUtility()
    p5Scaler.AddUnitAndShift("ANGU",ANGU,0.0)
    p5Scaler.AddUnitAndShift("NOSCALE",1.0,0.0)
    p5Scaler.SetUnit("DU",DUEarth)
    p5Scaler.SetUnit("TU",TUEarth)
    p5Scaler.SetUnit("VU",VUEarth)
    p5Scaler.SetUnit("MU",MU)
    p5Scaler.SetUnit("MFU",MFUEarth)
     
    p5StateData = np.array([7193.23682930212/DUEarth, 0.03783210817193441, np.deg2rad(12.76879567109692)/ANGU, 
                            np.deg2rad(317.1930261219809)/ANGU, np.deg2rad(306.1121483087803)/ANGU, np.deg2rad(97.38328827516361)/ANGU, 3000.0/MU])

    # phase type 6: spherical az/fpa, earth-centered, j2000
    p6Config = PhaseConfig()
    p6Config.OrbitStateRep = 'SphericalAzFPA'
    p6Config.Origin = 'Earth'
    p6Config.Axes = 'J2000'
    p6Config.DecVecData = [['OrbitState', 6], ['TotalMass', 1]]
    p6Config.Units = ["DU", "ANGU", "ANGU", "VU", "ANGU", "ANGU", "MU"] # [RMAG, RA, DEC, VMAG, AZI, FPA, MASS]
    
    p6Scaler = ScaleUtility()
    p6Scaler.AddUnitAndShift("ANGU",ANGU,0.0)
    p6Scaler.SetUnit("DU",DUEarth)
    p6Scaler.SetUnit("TU",TUEarth)
    p6Scaler.SetUnit("VU",VUEarth)
    p6Scaler.SetUnit("MU",MU)
    p6Scaler.SetUnit("MFU",MFUEarth)
     
    p6StateData = np.array([7218.033042318384/DUEarth, np.deg2rad(0.008069828046999572)/ANGU, np.deg2rad(10.37584481937181)/ANGU, 
                            7.418389313051722/VUEarth, np.deg2rad(82.5168076695536)/ANGU, np.deg2rad(87.84087298474195)/ANGU, 3000.0/MU])

    # collect all the phase data
    pConfigs = [p1Config, p2Config, p3Config, p4Config, p5Config, p6Config]
    pScalers = [p1Scaler, p2Scaler, p3Scaler, p4Scaler, p5Scaler, p6Scaler]
    pStateDatas = [p1StateData, p2StateData, p3StateData, p4StateData, p5StateData, p6StateData]

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

    # collect all the function data
    fConfigs = [f1Config]
    fScalers = [f1Scaler]

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

    # Simulate CSALT populating the function with time data
    time1 = 30000.0
    time2 = 30000.0
    aLink.SetData("Time",0,time1)
    aLink.SetData("Time",1,time2)  

    # loop through cases to check
    print('F ID, P1 ID, P2 ID, F val')
    i = 0
    # loop through functions
    for fConfig in fConfigs:
        fScaler = fScalers[i]
        j = 0
        # loop through first phase types
        for p1Config in pConfigs:
            p1Scaler = pScalers[j]
            k = 0
            # loop through second phase types
            for p2Config in pConfigs:
                p2Scaler = pScalers[k]

                # define the phase configurations for the linkage constraint
                aLink.SetPhaseConfig(0,p1Config) # phase 1
                aLink.SetPhaseConfig(1,p2Config) # phase 2
                aLink.SetFunConfig(fConfig) # the function

                # define the scaling utilities for the linkage constraint
                aLink.SetPhaseScaler(0,p1Scaler) # phase 1
                aLink.SetPhaseScaler(1,p2Scaler) # phase 2
                aLink.SetFunScaler(fScaler) # the function
                aLink.SetFunBounds(funLB, funUB)

                # set the state data
                # Simulate CSALT populating the function with time data
                time1 = 30000.0
                time2 = 30000.0
                aLink.SetData("Time",0,time1)
                aLink.SetData("Time",1,time2)  

                stateData1 = pStateDatas[j]
                stateData2 = pStateDatas[k]
                aLink.SetData("State",0, stateData1)
                aLink.SetData("State",1, stateData2)

                # get the unscaled phase states for use in the function
                # (don't need to get this here to set the function value, this is just for debugging)
                stateUnscaled1 = aLink.GetUnscaledStateForFun(0)
                stateUnscaled2 = aLink.GetUnscaledStateForFun(1)

                # evaluate the function
                fValue = aLink.EvaluateFunction()
                print(i, j, k, fValue[0:4])
                print(i, j, k, fValue[4:])

                # evaluate the jacobian
                #jacValue = aLink.EvaluateAnalyticJacobian()

                # do a crude finite difference test on the derivatives

                # start with time if the origins of a phase and fun are different
                # or if the state reps are different
                print(aLink.funConfig.Origin, aLink.phaseConfigs[1].Origin, aLink.funConfig.OrbitStateRep, aLink.phaseConfigs[1].OrbitStateRep)
                #if aLink.funConfig.Origin != aLink.phaseConfigs[1].Origin:
                if aLink.funConfig.OrbitStateRep != aLink.phaseConfigs[1].OrbitStateRep:
                
                #if aLink.funConfig.Origin != aLink.phaseConfigs[1].Origin and aLink.funConfig.OrbitStateRep != aLink.phaseConfigs[1].OrbitStateRep:
                    StateJacAna, TimeJacAna = aLink.EvaluateAnalyticJacobian() # analytical jacobians
                    stateUnscaledNominal = aLink.GetUnscaledStateForFun(1) # nominal unscaled state

                    # perturb a time
                    dt = 1.0e-6 # perturbation in days
                    aLink.SetData("Time", 0, time1+dt) # set the new time
                    stateUnscaledPerturbedTime = aLink.GetUnscaledStateForFun(0) # perturbed unscaled state
                    fValuePerturbedTime = aLink.EvaluateFunction()
                    TimeJacFD = (fValuePerturbedTime - fValue) / (dt * 86400.0) # finite difference approximation;

                    # now perturb a state
                    aLink.SetData("Time", 0, time1) # reset the time
                    dLUnscaled = 0.5 # unscaled perturbation size
                    dL = aLink.phaseScalers[0].ScaleVector([dLUnscaled], ["DU"]) # scale the perturbation
                    dState = np.array([0.0, dL[0], 0.0, 0.0, 0.0, 0.0, 0.0]) # set the perturbation vector
                    aLink.SetData("State", 0, stateData1+dState) # set the perturbed state
                    stateUnscaledPerturbedState = aLink.GetUnscaledStateForFun(0) # perturbed unscaled state
                    fValuePerturbedState = aLink.EvaluateFunction()
                    StateJacFD = (fValuePerturbedState - fValue) / dL # finite difference approximation

                    print(TimeJacFD)
                    print('')
                    print(TimeJacAna[0])
                    print('')
                    print(StateJacFD)
                    print('')
                    print(StateJacAna[0,:,1])
                    #print(TimeJacAna-TimeJacFD[0:6])
                    print('')
                    #print(StateJacAna[0:6,0]-StateJacFD[0:6])
                    #print('')


                #if j==0 and k==1:
                #    print('****************************************************************')
                #    print(stateUnscaled1)
                #    print(stateUnscaled2)
                #    print('****************************************************************')
                #print('=====================================================================')
                k+=1
            j+=1
        i+=1

    print('Done!')


