function [stateGuess,controlGuess] = DawnEMTGGuessFunction(timeVector,timeTypeVec,numControls,ifPlot)

global config

if ~exist('ifPlot','var')
    ifPlot = 0;
end

%%  Read the file
repoPath = '/Users/jknittel/Collocation/LowThrust/lowthrust';
folderLocation = '/TestProblems/Dawn/';
% fileName = 'CAESAR_sharp1000_2NEXT_80s_FBLT_minT_tight_J1.e';
fileNames = {'SuperDawn_FBLT_J0.e','SuperDawn_FBLT_J1.e','SuperDawn_FBLT_J2.e'};

% if exist([repoPath folderLocation fileName],'file') ~= 2
% 	error('Cant find .e file')
% end

%  Rotation for J2000Eq to J2000Ec
% R = [1.0 0.0 0.0;
%     0.0 0.91748206 -0.397777156;
%     0.0 0.39777716 0.9174820621]';
R = [1.0,0.0,0.0;0.0,1.0,0.0;0.0,0.0,1.0];
Rmat = [R zeros(3,3);zeros(3,3) R];

% timeArray = zeros(numRows,1);
% stateArray = zeros(numRows,7);

%% Redimensionalize the time vector
for rowIdx = 1:length(timeVector)
    timeVector(rowIdx) = timeVector(rowIdx)*config.TU;
end

% disp(config.TimeOffSet)

row_start = 1;
time_offset = 0;

idx_subtract = 0;
for i = 1:length(fileNames) 
	[data,cBody,refFrame,baseEpoch] = ReadEMTGGuessFile([repoPath folderLocation fileNames{i}]);
	date_split = strsplit(baseEpoch);
	day = str2num(date_split{1});
	year = str2num(date_split{3});
	
	switch date_split{2}
		case 'JAN'
			month = 1;
		case 'FEB'
			month = 2;
		case 'MAR'
			month = 3;
		case 'APR'
			month = 4;
		case 'MAY'
			month = 5;
		case 'JUN'
			month = 6;
		case 'JUL'
			month = 7;
		case 'AUG'
			month = 8;
		case 'SEP'
			month = 9;
		case 'OCT'
			month = 10;
		case 'NOV'
			month = 11;
		case 'DEC'
			month = 12;
	end
	
	time_start = date2jed([year,month,day])-2430000.5-config.TimeOffSet;
		
	%% Extract guess data
	numRows = size(data,1);
	
	for rowIdx = row_start:numRows+row_start-1
		if rowIdx > 1 && abs((data(rowIdx-(row_start-1),1)+time_start*86400) - timeArray(rowIdx-1-idx_subtract)) < 1e-12
			% disp(rowIdx)
			idx_subtract = idx_subtract + 1;
			continue
		end
		
	    timeArray(rowIdx-idx_subtract) = (data(rowIdx-(row_start-1),1)+time_start*86400);
		
	    controlArray(rowIdx-idx_subtract,1:3) = data(rowIdx-(row_start-1),9:11)';

	    stateArray(rowIdx-idx_subtract,1:6) = (Rmat*data(rowIdx-(row_start-1),2:7)')'/1000;
	    stateArray(rowIdx-idx_subtract,7) = data(rowIdx-(row_start-1),8);
		
	end
	row_start = row_start + numRows-idx_subtract;
	time_offset = timeArray(end);
end
figure(8)
% plot((timeArray(2:end)-timeArray(1:end-1))/86400)

% plot((timeArray-timeArray(1))/config.TU,controlArray(:,1))
% hold on
% plot((timeArray-timeArray(1))/config.TU,controlArray(:,2))
% plot((timeArray-timeArray(1))/config.TU,controlArray(:,3))

% plot((timeArray-timeArray(1))/86400,controlArray(:,1))
% hold on
% plot((timeArray-timeArray(1))/86400,controlArray(:,2))
% plot((timeArray-timeArray(1))/86400,controlArray(:,3))

% plot((timeArray-timeArray(1))/86400,stateArray(:,1))
% hold on
% plot((timeArray-timeArray(1))/86400,stateArray(:,2))
% plot((timeArray-timeArray(1))/86400,stateArray(:,3))
% stop

%% Poplulate state info
% if timeVector(1) < timeArray(1) || timeVector(end) > timeArray(end)
%     error('Requested time for guess is not contained in the guess data')
% end

% Determine which points have state and control
statePoints = find(timeTypeVec == 1 | timeTypeVec == 2);
controlPoints = find(timeTypeVec == 1 | timeTypeVec == 3);
stateTimes = timeVector(statePoints); %#ok<FNDSB>
controlTimes = timeVector(controlPoints); %#ok<FNDSB>

%  Interpolate the state
for stateIdx = 1:size(stateArray,2)
	%timeArray(2:length(timeArray))-timeArray(1:length(timeArray)-1)
    stateGuess(:,stateIdx) = interp1(timeArray,stateArray(:,stateIdx),stateTimes,'spline','extrap');
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

if ifPlot
    
    if numControls
        figure(2); hold on
        plot(controlTimes/config.TU,controlGuess(:,1),'-.')
        plot(controlTimes/config.TU,controlGuess(:,2),'-.')
        plot(controlTimes/config.TU,controlGuess(:,3),'-.')
    end
    
    figure(4); hold on;
    plot(timeArray/config.TU,stateArray(:,7),'-.');

end

[m,~] = size(stateGuess);
%% Non-Dimensionalize the state guess
for rowIdx = 1:m
    stateGuess(rowIdx,:) = (config.StateScaleMat*stateGuess(rowIdx,:)')';
end




