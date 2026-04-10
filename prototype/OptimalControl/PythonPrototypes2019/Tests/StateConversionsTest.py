# StateConversionsTest.py

# Module (not a class) for holding functions for coordinate transformations. This is the "test" version that has scrap functions and a main function. 

# Revision history:
# 2018-11-19; Noble Hatten; Created
# 2018-11-21; Noble Hatten; Tested Jacobians against AD

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from enum import Enum # requires python 3.4+

class stateTypes(Enum):
    Cartesian = 1
    ClassicalKeplerian = 2
    SphericalAzFPA = 4

eps = np.finfo(float).eps
circTol = 1.0e-10 # tolerance on eccentricity for a circular orbit
eqTol = 1.0e-10 # tolerance on inclination for an equatorial orbit (radians)
hypTol = 1.0e-10 # tolerance on hyperbolic orbit singularity when TA = acos(1/ECC)
fudgeTol = 1.0e-12

def acosSafe(x):
    """Takes the acos of the argument, making sure that the argument is between -1 and 1"""
    # @param x Argument of acos
    # @return acosx Acos(x)

    if x < -1.0:
        x = -1.0
    elif x > 1.0:
        x = 1.0

    acosx = np.arccos(x)

    return acosx

def StateConv(stateIn, inputType, outputType, mu = 398600.4415):
    """Converts spacecraft state from one type to another"""
    # @param stateIn Input state in inputType
    # @param inputType The type of the input state
    # @param outputType The type of the output state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return stateOut Output state in outputType
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    # If input type and output type are the same, return the input
    if inputType == outputType:
        stateOut = stateIn
        return stateOut

    # Convert to Cartesian state or not
    if inputType != stateTypes.Cartesian.value:
        cart = ToCartesian(stateIn, inputType, mu)
    else:
        cart = stateIn

    # Convert to state type requested
    if outputType == stateTypes.Cartesian.value:
        stateOut = cart
    else:
        stateOut = FromCartesian(cart, outputType, mu)
    return stateOut

def ToCartesian(stateIn, inputType, mu = 398600.4415):
    """Converts spacecraft state from inertial Cartesian to another representation"""
    # @param stateIn Input state in inputType
    # @param inputType The type of the input state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return cart Output state in inertial Cartesian
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    if inputType == stateTypes.Cartesian.value:
        cart = stateIn
    elif inputType == stateTypes.ClassicalKeplerian.value:
        cart = Kep2Cart(stateIn, mu)
    elif inputType == stateTypes.SphericalAzFPA.value:
        cart = SphAzFPA2Cart(stateIn)
    return cart

def FromCartesian(cart, outputType, mu = 398600.4415):
    """Converts spacecraft state from inertial Cartesian to one representation"""
    # @param stateIn Input state in inertial Cartesian
    # @param outputType The type of the output state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return state Output state in outputType
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # Classical Keplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    if outputType == stateTypes.Cartesian.value:
        state = cart
    elif outputType == stateTypes.ClassicalKeplerian.value:
        state = Cart2Kep(cart, mu)
    elif outputType == stateTypes.SphericalAzFPA.value:
        state = Cart2SphAzFPA(cart)
    return state

def Kep2Cart(elements, mu = 398600.4415):
    """Converts spacecraft state from classical Keplerian elements to inertial Cartesian state"""
    # @param elements Classical Keplerian elements: [SMA, ECC, INC, AOP, RAAN, TA]
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @ return cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ]

    # break input into components
    SMA = elements[0]
    ECC = elements[1]
    INC = elements[2]
    AOP = elements[3]
    RAAN = elements[4]
    TA = elements[5]

    # sanity check input
    if ECC < 0.0:
        cart = []
        print("Kep2Cart: ECC must be >= 0.0")
        return cart

    # signs on SMA and ECC are not consistent
    if ECC > 1.0 and SMA > 0.0:
        cart = []
        print("Kep2Cart: If ECC > 1.0, SMA must be < 0.0")
        return cart
    elif ECC < 1.0 and SMA < 0.0:
        cart = []
        print("Kep2Cart: If ECC < 1.0, SMA must be > 0.0")
        return cart
    elif abs(ECC - 1.0) < 2.0 * eps:
        cart = []
        print("Kep2Cart: Parabolic orbits cannot be entered in Keplerian or Modified Keplerian format")
        return cart
    
    if abs(ECC - 1.0) < 2.0 * eps:
        print("Kep2Cart: Orbit is nearly parabolic. State conversion routine is near numerical singularity.")

    # Orbit is hyperbolic and TA is higher/lower than limiting ValueError
    if (ECC > 1.0 and SMA < 0.0):
        limitVal = acosSafe(-1.0/ECC)
        if abs(TA) > limitVal:
            cart = []
            print("Kep2Cart: The TA value is not physically possible for a hyperbolic orbit")
            return cart

    # Equatorial and circular
    if abs(ECC) < circTol and abs(INC) < eqTol:
        LAMtrue = AOP + RAAN + TA
        LA = LAMtrue
        AOP = 0.0
        RAAN = 0.0
    elif abs(ECC) < circTol and abs(INC) >= eqTol: # circular inclined
        U = AOP + TA
        TA = U
        AOP = 0.0
    elif abs(INC) < eqTol and abs(ECC) >= circTol: # elliptic equatorial
        AOP = AOP + RAAN
        RAAN = 0.0

    if ECC == 1.0:
        # not supported yet!
        pass
    else:
        p = SMA * (1.0 - ECC * ECC)

    # calculate terms used more than once
    sinTA = np.sin(TA)
    cosTA = np.cos(TA)
    onePlusECCcosTA = 1.0 + ECC * cosTA
    sqrtMuP = np.sqrt(mu / p)
    if onePlusECCcosTA < hypTol and ECC > 1.0:
        print("Kep2Cart: Near singularity that occurs when TA = acos(1/ECC) for hyperbolic orbit")

    # position and velocity in perifocal system
    rv_p = np.array([p * cosTA / onePlusECCcosTA, p * sinTA / onePlusECCcosTA, 0.0])
    vv_p = np.array([-sqrtMuP * sinTA, sqrtMuP * (ECC + cosTA), 0.0])

    # rotate to inertial system
    Rot = np.dot(R3(-RAAN), np.dot(R1(-INC), R3(-AOP)))
    rv = np.dot(Rot, rv_p)
    vv = np.dot(Rot, vv_p)

    # concatenate
    #cart = np.concatenate((rv, vv)).tolist()
    #cart = np.concatenate((rv, vv))
    cart = np.array([rv[0], rv[1], rv[2], vv[0], vv[1], vv[2]])
    return cart

