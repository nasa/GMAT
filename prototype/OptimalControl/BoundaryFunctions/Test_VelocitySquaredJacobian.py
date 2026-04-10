"""
Test_VelocitySquaredJacobian.py

Revision log:
2019-03-08; Noble Hatten; Created
"""

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from PhaseConfiguration import PhaseConfig
from ScaleUtil import ScaleUtility
#from SimpleLinkage import LinkageConstraint
from EphemSmoother import EphemSmoother
from VelocitySquaredFunction import VelocitySquaredFunction

if __name__ == "__main__":
    # create and evaluate the following scenario:
    # a phase, whose state is in sun-centered, ecliptic j2000 frame, spherical Az/FPA coordinates
    # a constraint on the square of the velocity magnitude relative to earth-centered, j2000 frame



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
    VUSquaredEarth = VUEarth * VUEarth # km^2/s^2
    AUEarth = VUEarth/TUEarth # km/s^2
    MFUEarth = MU/TUEarth # kg/s
    ANGVUEarth = 1.0 / TUEarth # rad/s

    # phase: spherical Az/FPA, sun-centered, ecliptic j2000
    pConfig = PhaseConfig()
    pConfig.OrbitStateRep = 'SphericalAzFPA'
    #pConfig.OrbitStateRep = 'ClassicalKeplerian'
    #pConfig.OrbitStateRep = 'Cartesian'
    pConfig.Origin = 'Sun'
    pConfig.Axes = 'J2000' # was EclipJ2000, but we are not ready to handle axis changes yet
    pConfig.DecVecData = [['OrbitState', 6], ['TotalMass', 1]] # each inner list is a 'substate', described by its name and the number of elements
    pConfig.Units = ["DU", "ANGU", "ANGU", "VU", "ANGU", "ANGU", "MU"] # in the same order as DecVecData for SphericalAzFPA
    #pConfig.Units = ["DU", "NOSCALE", "ANGU", "ANGU", "ANGU", "ANGU", "MU"] # in the same order as DecVecData for ClassicalKeplerian
    #pConfig.Units = ["DU", "DU", "DU", "VU", "VU", "VU", "MU"] # in the same order as DecVecData for Cartesian

    # phase: scaling
    pScaler = ScaleUtility()
    pScaler.SetUnit("DU",DUSun)
    pScaler.SetUnit("TU",TUSun)
    pScaler.SetUnit("VU",VUSun)
    pScaler.AddUnitAndShift("ANGU",ANGU,0.0)
    pScaler.SetUnit("MU",MU)
    pScaler.SetUnit("MFU",MFUSun)
    #pScaler.AddUnitAndShift("VUSquaredEarth",VUSquaredEarth,0.0) # set the values used for scaling in the ScaleUtility object

    # function: scaling
    fScaler = ScaleUtility() # the ScaleUtility object for the function
    fScaler.AddUnitAndShift("VUSquared",VUSquaredEarth,0.0) # set the values used for scaling in the ScaleUtility object


    # function: configure; cartesian, earth-centered, j2000
    fConfig = PhaseConfig()
    fConfig.Origin = 'Earth'
    fConfig.Axes = 'J2000'
    fConfig.Units = ["VUSquared"]
    fConfig.OrbitStateRep = 'Cartesian'
    #fConfig.OrbitStateRep = 'SphericalAzFPA'

    # Create the GMATFunction object and set basic stuff
    vFun = VelocitySquaredFunction()
    vFun.SetNumPoints(1) # only one point
    vFun.SetNumFunctions(1) # only one function (i.e., a scalar)

    # define the phase configurations for the constraint
    vFun.SetPhaseConfig(0, pConfig) # phase 1
    vFun.SetFunConfig(fConfig) # the function

    # define the scaling utilities for the constraint
    vFun.SetPhaseScaler(0, pScaler) # phase 1
    vFun.SetFunScaler(fScaler) # the function

    # set bounds on the function
    funLB = np.array([7.5]) # unscaled
    funUB = np.array([20000.0]) # unscaled
    vFun.SetFunBounds(funLB, funUB)

    # add a custom setting
    vFun.SetCustomSetting('TargetVelocitySquared', 1000.5) # arguments are key and value

    # set the EphemerisSmoother object
    vFun.SetEphemSmoother(ephemSmoother)
    vFun.SetEphemType('Spline')

    # Simulate CSALT populating the function with data
    time = 30000.0 # MJDA1
    vFun.SetData("Time", 0, time)

    # scaled state data
    stateData = np.array([150.0e6/DUSun, 1.2/ANGU, -0.1/ANGU, 34.0/VUSun, 0.7/ANGU, 0.2/ANGU, 3000.0/MU]) # [RMAG, RA, DEC, VMAG, AZI, FPA] wrt sun
    #stateData = np.array([150.0e6/DUSun, 0.7, 0.1/ANGU, 1.3/ANGU, 0.7/ANGU, 0.2/ANGU, 3000.0/MU]) # keplerian wrt sun
    #stateData = np.array([150.0e6/DUSun, 20.0e6/DUSun, -5.0e4/DUSun, 3.0/VUSun, 33.0/VUSun, 0.2/VUSun, 3000.0/MU]) # Cart wrt sun
    vFun.SetData("State", 0, stateData)

    # evaluate the function and the state
    stateUnscaledNominal = vFun.GetUnscaledStateForFun(0)
    fValueNominal = vFun.EvaluateFunction() # this value is now being returned SCALED

    # evaluate the jacobian
    JacFuncVarsWrtPhaseVarsAna, JacFuncVarsWrtTimeAna = vFun.GetUnscaledJacobianOfFunVarsWrtDecVarsAndTime(0) # just the state transformation
    StateJacAna, TimeJacAna = vFun.EvaluateJacobian() # of the function; these values are now being returned SCALED

    # test jacobian against finite differences

    # perturb time
    dt = 1.0e-6 # perturbation in days
    vFun.SetData("Time", 0, time + dt)
    stateUnscaledPerturbedTime = vFun.GetUnscaledStateForFun(0) # perturbed unscaled state
    fValuePerturbedTime = vFun.EvaluateFunction() # SCALED
    TimeJacFD = (fValuePerturbedTime - fValueNominal) / (dt * 86400.0) # SCALED; dt does NOT need to be scaled because time is not currently being scaled; but does need to be in seconds because that's how the ddt's are calculated
    #JacFuncVarsWrtTimeFD = (stateUnscaledPerturbedTime - stateUnscaledNominal) / (dt * 86400.0) # multiply by 86400 to convert from 1/day to 1/sec; no longer used since we are scaling now

    # test the derivatives of just the state transformation from z^D to z^F
    # perturb state
    dx = np.zeros(7)
    StateJacFD = np.zeros(7)
    JacFuncVarsWrtPhaseVarsFD = np.zeros((7,7))
    vFun.SetData("Time", 0, time) # reset the time
    dxUnscaled = np.array([1.0, 1.0e-4, 1.0e-4, 1.0e-4, 1.0e-4, 1.0e-4, 1.0]) # spherical
    #dxUnscaled = np.array([1.0, 1.0e-4, 1.0e-8, 1.0e-4, 1.0e-4, 1.0e-4, 1.0]) # keplerian
    #dxUnscaled = np.array([1.0, 1.0, 1.0, 1.0e-3, 1.0e-3, 1.0e-3, 1.0]) # cartesian
    
    dx = vFun.phaseScalers[0].ScaleVector(dxUnscaled, vFun.phaseConfigs[0].Units) # scale the perturbation
    for i in range(7):
        dState = np.zeros(7)
        dState[i] = dx[i] # set the perturbation vector
        vFun.SetData("State", 0, stateData + dState) # set the perturbed state

        stateUnscaledPerturbedState = vFun.GetUnscaledStateForFun(0) # perturbed unscaled state
        fValuePerturbedState = vFun.EvaluateFunction() # SCALED
        #StateJacFD[i] = (fValuePerturbedState - fValueNominal) / dxUnscaled[i] # using dxUnscaled because the function is not currently being scaled
        StateJacFD[i] = (fValuePerturbedState - fValueNominal) / dx[i] # being scaled now, so use dx
        #JacFuncVarsWrtPhaseVarsFD[0:7, i] = (stateUnscaledPerturbedState - stateUnscaledNominal) / dxUnscaled[i] # finite difference approximation

    

    # test against automatic differentiation
    TimeJacAD_Fun = jacobian(vFun.EvaluateFunctionWithInputs, 0)
    StateJacAD_Fun = jacobian(vFun.EvaluateFunctionWithInputs, 1)
    TimeJacAD = TimeJacAD_Fun(time, stateData, 0, 0)
    StateJacAD = StateJacAD_Fun(time, stateData, 0, 0)
        
        
    print('done')