%% =====  Intializations
ClearAll
import HyperSensitivePathObject
import exec.RadauPhase

%%  Configure Phase 1
pathFuncObj = HyperSensitivePathObject();

numStateVars = 1;
phase1 = RadauPhase;
phase1.initialGuessMode = 'LinearNoControl';
meshIntervalFractions = [ -1 -0.25 0 0.25  1];
meshIntervalNumPoints = [7 5 5 7]';
%  Time configuration
timeLowerBound = 0;
initialGuessTime = 0;
finalGuessTime = 50;
timeUpperBound = 50;
%  State configuration
numStateVars = numStateVars;
stateLowerBound = -50;
initialGuessState = 1;
finalGuessState = 1;
stateUpperBound = 50;
%  Control configuration
controlUpperBound = 50;
controlLowerBound = -50;
numControlVars = 1;

decVector = [                         0
                        50
                         1
        -0.267072440656336
        0.0891181633996139
        -0.243508190120526
        0.0484165106053577
        0.0169622684299867
       -0.0268155310752096
      -0.00677719756314767
        0.0235758753831317
      -0.00382863711513253
       -0.0253337916016031
        0.0128680672502992
        0.0300845932331574
       -0.0227768312559897
      -0.00943173787255556
       0.00891501151323474
      -0.00396333759703528
       0.00409434752414663
      -0.00068714571521797
      0.000623761890282567
     -0.000114910396991732
      0.000147297256217489
     -1.78453360134338e-06
      3.77859113818496e-05
      1.50849378640702e-05
     -0.000165390075365232
     -4.30904443626666e-05
      3.54978215573492e-06
      1.46858916167594e-05
      2.58564174129133e-05
      4.76523799886422e-05
      4.26279395604326e-05
       0.00021658578944853
       0.00023292067861142
       0.00031325102217287
         0.108744954891378
     -0.000752772948667303
       -0.0471261437195893
       0.00101668584609107
        0.0408591711271692
      -0.00148490261997778
       -0.0434941196604928
       0.00398209759309219
        0.0566542351615857
        0.0404125740225061
       -0.0428474879835071
         0.453037117849855
         0.810782397858858
                         1];