def Kep2CartScalars(SMA, ECC, INC, AOP, RAAN, TA, mu = 398600.4415):
    """Converts spacecraft state from classical Keplerian elements to inertial Cartesian state"""
    # @param elements Classical Keplerian elements: [SMA, ECC, INC, AOP, RAAN, TA]
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @ return cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ]

    # sanity check input
    if ECC < 0.0:
        cart = []
        print("Kep2Cart: ECC must be >= 0.0")
        return cart

    # signs on SMA and ECC are not consistent
    if ECC > 1.0 and SMA > 0.0:
        cart = []
        print("Kep2Cart: If ECC > 1.0, SMA must be < 0.0")
        return cart
    elif ECC < 1.0 and SMA < 0.0:
        cart = 0.0
        print("Kep2Cart: If ECC < 1.0, SMA must be > 0.0")
        return cart
    elif abs(ECC - 1.0) < 2.0 * eps:
        cart = []
        print("Kep2Cart: Parabolic orbits cannot be entered in Keplerian or Modified Keplerian format")
        return cart
    
    if abs(ECC - 1.0) < 2.0 * eps:
        print("Kep2Cart: Orbit is nearly parabolic. State conversion routine is near numerical singularity.")

    # Orbit is hyperbolic and TA is higher/lower than limiting ValueError
    if (ECC > 1.0 and SMA < 0.0):
        limitVal = acosSafe(-1.0/ECC)
        if abs(TA) > limitVal:
            cart = []
            print("Kep2Cart: The TA value is not physically possible for a hyperbolic orbit")
            return cart

    # Equatorial and circular
    if abs(ECC) < circTol and abs(INC) < eqTol:
        LAMtrue = AOP + RAAN + TA
        LA = LAMtrue
        AOP = 0.0
        RAAN = 0.0
    elif abs(ECC) < circTol and abs(INC) >= eqTol: # circular inclined
        U = AOP + TA
        TA = U
        AOP = 0.0
    elif abs(INC) < eqTol and abs(ECC) >= circTol: # elliptic equatorial
        AOP = AOP + RAAN
        RAAN = 0.0

    if ECC == 1.0:
        # not supported yet!
        pass
    else:
        p = SMA * (1.0 - ECC * ECC)

    # calculate terms used more than once
    sinTA = np.sin(TA)
    cosTA = np.cos(TA)
    onePlusECCcosTA = 1.0 + ECC * cosTA
    sqrtMuP = np.sqrt(mu / p)
    if onePlusECCcosTA < hypTol and ECC > 1.0:
        print("Kep2Cart: Near singularity that occurs when TA = acos(1/ECC) for hyperbolic orbit")

    # position and velocity in perifocal system
    rv_p = np.array([p * cosTA / onePlusECCcosTA, p * sinTA / onePlusECCcosTA, 0.0])
    vv_p = np.array([-sqrtMuP * sinTA, sqrtMuP * (ECC + cosTA), 0.0])

    # rotate to inertial system
    Rot = np.dot(R3(-RAAN), np.dot(R1(-INC), R3(-AOP)))
    rv = np.dot(Rot, rv_p)
    vv = np.dot(Rot, vv_p)

    # concatenate
    #cart = np.concatenate((rv, vv)).tolist()
    #cart = np.concatenate((rv, vv))
    cart = np.array([rv[0], rv[1], rv[2], vv[0], vv[1], vv[2]])
    return cart

def SphAzFPA2Cart(state):
    """Converts spacecraft state from SphericalAzFPA to inertial Cartesian state"""
    # @param state SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # @ return cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ]
    # All angles are in radians

    # unpack state
    RMAG = state[0]
    RA = state[1]
    DEC = state[2]
    VMAG = state[3]
    AZI = state[4]
    FPA = state[5]

    # common terms
    cosRA = np.cos(RA)
    sinRA = np.sin(RA)
    cosDEC = np.cos(DEC)
    sinDEC = np.sin(DEC)
    cosAZI = np.cos(AZI)
    sinAZI = np.sin(AZI)
    cosFPA = np.cos(FPA)
    sinFPA = np.sin(FPA)

    r = RMAG * np.array([cosDEC * cosRA, cosDEC * sinRA, sinDEC])
    v = np.empty(3)
    v[0] = cosFPA * cosDEC * cosRA - sinFPA * (sinAZI * sinRA + cosAZI * sinDEC * cosRA)
    v[1] = cosFPA * cosDEC * sinRA + sinFPA * (sinAZI * cosRA - cosAZI * sinDEC * sinRA)
    v[2] = cosFPA * sinDEC + sinFPA * cosAZI * cosDEC
    v = VMAG * v

    # concatenate
    cart = np.concatenate((r, v)).tolist()
    return cart

def SphAzFPA2CartScalars(RMAG, RA, DEC, VMAG, AZI, FPA):
    """Converts spacecraft state from SphericalAzFPA to inertial Cartesian state"""
    # @param state SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # @ return cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ]
    # All angles are in radians

    # common terms
    cosRA = np.cos(RA)
    sinRA = np.sin(RA)
    cosDEC = np.cos(DEC)
    sinDEC = np.sin(DEC)
    cosAZI = np.cos(AZI)
    sinAZI = np.sin(AZI)
    cosFPA = np.cos(FPA)
    sinFPA = np.sin(FPA)

    r = RMAG * np.array([cosDEC * cosRA, cosDEC * sinRA, sinDEC])
    v = np.array([cosFPA * cosDEC * cosRA - sinFPA * (sinAZI * sinRA + cosAZI * sinDEC * cosRA),
                  cosFPA * cosDEC * sinRA + sinFPA * (sinAZI * cosRA - cosAZI * sinDEC * sinRA),
                  cosFPA * sinDEC + sinFPA * cosAZI * cosDEC])
    v = VMAG * v

    # concatenate
    cart = np.array([r[0], r[1], r[2], v[0], v[1], v[2]])
    return cart

def Cart2Kep(cart, mu = 398600.4415):
    """Converts spacecraft state from inertial Cartesian state to classical Keplerian elements"""
    # @param cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ] 
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return elements Classical Keplerian elements: [SMA, ECC, INC, AOP, RAAN, TA]
    
    # constants
    twopi = 2.0 * np.pi
    kv = np.array([0.0, 0.0, 1.0]) # inertial z axis

    # unpack state
    rv = np.array(cart[0:3]) # position vector
    vv = np.array(cart[3:6]) # velocity vector
    r = np.linalg.norm(rv) # magnitude of position vector
    v = np.linalg.norm(vv) # magnitude of velocity vector

    hv = np.cross(rv, vv) # angular momentum vector
    h = np.linalg.norm(hv) # magnitude of hv

    nv = np.cross(kv, hv) # node vector; zero if equatorial orbit
    n = np.linalg.norm(nv)

    # calculate eccentricity vector and eccentricity
    ev = (v * v - mu / r) * rv / mu - np.dot(rv, vv) * vv / mu
    ECC = np.linalg.norm(ev)
    E = v * v * 0.5 - mu / r # orbit energy

    # check eccentricity for parabolic orbit
    if abs(ECC - 1.0) < 2.0 * eps:
        print("Cart2Kep: orbit is nearly parabolic via ECC and state conversion routine is near numerical singularity")

    # check energy for parabolic orbit
    if abs(E) < 2.0 * eps:
        print("Cart2Kep: orbit is nearly parabolic via Energy and state conversion routine is near numerical singularity")

    # determine SMA depending on orbit type
    if abs(ECC - 1.0) < 2.0 * eps:
        SMA = np.inf
        p = h * h / mu
    else:
        SMA = -mu / 2.0 / E
        p = SMA * (1.0 - ECC * ECC)

    # determine inclination
    INC = acosSafe(hv[2] / h)

    # determine if we have a special case

    # elliptic equatorial
    if INC <= eqTol and ECC > circTol:
        # determine RAAN
        RAAN = 0.0

        # determine AOP
        AOP = acosSafe(ev[0] / ECC)
        if ev[1] < 0.0:
            AOP = twopi - AOP

        # determine TA
        TA = acosSafe(np.dot(ev, rv) / ECC / r)
        if np.dot(rv, vv) < 0.0:
            TA = twopi - TA

    # circular inclined
    elif INC >= eqTol and ECC <= circTol:
        # determine RAAN
        RAAN = acosSafe(nv[0] / n)
        if nv[1] < 0.0:
            RAAN = twopi - RAAN

        # determine AOP
        AOP = 0.0

        # determine TA
        TA = acosSafe(np.dot(nv/n, rv/r))
        if rv[2] < 0.0:
            TA = twopi - TA

    # circular equatorial
    elif INC <= eqTol and ECC <= circTol:
        RAAN = 0.0
        AOP = 0.0
        TA = acosSafe(rv[0]/r)
        if rv[1] < 0.0:
            TA = twopi - TA

    # not a special case
    else:
        # determine RAAN
        RAAN = acosSafe(nv[0]/n)
        if nv[1] < 0.0:
            RAAN = twopi - RAAN

        # determine AOP
        AOP = acosSafe(np.dot(nv/n, ev/ECC))
        if ev[2] < 0.0:
            AOP = twopi - AOP

        # determine TA
        TA = acosSafe(np.dot(ev/ECC, rv/r))
        if np.dot(rv, vv) < 0.0:
            TA = twopi - TA

    # pack up
    elements = [SMA, ECC, INC, AOP, RAAN, TA]

    return elements

