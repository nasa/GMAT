function [myMod, gmatStartupPath] = load_gmat(scriptFileName, gmatStartupFile)

% This file must be located in the bin directory of the GMAT installation

% Create a cleanup object to return to initial working directory when this
% cleanupObj is deleted at the end of the function call.
currDir = pwd;
cleanupObj = onCleanup(@() cleanupFun(currDir));

% Change working directory to location of GMAT bin and JARs
scriptPath = strsplit(mfilename('fullpath'),filesep);
if ismac || isunix
    scriptPath{1} = filesep;
end
gmatBinPath = fullfile(scriptPath{1:end-1});
if ismac
    gmatInstallPath = fullfile(scriptPath{1:end-2}); % was 5
else
    gmatInstallPath = fullfile(scriptPath{1:end-2});
end
gmatStartupPath = fullfile(gmatInstallPath, 'bin');
cd(gmatStartupPath);
java.lang.System.gc();

if nargin < 1
    scriptFileName = [];
end
if nargin < 2
    gmatStartupFile = fullfile(gmatStartupPath, 'gmat_startup_file.txt');
end

% Java .jar files to load, along with their corresponding shared library
jars = {'gmat', 'station', 'navigation'};

% Add directories and jars to Matlab's Java classpath if not already loaded
jpath = javaclasspath('-all');

if ~any(strcmp(jpath, gmatBinPath))
    javaaddpath(gmatBinPath);
end

for jar = jars
    jarpath = strcat(gmatBinPath, filesep, jar, '.jar');

    if ~any(strcmp(jpath, jarpath))
        javaaddpath(jarpath);
    end
end
java.lang.System.gc();

% Load shared libraries
% Note that this must be done from a Java program, because running
% java.lang.System.load from Matlab causes the library to be loaded
% in Matlab's own Java classloader. Then it's unavailable for use from
% GMAT's own Java classes.

% Create instance of gmat_matlab_loadlibrary so it can be garbage collected
% to unload the shared library
libraryLoader = gmat_matlab_loadlibrary;
libraryLoader.loadLibrary(jars);
libraryLoader.setGMATPath();

% Setup Moderator
myMod = gmat.Moderator.Instance();
gmat.gmat.Setup(gmatStartupFile);
cd(currDir);

% Make the ROOT_PATH absolute to the GMAT install directory
% This does not apply to the value of ROOT_PATH used in myMod.Initialize(),
% as that uses the ROOT_PATH in the gmatStartupFile. This means that
% objects initialized in myMod.Initialize() use the value of ROOT_PATH
% in the gmatStartupFile
% fileManager = gmat.FileManager.Instance();
% fileManager.SetAbsPathname('ROOT_PATH', gmatInstallPath);

% Set the output directory to be in currDir, and create it if needed
% fileManager.SetAbsPathname('OUTPUT_PATH', fullfile(currDir, 'output'));
% [~,~] = mkdir(currDir, 'output');

% Interpret the GMAT script so we can get its components
if ~isempty(scriptFileName)
    result = gmat.gmat.LoadScript(scriptFileName);
    disp(['Interpret Script Status: ' int2str(result)]);
    
    if result == 0
        
        errorList = myMod.GetScriptInterpreter().GetErrorList();
        errorListCell = cell(errorList.size(),1);
        
        for ii = 1:errorList.size()
            errorListCell{ii} = char(errorList.get(ii-1));
        end
        
        error('Failed to Interpret Script:\n%s', strjoin(errorListCell));
    end
end

end

function [] = cleanupFun(path)
cd(path);
end

