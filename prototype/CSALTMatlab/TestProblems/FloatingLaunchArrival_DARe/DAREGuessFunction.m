function [stateGuess,controlGuess] = DAREGuessFunction(timeVector,timeTypeVec,numControls)

global config

%%  Read the file
repoPath = '/Users/jknittel/Collocation/LowThrust/lowthrust';
folderLocation = '/TestProblems/DARE_MultiPhase/';
fileName = '05312021_optimal_trajectory_J0.e';

if exist([repoPath folderLocation fileName],'file') ~= 2
	error('Cant find .e file')
end

[data,cBody,refFrame,baseEpoch] = ReadEMTGGuessFile([repoPath folderLocation fileName]);

%% Redimensionalize the time vector
for rowIdx = 1:length(timeVector)
    timeVector(rowIdx) = timeVector(rowIdx)*config.TU;
end

%% Extract guess data
numRows = size(data,1);
timeArray = zeros(numRows,1);
stateArray = zeros(numRows,7);
stateArray(7,:) = 1;
controlMag = 0.000;
%controlArray = ones(numRows,3)*controlMag;
%  Rotation for J2000Eq to J2000Ec
R = [1.0 0.0 0.0;
    0.0 0.91748206 -0.397777156;
    0.0 0.39777716 0.9174820621]';
Rmat = [R zeros(3,3);zeros(3,3) R];
initMass = 1697;
finalMass = 1414;
dM = (finalMass - initMass)/numRows;
for rowIdx = 1:numRows
    timeArray(rowIdx) = data(rowIdx,1);
    controlArray(rowIdx,1:3) = data(rowIdx,9:11)'*0;
    stateArray(rowIdx,1:6) = (Rmat*data(rowIdx,2:7)')'/1000*1.1;
    stateArray(rowIdx,7) = data(rowIdx,8);
end

%% Non-Dimensionalize the guess arrays.


%% Poplulate state info
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
if numControls == 0
    controlGuess = [];
else
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
end

%% Non-Dimensionalize the state guess
for rowIdx = 1:length(stateGuess)
    stateGuess(rowIdx,:) = (config.StateScaleMat*stateGuess(rowIdx,:)')';
end