def Cart2KepScalars(rx, ry, rz, vx, vy, vz, mu = 398600.4415):
    """Converts spacecraft state from inertial Cartesian state to classical Keplerian elements"""
    # @param cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ] 
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return elements Classical Keplerian elements: [SMA, ECC, INC, AOP, RAAN, TA]
    
    # constants
    twopi = 2.0 * np.pi
    kv = np.array([0.0, 0.0, 1.0]) # inertial z axis

    # unpack state
    rv = np.array([rx, ry, rz]) # position vector
    vv = np.array([vx, vy, vz]) # velocity vector
    r = np.linalg.norm(rv) # magnitude of position vector
    v = np.linalg.norm(vv) # magnitude of velocity vector

    hv = np.cross(rv, vv) # angular momentum vector
    h = np.linalg.norm(hv) # magnitude of hv

    nv = np.cross(kv, hv) # node vector; zero if equatorial orbit
    n = np.linalg.norm(nv)

    # calculate eccentricity vector and eccentricity
    ev = (v * v - mu / r) * rv / mu - np.dot(rv, vv) * vv / mu
    ECC = np.linalg.norm(ev)
    E = v * v * 0.5 - mu / r # orbit energy

    # check eccentricity for parabolic orbit
    if abs(ECC - 1.0) < 2.0 * eps:
        print("Cart2Kep: orbit is nearly parabolic via ECC and state conversion routine is near numerical singularity")

    # check energy for parabolic orbit
    if abs(E) < 2.0 * eps:
        print("Cart2Kep: orbit is nearly parabolic via Energy and state conversion routine is near numerical singularity")

    # determine SMA depending on orbit type
    if abs(ECC - 1.0) < 2.0 * eps:
        SMA = np.inf
        p = h * h / mu
    else:
        SMA = -mu / 2.0 / E
        p = SMA * (1.0 - ECC * ECC)

    # determine inclination
    INC = acosSafe(hv[2] / h)

    # determine if we have a special case

    # elliptic equatorial
    if INC <= eqTol and ECC > circTol:
        # determine RAAN
        RAAN = 0.0

        # determine AOP
        AOP = acosSafe(ev[0] / ECC)
        if ev[1] < 0.0:
            AOP = twopi - AOP

        # determine TA
        TA = acosSafe(np.dot(ev, rv) / ECC / r)
        if np.dot(rv, vv) < 0.0:
            TA = twopi - TA

    # circular inclined
    elif INC >= eqTol and ECC <= circTol:
        # determine RAAN
        RAAN = acosSafe(nv[0] / n)
        if nv[1] < 0.0:
            RAAN = twopi - RAAN

        # determine AOP
        AOP = 0.0

        # determine TA
        TA = acosSafe(np.dot(nv/n, rv/r))
        if rv[2] < 0.0:
            TA = twopi - TA

    # circular equatorial
    elif INC <= eqTol and ECC <= circTol:
        RAAN = 0.0
        AOP = 0.0
        TA = acosSafe(rv[0]/r)
        if rv[1] < 0.0:
            TA = twopi - TA

    # not a special case
    else:
        # determine RAAN
        RAAN = acosSafe(nv[0]/n)
        if nv[1] < 0.0:
            RAAN = twopi - RAAN

        # determine AOP
        AOP = acosSafe(np.dot(nv/n, ev/ECC))
        if ev[2] < 0.0:
            AOP = twopi - AOP

        # determine TA
        TA = acosSafe(np.dot(ev/ECC, rv/r))
        if np.dot(rv, vv) < 0.0:
            TA = twopi - TA

    # pack up
    elements = np.array([SMA, ECC, INC, AOP, RAAN, TA])

    return elements

def Cart2SphAzFPA(cart):
    """Converts spacecraft state from inertial Cartesian state to SphericalAzFPA state"""
    # @param cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ] 
    # @return state SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    cartArray = np.array(cart)
    RMAG = np.linalg.norm(cart[0:3])
    VMAG = np.linalg.norm(cart[3:6])
    RA = np.arctan2(cart[1], cart[0])
    DEC = np.arcsin(cart[2] / RMAG)
    FPA = acosSafe(np.dot(cartArray[0:3], cartArray[3:6]) / RMAG / VMAG)
    cosDEC = np.cos(DEC)
    sinDEC = np.sin(DEC)
    cosRA = np.cos(RA)
    sinRA = np.sin(RA)

    xHat = [cosDEC * cosRA, cosDEC * sinRA, sinDEC]
    yHat = [np.cos(RA + np.pi * 0.5), np.sin(RA + np.pi * 0.5), 0.0]
    zHat = [-sinDEC * cosRA, -sinDEC * sinRA, cosDEC]

    R = np.array([xHat, yHat, zHat])
    newV = np.dot(R, cartArray[3:6])
    AZI = np.arctan2(newV[1], newV[2])

    state = [RMAG, RA, DEC, VMAG, AZI, FPA]
    
    return state

def Cart2SphAzFPAScalars(rx, ry, rz, vx, vy, vz):
    """Converts spacecraft state from inertial Cartesian state to SphericalAzFPA state"""
    # @param cart Inertial Cartesian state: [X, Y, Z, VX, VY, VZ] 
    # @return state SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    cartArray = np.array([rx, ry, rz, vx, vy, vz])
    RMAG = np.linalg.norm(cartArray[0:3])
    VMAG = np.linalg.norm(cartArray[3:6])
    RA = np.arctan2(cartArray[1], cartArray[0])
    DEC = np.arcsin(cartArray[2] / RMAG)
    FPA = acosSafe(np.dot(cartArray[0:3], cartArray[3:6]) / RMAG / VMAG)
    cosDEC = np.cos(DEC)
    sinDEC = np.sin(DEC)
    cosRA = np.cos(RA)
    sinRA = np.sin(RA)

    xHat = [cosDEC * cosRA, cosDEC * sinRA, sinDEC]
    yHat = [np.cos(RA + np.pi * 0.5), np.sin(RA + np.pi * 0.5), 0.0]
    zHat = [-sinDEC * cosRA, -sinDEC * sinRA, cosDEC]

    R = np.array([xHat, yHat, zHat])
    newV = np.dot(R, cartArray[3:6])
    AZI = np.arctan2(newV[1], newV[2])

    state = np.array([RMAG, RA, DEC, VMAG, AZI, FPA])
    
    return state

