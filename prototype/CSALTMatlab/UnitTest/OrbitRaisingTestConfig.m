%==========================================================================
%% =====  Problem specific configuration
%==========================================================================
ClearAll
import OrbitRaisingPathObject
import exec.RadauPhase

phase1 = RadauPhase();
phase1.initialGuessMode  = 'LinearUnityControl';
meshIntervalFractions  = [-1; 1];
meshIntervalNumPoints = 7*ones(1,1);

pathFuncObj = OrbitRaisingPathObject();
%  Set time properties
timeLowerBound   = 0;
initialGuessTime = 0;
finalGuessTime   = 1;
timeUpperBound   = 3.32;
%  Set state properties
numStateVars              = 5;
stateLowerBound           = [0.5 0 -10 -10 0.1]';
initialGuessState         = [1 0 0  1 1]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState           = [1 pi 0 .5 0.5]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound           = [5 4*pi 10 10 3]';
%  Set control properties
numControlVars            = 2;
controlUpperBound         = [10 10]';
controlLowerBound         = [-10 -10]';

decVector = [
                         0
                      3.32
                         1
      2.84101063191129e-13
                         0
          1.00000000000002
                         1
          0.46564207244944
         0.884974100541562
          1.00292150959124
          0.24510014177824
        0.0248656190820442
          1.02556394151943
         0.981833726195737
         0.566713541657015
         0.823914988214297
          1.03953838754381
         0.783146974241609
         0.128385089613886
          1.04637271043109
         0.942615846003289
         0.826929089699722
         0.562306508250403
          1.19252136085221
          1.41233382715236
         0.293096855201965
         0.914835511777526
         0.890255729229884
         0.923208252346056
        -0.384300120276895
             1.40796689256
          1.88052713429625
         0.245429245261179
         0.755727892844444
         0.835129308962834
        -0.952951568440912
          0.30312263963231
          1.50799680500876
          2.20156392370062
        0.0743757524753398
         0.766816704594532
         0.788154409222363
        -0.823874663720378
         0.566772060138446
           1.5238694762769
          2.40497639294557
         0.018970008002483
         0.794803522925167
         0.758622057308969
        -0.734971943498651
         0.678098021376783
          1.52560750596174
           2.4557570010183
      5.54801539173474e-18
         0.809615085255086
                  0.751332];



