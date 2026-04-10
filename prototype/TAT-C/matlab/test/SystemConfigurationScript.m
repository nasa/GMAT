%% This script configures the MATLAB path.  If you path is not correctly configured to run the
%  O-C system, you must run this before executing unit tests and or API drivers.

% installDir is the directory of the Git repo (i.e. the folder that
% contains "src" and "test" folders etc.
installDir = 'C:/Users/sphughe1/Documents/GMATRepos/gmat-public/prototype/TAT-C/matlab';
tatCFilePaths = genpath(installDir);
addpath(tatCFilePaths);

% Clean up the workspace
clear installDir tatCFilePaths