def dCartdOE(state, inputType, mu = 398600.4415):
    """Returns Jacobian of Cartesian state w.r.t. classical Keplerian orbital elements."""
    # @param state Value of state at which to calculate Jacobian
    # @param type of input state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return J Jacobian, organized in the order of the state types below
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    # convert state to elements if necessary
    if inputType != stateTypes.ClassicalKeplerian.value:
        oe = StateConv(state, inputType, stateTypes.ClassicalKeplerian.value, mu)
    else:
        oe = state

    # extract elements
    SMA = oe[0]
    ECC = oe[1]
    INC = oe[2]
    AOP = oe[3]
    RAAN = oe[4]
    TA = oe[5]

    # sanity check input
    if ECC < 0.0:
        J = []
        print("dCartdOE: ECC must be >= 0.0")
        return J

    # signs on SMA and ECC are not consistent
    if ECC > 1.0 and SMA > 0.0:
        J = []
        print("dCartdOE: If ECC > 1.0, SMA must be < 0.0")
        return J
    elif ECC < 1.0 and SMA < 0.0:
        J = []
        print("dCartdOE: If ECC < 1.0, SMA must be > 0.0")
        return J
    elif abs(ECC - 1.0) < 2.0 * eps:
        J = []
        print("dCartdOE: Parabolic orbits cannot be entered in Keplerian or Modified Keplerian format")
        return J
    
    if abs(ECC - 1.0) < 2.0 * eps:
        print("dCartdOE: Orbit is nearly parabolic. State conversion routine is near numerical singularity.")

    # Orbit is hyperbolic and TA is higher/lower than limiting ValueError
    if (ECC > 1.0 and SMA < 0.0):
        limitVal = acosSafe(-1.0/ECC)
        if abs(TA) > limitVal:
            J = []
            print("dCartdOE: The TA value is not physically possible for a hyperbolic orbit")
            return J

    # calculate useful quantities

    # Equatorial and circular
    if abs(ECC) < circTol and abs(INC) < eqTol:
        LAMtrue = AOP + RAAN + TA
        LA = LAMtrue
        AOP = 0.0
        RAAN = 0.0
    elif abs(ECC) < circTol and abs(INC) >= eqTol: # circular inclined
        U = AOP + TA
        TA = U
        AOP = 0.0
    elif abs(INC) < eqTol and abs(ECC) >= circTol: # elliptic equatorial
        AOP = AOP + RAAN
        RAAN = 0.0

    if ECC == 1.0:
        # not supported yet!
        oneMinusECC2 = 1.0 - ECC * ECC
    else:
        oneMinusECC2 = 1.0 - ECC * ECC
        p = SMA * oneMinusECC2

    # calculate useful quantities
    cosTA = np.cos(TA)
    sinTA = np.sin(TA)
    eta = np.sqrt(oneMinusECC2)
    r = p / (1.0 + ECC * cosTA)
    rpHat = [cosTA, sinTA, 0.0]
    rp = r * np.array(rpHat)
    vp = np.array([-(mu / SMA / oneMinusECC2)**0.5 * sinTA, (mu / SMA / oneMinusECC2)**0.5 * (ECC + cosTA), 0.0])
    Rt = np.dot(R3(-RAAN), np.dot(R1(-INC), R3(-AOP)))

    # calculate derivatives of position
    temp3 = (r / SMA) * np.array(rpHat)
    drvdSMA = np.dot(Rt, temp3)

    drvdECC = (-2.0 * r * ECC / eta**2 - SMA * oneMinusECC2 / (1.0 + ECC * cosTA)**2 * cosTA) * np.dot(Rt, rpHat)

    drvdINC = np.dot(-R3(-RAAN), np.dot(dR1(-INC), np.dot(R3(-AOP), rp)))

    drvdAOP = np.dot(-R3(-RAAN), np.dot(R1(-INC), np.dot(dR3(-AOP), rp)))

    drvdRAAN = np.dot(-dR3(-RAAN), np.dot(R1(-INC), np.dot(R3(-AOP), rp)))
    
    temp3 = np.array([SMA * (-oneMinusECC2) * sinTA / (1.0 + ECC * cosTA)**2,
             oneMinusECC2 * (sinTA**2 * ECC + cosTA + ECC * cosTA**2) * SMA / (1.0 + ECC * cosTA)**2,
             0.0])
    drvdTA = np.dot(Rt, temp3)

    # calculate derivatives of velocity
    temp3 = np.array([1.0 / 2.0 / (mu / SMA / oneMinusECC2)**0.5 * sinTA * mu / SMA**2 / oneMinusECC2,
             -1.0 / 2.0 / (mu / SMA / oneMinusECC2)**0.5 * (ECC + cosTA) * mu / SMA**2 / oneMinusECC2,
             0.0])
    dvvdSMA = np.dot(Rt, temp3)

    temp3 = np.array([-1.0 / (mu / SMA / oneMinusECC2)**0.5 * sinTA * mu / SMA / oneMinusECC2**2 * ECC,
             1.0 / (mu / SMA / oneMinusECC2)**0.5 * (ECC + cosTA) * mu / SMA / oneMinusECC2**2 * ECC + (mu / SMA / oneMinusECC2)**0.5,
             0.0])
    dvvdECC = np.dot(Rt, temp3)

    dvvdINC = np.dot(-R3(-RAAN), np.dot(dR1(-INC), np.dot(R3(-AOP), vp)))

    dvvdAOP = np.dot(-R3(-RAAN), np.dot(R1(-INC), np.dot(dR3(-AOP), vp)))

    dvvdRAAN = np.dot(-dR3(-RAAN), np.dot(R1(-INC), np.dot(R3(-AOP), vp)))

    temp3 = [-(mu / SMA / oneMinusECC2)**0.5 * cosTA,
             -(mu / SMA / oneMinusECC2)**0.5 * sinTA,
             0.0]
    dvvdTA = np.dot(Rt, temp3)

    dTAdEA = SMA * np.sqrt(oneMinusECC2) / r
    EA = TA2EA(TA, ECC)
    fac = 1.0 # dTAdEA * dEAdMA
    J = np.empty((6,6))
    J[0:3,:] = np.array([drvdSMA, drvdECC, drvdINC, drvdAOP, drvdRAAN, drvdTA*fac]).transpose()
    J[3:6,:] = np.array([dvvdSMA, dvvdECC, dvvdINC, dvvdAOP, dvvdRAAN, dvvdTA*fac]).transpose()
    return J

def dOEdCart(state, inputType, mu = 398600.4415):
    """Returns Jacobian of classical Keplerian orbital elements w.r.t. Cartesian state"""
    # @param state Value of state at which to calculate Jacobian
    # @param type of input state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return J Jacobian, organized in the order of the state types below
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    # convert state to position and velocity vectors if necessary
    if inputType != stateTypes.Cartesian.value:
        cart = StateConv(state, inputType, stateTypes.Cartesian.value, mu)
        rv = np.array(cart[0:3])
        vv = np.array(cart[3:6])
    else:
        rv = np.array(state[0:3])
        vv = np.array(state[3:6])

    # basic orbit properties
    r = np.linalg.norm(rv)
    v = np.linalg.norm(vv)
    hv = np.cross(rv, vv)
    h = np.linalg.norm(hv)
    hvz = hv[2]

    # useful unit vectors in calculating RAAN and AOP
    zhat = np.array([0.0, 0.0, 1.0])
    xhat = np.array([1.0, 0.0, 0.0])

    # the node vector
    nv = np.cross(zhat, hv)
    n = np.linalg.norm(nv)
    nvx = nv[0]

    # the eccentricity vector, energy, and SMA
    ev = ((v**2 - mu / r) * rv - np.dot(rv, vv) * vv) / mu
    ECC = np.linalg.norm(ev)
    E = v**2 * 0.5 - mu / r
    
    # check eccentricity for parabolic orbit
    if abs(ECC - 1.0) < 2.0 * eps:
        print("dOEdCart: orbit is nearly parabolic via ECC and state conversion routine is near numerical singularity")

    # check energy for parabolic orbit
    if abs(E) < 2.0 * eps:
        print("dOEdCart: orbit is nearly parabolic via Energy and state conversion routine is near numerical singularity")


    # determine SMA depending on orbit type
    if abs(ECC - 1.0) < 2.0 * eps:
        SMA = np.inf
    else:
        SMA = -mu / 2.0 / E

    # derivatives of intermediate quantities

    # derivatives of eccentricity vector and eccentricity unit vector
    devdrv = ((v**2 - mu / r) * np.identity(3) + mu * np.outer(rv, rv) / r**3 - np.outer(vv, vv)) / mu
    devdvv = 1.0 / mu * (2.0 * np.outer(rv, vv) - np.dot(rv, vv) * np.identity(3) - np.outer(vv, rv))
    dehatdrv = devdrv / ECC - np.dot(np.outer(ev, ev), devdrv) / ECC**3
    dehatdvv = devdvv / ECC - np.dot(np.outer(ev, ev), devdvv) / ECC**3

    # derivatives of the node vector and node unit vector
    dndrv = (-1.0 / n) * np.dot(nv, np.dot(skew(zhat), skew(vv)))
    dnvdrv = np.dot(-skew(zhat), skew(vv))
    dndvv = (1.0 / n) * np.dot(nv, np.dot(skew(zhat), skew(rv)))
    dnvdvv = np.dot(skew(zhat), skew(rv))
    dnhatdrv = dnvdrv / n - np.outer(nv, dndrv) / n**2
    dnhatdvv = dnvdvv / n - np.outer(nv, dndvv) / n**2

    # derivatives of orbital elements

    # SMA
    dSMAdrv = 2.0 * (SMA / r)**2 / r * rv
    dSMAdvv = 2.0 * SMA**2 / mu * vv

    # ECC
    temp1 = (v**2 - mu / r) *np.identity(3)
    temp2 = (mu / r**3) * np.outer(rv, rv)
    dECCdrv = (1.0 / ECC / mu) * np.dot(ev, temp1 + temp2 - np.outer(vv, vv))
    dECCdvv = (1.0 / ECC / mu) * np.dot(ev, 2.0 * np.outer(rv, vv) - np.dot(np.dot(rv, vv), np.identity(3)) - np.outer(vv, rv))

    # INC
    dINCdrv = (1.0 / np.sqrt(1.0 - (hvz / h)**2)) * np.dot(np.dot(zhat, (1.0/h) * np.identity(3) - (1.0/h**3) * np.outer(hv, hv)), skew(vv))
    dINCdvv = (-1.0 / np.sqrt(1.0 - (hvz / h)**2)) * np.dot(np.dot(zhat, (1.0/h) * np.identity(3) - (1.0/h**3) * np.outer(hv, hv)), skew(rv))

    # RAAN
    temp1 = (nvx / n**3) * nv
    temp2 = (1.0 / n) * xhat
    dRAANdrv = (-1.0 / np.sqrt(1.0 - (nvx / n)**2)) * np.dot(temp1 - temp2, np.dot(skew(zhat), skew(vv)))
    dRAANdvv = (1.0 / np.sqrt(1.0 - (nvx / n)**2)) * np.dot(temp1 - temp2, np.dot(skew(zhat), skew(rv)))

    if nv[1] < 0.0:
        dRAANdrv = -dRAANdrv
        dRAANdvv = -dRAANdvv

    # AOP
    temp1 = (1.0 / n) *np.dot(nv, dehatdrv)
    temp2 = (1.0 / ECC) * np.dot(ev, dnhatdrv)
    dAOPdrv = (-1.0 / np.sqrt(1.0 - (np.dot(ev, nv) / ECC / n)**2)) * (temp1 + temp2)
    temp1 = (1.0 / n) *np.dot(nv, dehatdvv)
    temp2 = (1.0 / ECC) * np.dot(ev, dnhatdvv)
    dAOPdvv = (-1.0 / np.sqrt(1.0 - (np.dot(ev, nv) / ECC / n)**2)) * (temp1 + temp2)

    if ev[2] < 0.0:
        dAOPdrv = -dAOPdrv
        dAOPdvv = -dAOPdvv

    # TA
    temp1 = (1.0 / r) * np.dot(rv, dehatdrv)
    temp2 = (1.0 / ECC) * ev
    temp3 = (1.0 / r) * np.identity(3) - (1.0 / r**3) * np.outer(rv, rv)
    dTAdrv = (-1.0 / np.sqrt(1.0 - (np.dot(ev, rv) / ECC / r)**2)) * (temp1 + np.dot(temp2, temp3))
    temp1 = (1.0 / r) * np.dot(rv, dehatdvv)
    dTAdvv = (-1.0 / np.sqrt(1.0 - (np.dot(ev, rv) / ECC / r)**2)) * temp1

    if np.dot(rv, vv) < 0.0:
        dTAdrv = -dTAdrv
        dTAdvv = -dTAdvv

    # assemble the jacobian
    J = np.empty((6,6))
    J[:,0] = np.concatenate((dSMAdrv, dSMAdvv))
    J[:,1] = np.concatenate((dECCdrv, dECCdvv))
    J[:,2] = np.concatenate((dINCdrv, dINCdvv))
    J[:,3] = np.concatenate((dAOPdrv, dAOPdvv))
    J[:,4] = np.concatenate((dRAANdrv, dRAANdvv))
    J[:,5] = np.concatenate((dTAdrv, dTAdvv))
    J = J.transpose()

    return J

