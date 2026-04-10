%%  Point to the folder one level above the LowThrust Directory
repoPath = cd;

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
includeIPOPT    = true();   % Required for low thrust optimization
includeIntLab   = false();   % Required for low thrust optimization
includeDEReader = false();   % Required for low thrust models

%% ========================================================================
%  Users should not need to change anything below this if directory is
%  correctly configured
%% ========================================================================

%%  Add requested paths, (All directories in LowThrust folder are added)
addpath(genpath([repoPath]));
% SNOPT for optimizer
if includeSNOPT
    %'C:\Users\ACL_FDS2\Documents\LowThrustProject\ThirdParty\snopt'
    addpath(genpath([repoPath '\..\..\ThirdParty\snopt']))
end

if includeIPOPT
    %'C:\Users\ACL_FDS2\Documents\LowThrustProject\ThirdParty\snopt'
    addpath(genpath([repoPath '\..\..\ThirdParty\ipopt']))
end
%  DE File Reader for ephemerides and the ephem files themselves
if includeDEReader
    addpath(genpath([repoPath '/lowthrusttsi/ThirdParty/KSC_DEReader']));
    addpath(genpath([repoPath '/lowthrusttsi/ThirdParty/EphemFiles']));
    init_eph('DE405.dat');
    isDynInitialized = true();
end
%  Int lab for automatic differntiation.
if includeIntLab
    addpath(genpath([repoPath '/ThirdParty/Intlab_V6']));
end






