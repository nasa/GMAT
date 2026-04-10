%%  Point to the folder one level above the LowThrust Directory
repoPath = '/home/djc/csalt';

%%  Required Directory Structure 
%
%  repoPath/LowThrust : Contains classes for low thrust modelling and
%  optimization.  Does not contain third party libraries or ephem files
%
%  repoPath/ThirdParty/snopt : Contains snopt mex files
%
%  repoPath/ThirdParty/KSC_DEReader : Contains reader for DE files.  
%
%  repoPath/ThirdParty/EphemFiles : Contains the binary ephem files
%
%  repoPath/ThirdParty/Intlab_V6 : Contains the automatic differentiation
%  library

%%  Configure which folders to include
includeSNOPT    = true();   % Required for low thrust optimization
includeIntLab   = false();   % Required for low thrust optimization
includeDEReader = false();   % Required for low thrust models

%% ========================================================================
%  Users should not need to change anything below this if directory is
%  correctly configured
%% ========================================================================

%%  Add requested paths, (All directories in LowThrust folder are added)
addpath(genpath([repoPath '/lowthrust']));
% SNOPT for optimizer
if includeSNOPT
    addpath(genpath([repoPath '/ThirdParty/ThirdParty/snopt']))
end
%  DE File Reader for ephemerides and the ephem files themselves
if includeDEReader
    addpath(genpath([repoPath '/ThirdParty/ThirdParty/KSC_DEReader']));
    addpath(genpath([repoPath '/ThirdParty/ThirdParty/EphemFiles']));
    init_eph('DE405.dat');
    isDynInitialized = true();
end
%  Int lab for automatic differntiation.
if includeIntLab
    addpath(genpath([repoPath '/ThirdParty/ThirdParty/Intlab_V6']));
end