#def dOEdCartFudge(state, inputType, mu = 398600.4415):
#    """Returns Jacobian of classical Keplerian orbital elements w.r.t. Cartesian state"""
#    # @param state Value of state at which to calculate Jacobian
#    # @param type of input state
#    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
#    # @return J Jacobian, organized in the order of the state types below
#    #
#    # Definitions of Types:
#    # Cartesian: [X, Y, Z, VX, VY, VZ]
#    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
#    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
#    # All angles are in radians

#    ## first, convert to Cartesian if necessary, then fudge values, then compute
#    #if inputType != stateTypes.ClassicalKeplerian.value:
#    #    kep = StateConv(state, inputType, stateTypes.ClassicalKeplerian.value, mu)
#    #else:
#    #    kep = state

#    ## fudge values
#    #print('starting kep: ', kep)
#    #if kep[2] < fudgeTol:
#    #    # equatorial
#    #    # make it not equatorial
#    #    kep[2] = fudgeTol + 2.0 * eps
#    #    print('increased e')

#    #if kep[1] < fudgeTol:
#    #    # circular
#    #    kep[1] = fudgeTol + 2.0 * eps
#    #    print('increased i')

#    #if kep[4] < fudgeTol:
#    #    # raan is near 0
#    #    kep[4] = fudgeTol + 2.0 * eps
#    #    print('increased raan because near 0')
#    #elif np.abs(kep[4] - np.pi) < fudgeTol:
#    #    # raan is near 180 deg
#    #    if kep[4] >= np.pi:
#    #        kep[4] = np.pi + fudgeTol + 2.0 * eps
#    #        print('increased raan because near pi')
#    #    else:
#    #        kep[4] = np.pi - fudgeTol - 2.0 * eps
#    #        print('decreased raan because near pi')
#    #elif np.abs(kep[4] - 2.0 * np.pi) < fudgeTol:
#    #    # raan is near 360 deg
#    #    kep[4] = 2.0 * np.pi - fudgeTol - 2.0 * eps
#    #    print('decreased raan because near 2pi')

#    #print('ending kep: ', kep)

#    ## now, convert new keplerians to cartesian
#    #cart = StateConv(kep, stateTypes.ClassicalKeplerian.value, stateTypes.Cartesian.value, mu)
#    #rv = np.array(cart[0:3])
#    #vv = np.array(cart[3:6])


#    # convert state to position and velocity vectors if necessary
#    if inputType != stateTypes.Cartesian.value:
#        cart = StateConv(state, inputType, stateTypes.Cartesian.value, mu)
#        rv = np.array(cart[0:3])
#        vv = np.array(cart[3:6])
#    else:
#        rv = np.array(state[0:3])
#        vv = np.array(state[3:6])

#    # basic orbit properties
#    r = np.linalg.norm(rv)
#    v = np.linalg.norm(vv)
#    hv = np.cross(rv, vv)
#    h = np.linalg.norm(hv)
#    hvz = hv[2]

#    # useful unit vectors in calculating RAAN and AOP
#    zhat = np.array([0.0, 0.0, 1.0])
#    xhat = np.array([1.0, 0.0, 0.0])

#    # the node vector
#    nv = np.cross(zhat, hv)
#    n = np.linalg.norm(nv)
#    nvx = nv[0]

#    # the eccentricity vector, energy, and SMA
#    ev = ((v**2 - mu / r) * rv - np.dot(rv, vv) * vv) / mu
#    ECC = np.linalg.norm(ev)
#    E = v**2 * 0.5 - mu / r

#    # inclination
#    INC = acosSafe(hv[2] / h)
    
#    # check eccentricity for parabolic orbit
#    if abs(ECC - 1.0) < 2.0 * eps:
#        print("dOEdCart: orbit is nearly parabolic via ECC and state conversion routine is near numerical singularity")

#    # check energy for parabolic orbit
#    if abs(E) < 2.0 * eps:
#        print("dOEdCart: orbit is nearly parabolic via Energy and state conversion routine is near numerical singularity")


#    # determine SMA depending on orbit type
#    if abs(ECC - 1.0) < 2.0 * eps:
#        SMA = np.inf
#    else:
#        SMA = -mu / 2.0 / E

   
#    # derivatives of intermediate quantities

#    # derivatives of eccentricity vector and eccentricity unit vector
#    devdrv = ((v**2 - mu / r) * np.identity(3) + mu * np.outer(rv, rv) / r**3 - np.outer(vv, vv)) / mu
#    devdvv = 1.0 / mu * (2.0 * np.outer(rv, vv) - np.dot(rv, vv) * np.identity(3) - np.outer(vv, rv))
#    dehatdrv = devdrv / (ECC + fudgeTol) - np.dot(np.outer(ev, ev), devdrv) / (ECC**3 + fudgeTol)
#    dehatdvv = devdvv / (ECC + fudgeTol) - np.dot(np.outer(ev, ev), devdvv) / (ECC**3 + fudgeTol)

