function [stateGuess,controlGuess] = LaunchVehicleAscentGuessFunction(timeVector,timeTypeVec,numControls)

%  Load the data
timeArray = [0 75 155]';

stateArray = [0 0 0 0;
             12e5/2 12e5/2 7000/2 5000
             12e5 12e5 7000 0]/1000; 

controlAngleArray = [pi/2;0;-1];
controlArray(:,1) = cos(controlAngleArray);
controlArray(:,2) = sin(controlAngleArray);

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



