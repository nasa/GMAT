function [stateGuess,controlGuess] = DROtoL2NewGuessFunction(timeVector,timeTypeVec,numControls)

x =[
   1.0765
    1.0686
    0.9933
    0.9418
    0.9331
    0.9838
    1.0733
    1.1086
    1.0836
    1.0317
    0.9545
    0.9200
    0.9172
    0.9798
    1.0733
    1.1221
    1.1253
    1.1356
    1.1395
    1.1209
];

y = [
   -0.0058
   -0.0395
   -0.0930
   -0.0443
    0.0275
    0.0786
    0.0635
   -0.0082
   -0.0688
   -0.0946
   -0.0855
   -0.0546
    0.0429
    0.0968
    0.1071
    0.0695
    0.0536
    0.0081
   -0.0609
   -0.0997];

numStates = length(x);
z = zeros(numStates,1);
times = linspace(0,5.0,numStates)';
vel = zeros(numStates,1);
mass = 1.5*ones(numStates,1);
timeArray = times;
x = rand(numStates,1)/2+0.8;
y = rand(numStates,1)/4 - 0.15;
stateArray = [x y z vel vel vel mass];
controlArray = zeros(numStates,3);

if timeVector(1) < timeArray(1) || timeVector(end) > timeArray(end)
    error('Requested time for guess is not contained in the guess data')
end

% Determine which points have state and control
statePoints = find(timeTypeVec == 1 | timeTypeVec == 2);
controlPoints = find(timeTypeVec == 1 | timeTypeVec == 3);
stateTimes = timeVector(statePoints); %#ok<FNDSB>
controlTimes = timeVector(controlPoints); %#ok<FNDSB>

%  Interpolate the state
stateGuess = interp1(timeArray, stateArray, stateTimes,'spline');

% Interploate the controls
%  kludge because Radau does not have control at the last point.
if size(controlArray,1) < size(timeArray,1);
    controlGuess = interp1(timeArray(1:end-1),controlArray, controlTimes,'spline');
else
    controlGuess = interp1(timeArray, controlArray, controlTimes,'spline');
end