#    # derivatives of the node vector and node unit vector
#    dndrv = (-1.0 / (n + fudgeTol)) * np.dot(nv, np.dot(skew(zhat), skew(vv)))
#    dnvdrv = np.dot(-skew(zhat), skew(vv))
#    dndvv = (1.0 / (n + fudgeTol)) * np.dot(nv, np.dot(skew(zhat), skew(rv)))
#    dnvdvv = np.dot(skew(zhat), skew(rv))
#    dnhatdrv = dnvdrv / (n + fudgeTol) - np.outer(nv, dndrv) / (n**2 + fudgeTol)
#    dnhatdvv = dnvdvv / (n + fudgeTol) - np.outer(nv, dndvv) / (n**2 + fudgeTol)

#    # derivatives of orbital elements

#    # SMA
#    dSMAdrv = 2.0 * (SMA / r)**2 / r * rv
#    dSMAdvv = 2.0 * SMA**2 / mu * vv

#    # ECC
#    temp1 = (v**2 - mu / r) *np.identity(3)
#    temp2 = (mu / r**3) * np.outer(rv, rv)
#    dECCdrv = (1.0 / (ECC + fudgeTol) / mu) * np.dot(ev, temp1 + temp2 - np.outer(vv, vv))
#    dECCdvv = (1.0 / (ECC + fudgeTol) / mu) * np.dot(ev, 2.0 * np.outer(rv, vv) - np.dot(np.dot(rv, vv), np.identity(3)) - np.outer(vv, rv))

#    # INC
#    dINCdrv = (1.0 / (np.sqrt(1.0 - (hvz / h)**2) + fudgeTol)) * np.dot(np.dot(zhat, (1.0/h) * np.identity(3) - (1.0/h**3) * np.outer(hv, hv)), skew(vv))
#    dINCdvv = (-1.0 / (np.sqrt(1.0 - (hvz / h)**2) + fudgeTol)) * np.dot(np.dot(zhat, (1.0/h) * np.identity(3) - (1.0/h**3) * np.outer(hv, hv)), skew(rv))

#    # RAAN
#    temp1 = (nvx / (n**3 + fudgeTol)) * nv
#    temp2 = (1.0 / (n + fudgeTol)) * xhat
#    dRAANdrv = (-1.0 / (np.sqrt(1.0 - (nvx / (n + fudgeTol))**2) + fudgeTol)) * np.dot(temp1 - temp2, np.dot(skew(zhat), skew(vv)))
#    dRAANdvv = (1.0 / (np.sqrt(1.0 - (nvx / (n + fudgeTol))**2) + fudgeTol)) * np.dot(temp1 - temp2, np.dot(skew(zhat), skew(rv)))

#    if nv[1] < 0.0:
#        dRAANdrv = -dRAANdrv
#        dRAANdvv = -dRAANdvv

#    # AOP
#    temp1 = (1.0 / (n + fudgeTol)) * np.dot(nv, dehatdrv)
#    temp2 = (1.0 / (ECC + fudgeTol)) * np.dot(ev, dnhatdrv)
#    dAOPdrv = (-1.0 / (np.sqrt(1.0 - (np.dot(ev, nv) / (ECC + fudgeTol) / (n + fudgeTol))**2) + fudgeTol)) * (temp1 + temp2)
#    temp1 = (1.0 / (n + fudgeTol)) *np.dot(nv, dehatdvv)
#    temp2 = (1.0 / (ECC + fudgeTol)) * np.dot(ev, dnhatdvv)
#    dAOPdvv = (-1.0 / (np.sqrt(1.0 - (np.dot(ev, nv) / (ECC + fudgeTol) / (n + fudgeTol))**2) + fudgeTol)) * (temp1 + temp2)

#    if ev[2] < 0.0:
#        dAOPdrv = -dAOPdrv
#        dAOPdvv = -dAOPdvv

#    # TA
#    temp1 = (1.0 / r) * np.dot(rv, dehatdrv)
#    temp2 = (1.0 / (ECC + fudgeTol)) * ev
#    temp3 = (1.0 / r) * np.identity(3) - (1.0 / r**3) * np.outer(rv, rv)
#    dTAdrv = (-1.0 / np.sqrt(1.0 - (np.dot(ev, rv) / (ECC + fudgeTol) / r)**2)) * (temp1 + np.dot(temp2, temp3))
#    temp1 = (1.0 / r) * np.dot(rv, dehatdvv)
#    dTAdvv = (-1.0 / np.sqrt(1.0 - (np.dot(ev, rv) / (ECC + fudgeTol) / r)**2)) * temp1

#    if np.dot(rv, vv) < 0.0:
#        dTAdrv = -dTAdrv
#        dTAdvv = -dTAdvv

#    # assemble the jacobian
#    J = np.empty((6,6))
#    J[:,0] = np.concatenate((dSMAdrv, dSMAdvv))
#    J[:,1] = np.concatenate((dECCdrv, dECCdvv))
#    J[:,2] = np.concatenate((dINCdrv, dINCdvv))
#    J[:,3] = np.concatenate((dAOPdrv, dAOPdvv))
#    J[:,4] = np.concatenate((dRAANdrv, dRAANdvv))
#    J[:,5] = np.concatenate((dTAdrv, dTAdvv))
#    J = J.transpose()

#    return J

def dCartdSphAzFPA(state, inputType, mu = 398600.4415):
    """Returns Jacobian of Cartesian state w.r.t. spherical AzFPA state"""
    # @param state Value of state at which to calculate Jacobian
    # @param type of input state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return J Jacobian, organized in the order of the state types below
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    # convert state to SphericalAzFPA if necessary
    if inputType != stateTypes.SphericalAzFPA.value:
        stateSphericalAzFPA = StateConv(state, inputType, stateTypes.SphericalAzFPA.value, mu)
        RMAG = stateSphericalAzFPA[0]
        RA = stateSphericalAzFPA[1]
        DEC = stateSphericalAzFPA[2]
        VMAG = stateSphericalAzFPA[3]
        AZI = stateSphericalAzFPA[4]
        FPA = stateSphericalAzFPA[5]
    else:
        RMAG = state[0]
        RA = state[1]
        DEC = state[2]
        VMAG = state[3]
        AZI = state[4]
        FPA = state[5]

    # repeatedly used terms
    cosRA = np.cos(RA)
    sinRA = np.sin(RA)
    cosDEC = np.cos(DEC)
    sinDEC = np.sin(DEC)
    cosAZI = np.cos(AZI)
    sinAZI = np.sin(AZI)
    cosFPA = np.cos(FPA)
    sinFPA = np.sin(FPA)

    # derivatives of position
    drvdRMAG = np.array([cosDEC * cosRA, cosDEC * sinRA, sinDEC])

    drvdRA = RMAG * np.array([-cosDEC * sinRA, cosDEC * cosRA, 0.0])

    drvdDEC = RMAG * np.array([-sinDEC * cosRA, -sinDEC * sinRA, cosDEC])

    drvdVMAG = np.array([0.0, 0.0, 0.0])

    drvdAZI = np.array([0.0, 0.0, 0.0])

    drvdFPA = np.array([0.0, 0.0, 0.0])

    # derivatives of velocity
    dvvdRMAG = np.array([0.0, 0.0, 0.0])

    dvvdRA = VMAG * np.array([-cosFPA * cosDEC * sinRA - sinFPA * (sinAZI * cosRA - cosAZI * sinDEC * sinRA),
                         cosFPA * cosDEC * cosRA + sinFPA * (-sinAZI * sinRA - cosAZI * sinDEC * cosRA),
                         0.0])

    dvvdDEC = VMAG * np.array([-cosFPA * sinDEC * cosRA - sinFPA * (cosAZI * cosDEC * cosRA),
                         -cosFPA * sinDEC * sinRA + sinFPA * (-cosAZI * cosDEC * sinRA),
                         cosFPA * cosDEC - sinFPA * cosAZI * sinDEC])

    dvvdVMAG = np.array([cosFPA * cosDEC * cosRA - sinFPA * (sinAZI * sinRA + cosAZI * sinDEC * cosRA),
                         cosFPA * cosDEC * sinRA + sinFPA * (sinAZI * cosRA - cosAZI * sinDEC * sinRA),
                         cosFPA * sinDEC + sinFPA * cosAZI * cosDEC])

    dvvdAZI = VMAG * np.array([-sinFPA * (cosAZI * sinRA - sinAZI * sinDEC * cosRA),
                         sinFPA * (cosAZI * cosRA + sinAZI * sinDEC * sinRA),
                         -sinFPA * sinAZI * cosDEC])

    dvvdFPA = VMAG * np.array([-sinFPA * cosDEC * cosRA - cosFPA * (sinAZI * sinRA + cosAZI * sinDEC * cosRA),
                         -sinFPA * cosDEC * sinRA + cosFPA * (sinAZI * cosRA - cosAZI * sinDEC * sinRA),
                         -sinFPA * sinDEC + cosFPA * cosAZI * cosDEC])

    # construct jacobian
    J = np.empty((6,6))
    J[0:3,:] = np.array([drvdRMAG, drvdRA, drvdDEC, drvdVMAG, drvdAZI, drvdFPA]).transpose()
    J[3:6,:] = np.array([dvvdRMAG, dvvdRA, dvvdDEC, dvvdVMAG, dvvdAZI, dvvdFPA]).transpose()

    return J

