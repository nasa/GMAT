"""
A GMATFunction to perform a patched-conic flyby.

2018-12-14; nhatten; created

@author: nhatten
"""
    
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
import spiceypy
import GMATFunction
from TimeUtil import TimeUtil
from helperData import helperData
from patched_conic_flyby_validation import patched_conic_flyby_validation as patchedConicMath

class PatchedConicFlybyFunction(GMATFunction.GMATFunction):
    
    def __init__(self):
        """
        Constructor
        """
        
        # Call parent class init method
        super().__init__()

        # create helperData object
        hD = helperData()

        # create coordinate translation object
        #cTranslation = CoordinateTranslation(hD.getKernel['naifTlsKernel'], hD.getKernel['deKernel'])

        # create coordinate transformation object
        #cTransformation = CoordinateTransformation(hD.getKernel['naifTlsKernel'], hD.getKernel['pckKernel'])

        # create time utility object
        tlsKernel = hD.getKernel('naifTlsKernel')
        self.tUtil = TimeUtil(tlsKernel)
        spiceypy.furnsh(hD.getKernel('deKernel'))
        spiceypy.furnsh(hD.getKernel('pckKernel'))

        return

    def __del__(self):
        """
        Destructor
        """

        # unload spice kernels
        spiceypy.unload(hD.getKernel('deKernel'))
        spiceypy.unload(hD.getKernel('pckKernel'))
        return

    def EvaluateFunction(self):
        """
        Evaluate the constraint
        @return funcValue The scaled value of the function
        """
        
        stateMinus = self.GetUnscaledStateForFun(0, returnElements=['All']) # get the incoming state in the format desired by the function (Cartesisan EclipJ2000 w.r.t. central body)
        statePlus = self.GetUnscaledStateForFun(1, returnElements=['All']) # get the outgoing state in the format desired by the function (Cartesisan EclipJ2000 w.r.t. central body)
        tMinus = self.GetTime(0) # get the incoming time
        tPlus = self.GetTime(1) # get the outgoing time

        # convert the times to spice ephemeris times
        etMinus = self.tUtil.MJDA12ET(tMinus, useSpice = 0)
        etPlus = self.tUtil.MJDA12ET(tPlus, useSpice = 0)

        # get the flyby body and central body
        centralBody = self.GetCustomSetting('centralBody')
        flybyBody = self.GetCustomSetting('flybyBody')
        mu = self.GetCustomSetting('flybyBodyMu')

        # get the state of the flyby body w.r.t. the central body at tMinus and tPlus
        flybyBodyStatePlus, LT = spiceypy.spkezr(flybyBody, etPlus, self.funConfig.Axes, 'None', centralBody)
        flybyBodyStateMinus, LT = spiceypy.spkezr(flybyBody, etMinus, self.funConfig.Axes, 'None', centralBody)

        # position at tPlus constraint
        cPositionPlus = statePlus[0:3] - flybyBodyStatePlus[0:3]
        
        # position at tMinus constraint
        cPositionMinus = stateMinus[0:3] - flybyBodyStateMinus[0:3]

        # v infinity magnitude equality constraint
        vInfPlus = statePlus[3:6] - flybyBodyStatePlus[3:6]
        vInfMinus = stateMinus[3:6] - flybyBodyStateMinus[3:6]
        cVInf = np.linalg.norm(vInfPlus) - np.linalg.norm(vInfMinus)

        # mass continuity constraint
        cMass = statePlus[6] - stateMinus[6]

        # time continuity constraint
        cTime = tPlus - tMinus

        # periapsis radius constraint
        delta = np.arccos(np.dot(vInfMinus, vInfPlus) / (np.linalg.norm(vInfMinus) * np.linalg.norm(vInfPlus)))
        cRp = (mu / np.linalg.norm(vInfPlus)**2) * ((1.0 / np.sin(0.5 * delta)) - 1.0)

        # put constraints into one array
        funcValue = np.concatenate((cPositionPlus, cPositionMinus, [cVInf], [cMass], [cTime], [cRp]))

        # scale the function value
        funcValue = self.funScaler.ScaleVector(funcValue, self.funConfig.Units)

            
        return funcValue


    
    
        



