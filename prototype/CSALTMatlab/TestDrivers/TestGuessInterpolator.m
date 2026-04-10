%% NOTE: PATHS BELOW ASSUME YOU ARE RUNNING THIS FROM THE "lowthrust" directory



%% An example using data provided by user in arrays
import userfunutils.TrajectoryData
guessGen = TrajectoryData();

% Create the arrays of state and control guess data and set them
timeArray = [0 75 155]';
stateArray = [0 0 0 0;
             12e5/2 12e5/2 7000/2 5000
             12e5 12e5 7000 0]/1000; 
         
controlAngleArray = [pi/2;0;-1];
controlArray(:,1) = cos(controlAngleArray);
controlArray(:,2) = sin(controlAngleArray);
guessGen = guessGen.SetControlGuessArrays(timeArray,controlArray);
guessGen = guessGen.SetStateGuessArrays(timeArray,stateArray);

% Interpolate the guess data
controlCollocationTimes = [1:5:156]';
controlValues = guessGen.GetControl(controlCollocationTimes)
stateCollocationTimes = [-4:5:200]';
stateValues = guessGen.GetState(stateCollocationTimes)

%% An example using data provided by user in a CCSDS ephem
import userfunutils.CCSDSTrajectoryData;
guessGen = CCSDSTrajectoryData('./TestDrivers/CCSDSEphemFile.oem');
stateCollocationTimes = [730486.49962963:0.01:730486.777407407]';;
stateValues = guessGen.GetState(stateCollocationTimes)
controlValues = guessGen.GetControl(stateCollocationTimes)

%% An example using data provided by user in an "och" history
import userfunutils.OCHTrajectoryData;
ocData = OCHTrajectoryData('./TestDrivers/OCHistoryFileExample.och');
stateCollocationTimes = [21545.00069444444:0.01:21545.11518363824]';
stateValues = ocData.GetState(stateCollocationTimes)
controlValues = ocData.GetControl(stateCollocationTimes)
%  pretend some optimization happened here that changed the data.  
%  TODO: provide a clean Set interface for this
ocData.fileUtility.Segments{1}.Data(:,2) = ocData.fileUtility.Segments{1}.Data(:,2) + 1.2;
%  Write solution to file
ocData.WriteData('./TestDrivers/OptimalSolution.och')

