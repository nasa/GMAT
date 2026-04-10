function [stateGuess,controlGuess] = MarsBenchmarkLoadGuessFunction(timeVector,timeTypeVec,numControls)
	
% answer = load('MarsBenchmarkNoCostLoFiResults','traj');
answer = load('MarsBenchmarkMaxMassHiFiResults','traj');

stateArray = [];
controlArray = [];
stateTimeArray = [];
cntrlTimeArray = [];

for i = 1:length(answer.traj.phaseList)
	if i == 1
		start = 1;
	else
		start = 2;
	end
	temp_state = answer.traj.phaseList{i}.DecVector.GetStateArray();
	stateArray = [stateArray;temp_state(start:end,:)];
	
	temp_times = answer.traj.phaseList{i}.TransUtil.GetTimeVector();
	stateTimeArray = [stateTimeArray;temp_times(start:end)];
	
	temp_cntrl = answer.traj.phaseList{i}.DecVector.GetControlArray();
	if length(temp_cntrl)
		controlArray = [controlArray;temp_cntrl(start:end,:)];
		cntrlTimeArray = [cntrlTimeArray;temp_times(start:end-1)];
	end
end

% Determine which points have state and control
statePoints = find(timeTypeVec == 1 | timeTypeVec == 2);
controlPoints = find(timeTypeVec == 1 | timeTypeVec == 3);
stateTimes = timeVector(statePoints); %#ok<FNDSB>
controlTimes = timeVector(controlPoints); %#ok<FNDSB>

%  Interpolate the state
for stateIdx = 1:size(stateArray,2)
	%timeArray(2:length(timeArray))-timeArray(1:length(timeArray)-1)
    stateGuess(:,stateIdx) = interp1(stateTimeArray,stateArray(:,stateIdx),stateTimes,'linear','extrap');
end


% Interploate the controls
%  kludge because Radau does not have control at the last point.
if numControls == 0
    controlGuess = [];
else
    if size(controlArray,1) < size(cntrlTimeArray,1);
        for controlIdx = 1:size(controlArray,2)
            controlGuess(:,controlIdx) = interp1(cntrlTimeArray(1:end-1),controlArray(:,controlIdx),controlTimes,'spline','extrap'); %#ok<*AGROW>
        end
    else
        for controlIdx = 1:size(controlArray,2)
            controlGuess(:,controlIdx) = interp1(cntrlTimeArray,controlArray(:,controlIdx),controlTimes,'spline','extrap');
        end
    end
end


    
% if numControls
%     figure(2); hold on
%     plot(controlTimes/config.TU,controlGuess(:,1),'-.')
%     plot(controlTimes/config.TU,controlGuess(:,2),'-.')
%     plot(controlTimes/config.TU,controlGuess(:,3),'-.')
% end

figure(1); hold on;
plot3(stateGuess(:,1),stateGuess(:,2),stateGuess(:,3),'-.');
