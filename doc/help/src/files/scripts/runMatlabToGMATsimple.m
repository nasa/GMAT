% Welcome to the runMatlabToGMATsimple sctipt. This script is part of the tutorial titled "Running GMAT scripts from MATLAB".
% This script is designed to build & run the specified script from Matlab.
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
disp(' ')
disp('Go into the SampleMissions folder and duplicate the Ex_TargetHohmannTransfer.script');
disp('and rename the copy to Ex_TargetHohmannTransfer.m');
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
inputDir  = [rootDir,'/SampleMissions'];

% Assign file to open and check to see if it exists
file2Open = 'Ex_TargetHohmannTransfer.m'; % This Matlab script will only work for files with .m extensions
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

% Execute the GMAT script from Matlab
% The Matlab server must be started from GMAT.
OpenGMAT % Open connection to GMAT for the sending of commands
ClearGMAT % Clear all objects currently set in GMAT
run([inputDir,'/',file2Open]); % Load and run GMAT script in Matlab. The run command can only execute Matlab files. 
BuildRunGMAT % Sends all the GMAT lines into GMAT to Build & Run 
WaitForGMAT % Wait for GMAT to finish running

disp(' ');
disp('Script has successfully run.');