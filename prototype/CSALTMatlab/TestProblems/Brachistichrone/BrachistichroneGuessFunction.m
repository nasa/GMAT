function [stateGuess,controlGuess] = BrachistichroneGuessFunction(timeVector,timeTypeVec,numControls)

%  Load the data
timeArray = [
                        0
        0.0114141685333755
        0.0360554075051943
        0.0689541298143692
         0.103590966094033
         0.133106067994036
          0.15166173029212
         0.156242193167397
         0.165032779018899
         0.184403394142377
         0.211337052334191
         0.241730675744777
         0.270956801501355
         0.294566679558573
         0.308972157696551
         0.312484386334793];

stateArray = [
                         0     -6.93889390390723e-18                         0
      8.16216471956306e-05      -0.00209335356224662        -0.367030569193634
       0.00255802072079951       -0.0206764487060992          -1.1534595183849
         0.017576989760147       -0.0733559014031684         -2.17263315545347
        0.0575234147384939        -0.157140512437347         -3.17988099640059
         0.117235866736403        -0.244136986424697         -3.96355405000006
         0.168487507533415        -0.302592770370206         -4.41261995618512
         0.182840627387957        -0.317163015027576         -4.51761062089568
         0.212265017800589        -0.345095921350832         -4.71235022588855
         0.285950304547639        -0.405279982518614         -5.10675330323823
         0.407328259104916        -0.482632705426008         -5.57283490463674
          0.56726456103237         -0.55540536779288         -5.97822560377556
         0.738861870216646        -0.605377759510737         -6.24138535960857
         0.885931429587651        -0.628140847071256         -6.35763884431994
         0.977575996094503        -0.633364724282878         -6.38402514252426
                         1        -0.633595410590581         -6.38518578757638];

controlArray = [
                         0
       -0.0583190647395084
        -0.184974145001816
        -0.352788023343274
        -0.525660675738607
        -0.672264266353711
        -0.766080500141532
        -0.787252419691528
        -0.834838441149836
        -0.934936420754955
         -1.06915278560299
         -1.21623940756471
         -1.35816149345997
         -1.47668215328505
         -1.55128339407895
         -1.6];

if timeVector(1) < timeArray(1) || timeVector(end) > timeArray(end)
    error('Requested time for guess is not contained in the guess data')
end

% Determine which points have state and control
statePoints = find(timeTypeVec == 1 | timeTypeVec == 2);
controlPoints = find(timeTypeVec == 1 | timeTypeVec == 3);
stateTimes = timeVector(statePoints); %#ok<FNDSB>
controlTimes = timeVector(controlPoints); %#ok<FNDSB>

%  Interpolate the state
for stateIdx = 1:size(stateArray,2)
    stateGuess(:,stateIdx) = interp1(timeArray,stateArray(:,stateIdx),stateTimes);
end

% Interploate the controls
%  kludge because Radau does not have control at the last point.
if size(controlArray,1) < size(timeArray,1);
    for controlIdx = 1:size(controlArray,2)
        controlGuess(:,controlIdx) = ...
            spline(timeArray(1:end-1),controlArray(:,controlIdx),...
            controlTimes); %#ok<*AGROW>
    end
else
    for controlIdx = 1:size(controlArray,2)
        controlGuess(:,controlIdx) = ...
            spline(timeArray,controlArray(:,controlIdx),controlTimes);
    end
end