def dSphAzFPAdCart(state, inputType, mu = 398600.4415):
    """Returns Jacobian of spherical AzFPA state w.r.t. Cartesian state"""
    # @param state Value of state at which to calculate Jacobian
    # @param type of input state
    # @param mu Gravitational parameter of central body (default value is Earth's in km^3/s^2)
    # @return J Jacobian, organized in the order of the state types below
    #
    # Definitions of Types:
    # Cartesian: [X, Y, Z, VX, VY, VZ]
    # ClassicalKeplerian: [SMA, ECC, INC, AOP, RAAN, TA]
    # SphericalAzFPA: [RMAG, RA, DEC, VMAG, AZI, FPA]
    # All angles are in radians

    # convert state to position and velocity vectors if necessary
    if inputType != stateTypes.Cartesian.value:
        cart = StateConv(state, inputType, stateTypes.Cartesian.value, mu)
        rv = np.array(cart[0:3])
        vv = np.array(cart[3:6])
    else:
        rv = np.array(state[0:3])
        vv = np.array(state[3:6])

    # frequently used terms
    RMAG = np.linalg.norm(rv)
    VMAG = np.linalg.norm(vv)
    RMAGdot = np.dot(rv, vv) / RMAG
    v2MinusRmagdot2 = VMAG**2 - RMAGdot**2
    rx = rv[0]
    ry = rv[1]
    rz = rv[2]
    vx = vv[0]
    vy = vv[1]
    vz = vv[2]
    rx2PlusRy2 = rx**2 + ry**2

    # derivatives w.r.t. position
    dRMAGdrv = (1.0 / RMAG) * rv

    dRAdrv = np.array([-ry / rx2PlusRy2, rx / rx2PlusRy2, 0.0])

    dDECdrv = (1.0 / (RMAG**2 * np.sqrt(rx2PlusRy2))) * np.array([-rx * rz, -ry * rz, rx2PlusRy2])

    dVMAGdrv = np.zeros((3))

    dAZIdrv = (1.0 / (v2MinusRmagdot2 * rx2PlusRy2)) * np.array([vy * (RMAG * vz - rz * RMAGdot) - ((rx * vy - ry * vx) / RMAG) * (rx * vz - rz * vx + ((rx * rz * RMAGdot) / RMAG)),
                                                                 -vx * (RMAG * vz - rz * RMAGdot) - ((rx * vy - ry * vx) / RMAG) * (ry * vz - rz * vy + ((ry * rz * RMAGdot) / RMAG)),
                                                                 ((rx * vy - ry * vx) * rx2PlusRy2 * RMAGdot) / RMAG**2])

    dFPAdrv = (1.0 / (RMAG * np.sqrt(v2MinusRmagdot2))) * ((RMAGdot / RMAG) * rv - vv) # GTDS has RMAG**2, which is wrong

    # derivatives w.r.t. velocity
    dRMAGdvv = np.zeros((3))

    dRAdvv = np.zeros((3))

    dDECdvv = np.zeros((3))

    dVMAGdvv = (1.0 / VMAG) * vv

    dAZIdvv = (1.0 / (RMAG * v2MinusRmagdot2)) * np.array([rz * vy - ry * vz, rx * vz - rz * vx, ry * vx - rx * vy])

    dFPAdvv = (1.0 / (RMAG * np.sqrt(v2MinusRmagdot2))) * (((np.dot(rv, vv)) / VMAG**2) * vv - rv) # GTDS has RMAG**2, which is wrong

    # construct jacobian
    J = np.empty((6,6))

    J[:,0] = np.concatenate((dRMAGdrv, dRMAGdvv))
    J[:,1] = np.concatenate((dRAdrv, dRAdvv))
    J[:,2] = np.concatenate((dDECdrv, dDECdvv))
    J[:,3] = np.concatenate((dVMAGdrv, dVMAGdvv))
    J[:,4] = np.concatenate((dAZIdrv, dAZIdvv))
    J[:,5] = np.concatenate((dFPAdrv, dFPAdvv))
    J = J.transpose()


    return J

def TA2EA(TA, ECC):
    """Calculates eccentric anomaly given true anomaly and eccentricity"""
    # @param TA true anomaly (rad)
    # @param ECC eccentricity
    # @return EA eccentric anomaly (rad)
    # angle is modulated to be between zero and 2pi

    cosTA = np.cos(TA)
    sinEA = np.sqrt(1.0 - ECC * ECC) * np.sin(TA) / (1.0 + ECC * cosTA)
    cosEA = (ECC + cosTA) / (1.0 + ECC * cosTA)
    EA = np.arctan2(sinEA, cosEA)

    while EA < 0.0:
        EA = EA + 2.0 * np.pi
    return EA

def skew(v):
    """given 3-element list/array, returns the 3x3 skew-symmetric matrix"""
    # @param v Input vector (3 elements)
    # @return V 3x3 skew-symmetric matrix as a numpy array
    V = np.array([[0.0, -v[2], v[1]], [v[2], 0.0, -v[0]], [-v[1], v[0], 0.0]])

    return V

def R1(angle):
    """Returns a rotation matrix corresponding to a 1 rotation through an angle"""
    # @param angle The angle of rotation (rad)
    # @return rotmat The rotation matrix as a numpy array
    c = np.cos(angle)
    s = np.sin(angle)
    rotmat = np.array([[1.0, 0.0, 0.0], [0.0, c, s], [0.0, -s, c]])
    return rotmat

def dR1(angle):
    """Returns derivative of a rotation matrix corresponding to a 1 rotation through an angle w.r.t. that angle"""
    # @param angle The angle of rotation (rad)
    # @return dRotmat The derivative of the rotation matrix as a numpy array
    c = np.cos(angle)
    s = np.sin(angle)
    dRotmat = np.array([[0.0, 0.0, 0.0], [0.0, -s, c], [0.0, -c, -s]])
    return dRotmat

def R2(angle):
    """Returns a rotation matrix corresponding to a 2 rotation through an angle"""
    # @param angle The angle of rotation (rad)
    # @return dRotmat the rotation matrix as a numpy array
    c = np.cos(angle)
    s = np.sin(angle)
    rotmat = np.array([[c, 0.0, -s], [0.0, 1.0, 0.0], [s, 0.0, c]])
    return rotmat

def dR2(angle):
    """Returns derivative of a rotation matrix corresponding to a 2 rotation through an angle w.r.t. that angle"""
    # @param angle The angle of rotation (rad)
    # @return dRotmat The derivative of ghe rotation matrix as a numpy array
    c = np.cos(angle)
    s = np.sin(angle)
    dRotmat = np.array([[-s, 0.0, -c], [0.0, 0.0, 0.0], [c, 0.0, -s]])
    return dRotmat

