meshIntervalFractions   = [-1; 1];
meshIntervalNumPoints   = 5;
%  Set time properties
initialGuessTime  = 0;
finalGuessTime    = 2;
timeLowerBound    = 0;
timeUpperBound    = 2;
%  Set state state properties
numStateVars      = 3;
stateLowerBound   = [-10 -10 -10]';
initialGuessState = [0 0 0 ]'; % [r0 theta0 vr0 vtheta0 m0]
finalGuessState   = [1 1 1]'; % [rf thetaf vrf vthetaf mf]
stateUpperBound   = [10 10 10 ]';
%  Set control properties
numControlVars    = 2;
controlUpperBound = [2 2]';
controlLowerBound = [-2 -2]';

decVectorAtSolution = [
    0
    2
    0
    0
    0
    2.02685447364672e-06
    -1.00000000022748
    0.0112037580927815
    -0.0600920505253997
    0.414941654018491
    0.371932042632291
    -0.928259962644309
    0.328994014385667
    -0.312591078335103
    1.05800722003318
    0.939463922725005
    -0.342647833671246
    1.07758902701204
    -0.301403478462027
    1.35368538830518
    0.941324938103115
    0.337501645850245
    1.59162774662842
    0.00570039265319998
    1.35647467435353
    0.786340826003737
    0.617792930847845
    1.71237398180327
    0.1
    1.34040352273402];