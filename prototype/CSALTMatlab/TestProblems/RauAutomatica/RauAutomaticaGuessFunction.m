function [stateGuess,controlGuess] = BrachistichroneGuessFunction(timeVector,timeTypeVec)


%  Load the data
timeArray = [
    -1e-07
    0.057054748403146
    0.169992800797119
    0.295217789787462
    0.384922834801366
    0.408234758205091
    0.465289606608237
    0.57822765900221
    0.703452647992553
    0.793157693006457
    0.816469616410183];

stateArray = [
    -2.77555756156289e-17                         0                         0
    0.000905139544324532       -0.0162367161200136         -0.56988392137426
    0.0238632003084004        -0.141485975543593         -1.68215702263789
    0.121871143720108        -0.408869801372039          -2.8596289652748
    0.263086296292873        -0.664440535475751         -3.64537314124312
    0.311335897673042         -0.73712913453615         -3.83960717236965
    0.451070824778489        -0.922501833712822         -4.29534539536863
    0.822148476163856         -1.30292146420366         -5.10475484468853
    1.37956304901694          -1.7027736754882         -5.83569750943825
    1.8640639347234          -1.9455482051056         -6.23787214592141
    2                        -2         -6.32455531781641];

controlArray = [
    0
    -0.0839759947154378
    -0.250982562937529
    -0.43578942911677
    -0.568554725681452
    -0.603314063085919
    -0.687137541913234
    -0.854021218335888
    -1.03898886005128
    -1.17231766155697];

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