def R3(angle):
    """Returns a rotation matrix corresponding to a 3 rotation through an angle"""
    # @param angle The angle of rotation (rad)
    # @return rotmat The rotation matrix as a numpy array
    c = np.cos(angle)
    s = np.sin(angle)
    rotmat = np.array([[c, s, 0.0], [-s, c, 0.0], [0.0, 0.0, 1.0]])
    return rotmat

def dR3(angle):
    """Returns derivative of a rotation matrix corresponding to a 3 rotation through an angle w.r.t. that angle"""
    # @param angle The angle of rotation (rad)
    # @return dRotmat The derivative of the rotation matrix as a numpy array
    c = np.cos(angle)
    s = np.sin(angle)
    dRotmat = np.array([[-s, c, 0.0], [-c, -s, 0.0], [0.0, 0.0, 0.0]])
    return dRotmat



if __name__ == "__main__":
    # test basic stuff
    np.set_printoptions(precision=20)

    mu = 398600.4415 # earth
    test_stateconv = 0
    test_partials = 1

    if test_partials == 1:
        oes = [[6578.0, 0.02, np.deg2rad(98.0), 3.0, 6.0, 2.0]] # non-circular inclined, not at pole
        oe = oes[0]

        # state values in different representations
        cart = StateConv(oe,stateTypes.ClassicalKeplerian.value,stateTypes.Cartesian.value)
        kep = StateConv(cart,stateTypes.Cartesian.value,stateTypes.ClassicalKeplerian.value)
        sphAzFPA = StateConv(oe,stateTypes.ClassicalKeplerian.value,stateTypes.SphericalAzFPA.value)

        # derivatives

        # x_c / x_k
        dCartdOE_ = dCartdOE(cart,stateTypes.Cartesian.value,mu)

        # x_c / x_s
        dCartdSphAzFPA_ = dCartdSphAzFPA(cart,stateTypes.Cartesian.value,mu)

        # x_k / x_c
        dOEdCart_ = dOEdCart(cart,stateTypes.Cartesian.value,mu)

        # x_s / x_c
        dSphAzFPAdCart_ = dSphAzFPAdCart(cart,stateTypes.Cartesian.value,mu)

        # compare against automatic differentiation

        # d Cart / d Kep
        dCartdOEAD = []
        for i in range(6):
            dCartdOEFun = jacobian(Kep2CartScalars, i)
            dCartdOEAD.append(dCartdOEFun(oe[0], oe[1], oe[2], oe[3], oe[4], oe[5], mu))
            print(i, 'DIFF: ', dCartdOEAD[i] - dCartdOE_.transpose()[i])
            print(i, 'AD:   ', dCartdOEAD[i])
            print(i, 'ANA:  ', dCartdOE_.transpose()[i])
            for j in range(6):
                if abs(dCartdOEAD[i][j]) > 2.0 * eps:
                    print(i, j, (dCartdOEAD[i][j] - dCartdOE_.transpose()[i,j]) / dCartdOEAD[i][j])
            print('')

        # d Kep / d Cart
        dOEdCartAD = []
        for i in range(6):
            dOEdCartFun = jacobian(Cart2KepScalars, i)
            dOEdCartAD.append(dOEdCartFun(cart[0], cart[1], cart[2], cart[3], cart[4], cart[5], mu))
            print(i,  'DIFF: ', dOEdCartAD[i] - dOEdCart_.transpose()[i])
            print(i, 'AD:   ', dOEdCartAD[i])
            print(i, 'ANA:  ', dOEdCart_.transpose()[i])
            for j in range(6):
                if abs(dOEdCartAD[i][j]) > 2.0 * eps:
                    print(i, j, (dOEdCartAD[i][j] - dOEdCart_.transpose()[i,j]) / dOEdCartAD[i][j])
            print('')

        # d Cart / d SphAzFPA
        dCartdSphAzFPAAD = []
        for i in range(6):
            dCartdSphAzFPAFun = jacobian(SphAzFPA2CartScalars, i)
            dCartdSphAzFPAAD.append(dCartdSphAzFPAFun(sphAzFPA[0], sphAzFPA[1], sphAzFPA[2], sphAzFPA[3], sphAzFPA[4], sphAzFPA[5]))
            print(i, 'DIFF: ', dCartdSphAzFPAAD[i] - dCartdSphAzFPA_.transpose()[i])
            print(i, 'AD:   ', dCartdSphAzFPAAD[i])
            print(i, 'ANA:  ', dCartdSphAzFPA_.transpose()[i])
            for j in range(6):
                if abs(dCartdSphAzFPAAD[i][j]) > 2.0 * eps:
                    print(j, (dCartdSphAzFPAAD[i][j] - dCartdSphAzFPA_.transpose()[i,j]) / dCartdSphAzFPAAD[i][j])
            print('')

        # d SphAzFPA / d Cart
        dSphAzFPAdCartAD = []
        for i in range(6):
            dSphAzFPAdCartFun = jacobian(Cart2SphAzFPAScalars, i)
            dSphAzFPAdCartAD.append(dSphAzFPAdCartFun(cart[0], cart[1], cart[2], cart[3], cart[4], cart[5]))
            print(i, 'DIFF: ', dSphAzFPAdCartAD[i] - dSphAzFPAdCart_.transpose()[i])
            print(i, 'AD:   ', dSphAzFPAdCartAD[i])
            print(i, 'ANA:  ', dSphAzFPAdCart_.transpose()[i])
            for j in range(6):
                if abs(dSphAzFPAdCartAD[i][j]) > 2.0 * eps:
                    print(j, (dSphAzFPAdCartAD[i][j] - dSphAzFPAdCart_.transpose()[i,j]) / dSphAzFPAdCartAD[i][j])
            print('')


    if test_stateconv == 1:
        print('\nTesting state transformations:\n')
        # starting orbital elements
        oes = [[42095.0, 0.0, 0.0, 0.0, 0.0, np.pi * 0.5]] # circular equatorial
        oes.append([82095.0, 0.8181, 0.0, 2.0, 3.0, 5.0]) # non-circular equatorial
        oes.append([7500.0, 0.0, np.pi * 0.5, 1.0, 0.4, 3.0 * np.pi * 0.35]) # circular inclined
        oes.append([6578.0, 0.02, np.deg2rad(28.5), 3.0, np.pi, 2.0]) # non-circular inclined, raan bad
        oes.append([6578.0, 0.02, np.deg2rad(98.0), 3.0, 6.0, 2.0]) # non-circular inclined
        oes.append([6578.0, 0.0, np.pi, 3.0, 6.0, 2.0]) # circular equatorial (retrograde)
        oes.append([6578.0, 0.1, np.pi * 0.5, 0.0, 0.0, 0.0]) # above north pole

        # conversions for all orbital elements
        inputReps = [1, 2, 4]
        outputReps = [1, 2, 4]
        maxError = -1.0e20
        oeIdx = 0
        error = []
        for oe in oes:
            oeIdx = oeIdx + 1
            i = -1
            for inputRep in inputReps:
                for outputRep in outputReps:
                    i = i + 1
                    # convert from keplerian to the inputRep of choice
                    stateIn = StateConv(oe, 2, inputRep, mu)

                    # convert from inputRep to outputRep
                    stateOut = StateConv(stateIn, inputRep, outputRep, mu)

                    # convert back to inputRep
                    stateCompare = StateConv(stateOut, outputRep, inputRep, mu)

                    # convert stateOut and StateCompare to Cartesian for comparison
                    c1 = StateConv(stateIn, inputRep, 1, mu)
                    c2 = StateConv(stateCompare, inputRep, 1, mu)
                    error.append(np.linalg.norm(np.array(c1) - np.array(c2)))
                    print("Error: ", oeIdx, " ", inputRep, " ", outputRep, " Diff = ", error[-1])
                    if error[-1] > maxError:
                        maxError = error[-1]
                    #print('Next output rep')
                #print('Next input rep')
            #print('Next state')

        print("MaxError = ", maxError)

    print('done')
