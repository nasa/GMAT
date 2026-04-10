function [stateGuess,controlGuess] = DROtoL2GuessFunction(timeVector,timeTypeVec,~)

% Load initial guess (good guess for initial half of tutorial)
f = 'initial_guess';
timeArray = importdata(fullfile(f,'t_guess.dat'))';
stateArray = importdata(fullfile(f,'x_guess.dat'))';
controlArray = importdata(fullfile(f,'u_guess.dat'))';

% Check that requested time is contained in the data and error if not
if timeVector(1) < timeArray(1) || timeVector(end) > timeArray(end)
    error('Requested time for guess is not contained in the guess data')
end

% Determine which points have state and control, or only state
statePoints = find(timeTypeVec == 1 | timeTypeVec == 2);
controlPoints = find(timeTypeVec == 1 | timeTypeVec == 3);
stateTimes = timeVector(statePoints); %#ok<FNDSB>
controlTimes = timeVector(controlPoints); %#ok<FNDSB>

%  Interpolate the data to the requested times
stateGuess = interp1(timeArray, stateArray, stateTimes,'spline');
controlGuess = interp1(timeArray, controlArray, controlTimes,'spline');


