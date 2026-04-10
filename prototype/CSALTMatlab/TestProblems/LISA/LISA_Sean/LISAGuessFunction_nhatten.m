function [stateGuess,controlGuess] = LISAGuessFunction_nhatten(timeVector,timeVectorType,phaseObj)
% guess function for LISA, obtained by reading matlab matrices
% timeVector is in scaled time

global scaleUtil

% reference time for the time vector
nTimes = length(timeVector);

% times are in seconds referenced to some reference epoch
times  = load('t_csalt.mat');

% need times in MJD
timesMJD = times.t_csalt;
nTimesInGuessArray = length(times.t_csalt);
for i=1:nTimesInGuessArray
    timesMJD(i) = times.t_csalt(i) / 86400 + scaleUtil.earthScaling.refTimeMJD;
end

% states 1-6 are pos/vel for s/c 1 in km, km/s
% states 7-12 are pos/vel for s/c 2 in km, km/s
% states 13-18 are pos/vel for s/c 3 in km, km/s
states = load('x_csalt.mat');

% loop through times in timeVector, interpolating as required to get
% guesses for all 18 states
nStates = 18;
statesPerBody = 6;
stateGuess = zeros(nTimes, nStates);
for i=1:nTimes
    timeScaled = timeVector(i); % get current scaled time
    timeMJD = scaleUtil.ScaledTimedToMJD(timeScaled, 3, scaleUtil.earthScaling.refTimeMJD); % turn the current scaled time into MJD
    
    % loop through the 3 spacecraft
    for j=1:3
        % get interpolated unscaled state at this time
        stateUnscaled = interp1(timesMJD, states.x_csalt(:,1+(j-1)*statesPerBody:j*statesPerBody), timeMJD, 'spline');
        
        % scale the state
        stateScaled = scaleUtil.earthScaling.StateScaleMat * stateUnscaled';
        
        % add to state guess
        stateGuess(i, 1+(j-1)*statesPerBody:j*statesPerBody) = stateScaled';
        
        % kill the velocity guess so we don't get a hyperbola
        %stateGuess(i, 4+(j-1)*statesPerBody:j*statesPerBody) = [0, 0, 0];
    end
end

% no control, but i have one that is just always 0
nControls = 1;
controlGuess = zeros(nTimes-1, nControls); % don't guess at last time
end