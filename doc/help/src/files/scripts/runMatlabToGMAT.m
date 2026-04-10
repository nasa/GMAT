% Welcome to the runMatlabToGMAT sctipt. This script is part of the tutorial titled "Running GMAT scripts from MATLAB".
% This script is designed to open the specified script into matlab, add a temp variable, use that temp variable to 
% add syntax for additional report text, and build & run that modified script from Matlab.
% 
%  The Matlab Server from GMAT must be started before using this script
%
% REVISION HISTORY
% Author      Date            Comment
%             (MM/DD/YYYY)
% Edwin Dove  09/27/2007      Original
% Edwin Dove  10/22/2007      Modified


% Clear the Matlab workspace and close open plots
clc
clear all
close all

% Brief intro
disp('Welcome to part of the Tutorial "Running GMAT scripts from MATLAB".');
disp('Make sure GMAT is open and the GMAT server has been started then');
disp('press ENTER to continue.');
pause
        
% Obtain the directory of the current Matlab file
format long
tempDir = mfilename('fullpath');
if ispc
    temp = findstr(tempDir,'\');
    matlabDir = tempDir(1:temp(size(temp,2))-1);
else
    temp = findstr(tempDir,'/');
    matlabDir = tempDir(1:temp(size(temp,2))-1);
end
cd(matlabDir);
cd('..');
rootDir  = cd;
inputDir  = [rootDir,'/input/SampleMissions'];

% Assign file to open and check to see if it exists
file2Open = 'Ex_TargetHohmannTransfer.script';
if (exist([inputDir,'/',file2Open]) == 2)
    disp(' ');
    disp('Script loaded successfully.');
else
    disp(' ');
    disp('The following file cannot be found:');
    disp([inputDir,'/',file2Open]);
    disp('Please locate the file you wish to run, edit this script. and try again.');
    return;
end

% Open script file and save text into a cell array
fid = fopen([inputDir,'/',file2Open]);
loadedScript = textscan(fid,'%s','delimiter','\n');
fclose(fid);

% Modify the saved cell array to contain syntax for creating a string object and 
% adding additional report lines at the end of the report file
reportFileLoc = strmatch('Create ReportFile',loadedScript{1,1}); % Find location of Create ReportFile syntax
moddedScript = loadedScript{1,1}(1:reportFileLoc-1,1); % Store the loaded script data up to just before the Create ReportFile syntax
moddedScript{size(moddedScript,1)+1,1} = 'Create String temp;'; % Add Create String syntax
moddedScript{size(moddedScript,1)+1,1} = ''; % Add blank after previously added line
numLinesLeft = size(loadedScript{1,1},1) - (reportFileLoc - 1); % Determine number of lines left to copy from the orignal loaded script file
moddedScript((size(moddedScript,1)+1):(size(moddedScript,1)+numLinesLeft),1) = loadedScript{1,1}(reportFileLoc:size(loadedScript{1,1},1),1); % Add in the remainder of the loaded script lines
moddedScript{size(moddedScript,1)+1,1} = 'GMAT temp = BREAK;'; % Add blank line break to Report syntax
moddedScript{size(moddedScript,1)+1,1} = 'Report Data temp;';
moddedScript{size(moddedScript,1)+1,1} = 'GMAT temp = This report was generated from the runMatlabToGMAT Matlab script;'; % Add unique Matlab line to Report syntax
moddedScript{size(moddedScript,1)+1,1} = 'Report Data temp;';
moddedScript{size(moddedScript,1)+1,1} = ['GMAT temp = Report has successfully been customized;']; % Add time report generated to Report syntax
moddedScript{size(moddedScript,1)+1,1} = 'Report Data temp;';

% Execute the modified GMAT script from Matlab. 
% The Matlab server must be started from GMAT.
OpenGMAT % Open connection to GMAT for the sending of commands
ClearGMAT % Clear all objects currently set in GMAT

for loop = 1:size(moddedScript,1);
    eval(moddedScript{loop,1});
end;
BuildRunGMAT % Sends all the GMAT lines into GMAT to Build & Run 
WaitForGMAT % Wait for GMAT to finish running

disp(' ');
disp('Script has successfully run.');