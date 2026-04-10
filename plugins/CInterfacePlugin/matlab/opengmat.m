function [retval] = opengmat

% OPENGMAT GMAT engine startup routine
%  [retval] = opengmat() Loads the GMAT interface and core library 
%
%   OUTPUTS
%   retval          (1x1)       Integer return code indicating the status
%                               of the GMAT startup process.
%
%   keyword: GMAT 
%
%
% (This file is part of GMAT, The General Mission Analysis Tool, and is
%  distributed under the NASA Open Source Agreement.  See file source for
%  more details.)
%
% See also: closegmat

% GMAT: General Mission Analysis Tool
%
% Copyright (c) 2002-2026 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Rights Reserved.
%
% Licensed under the Apache License, Version 2.0 (the "License"); 
% You may not use this file except in compliance with the License. 
% You may obtain a copy of the License at:
% http://www.apache.org/licenses/LICENSE-2.0 
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS,
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
% express or implied.   See the License for the specific language
% governing permissions and limitations under the License.
%
% Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA
% Prime Contract NNG10CP02C, Task Order 28.
%
% Author: Darrel J. Conway, Thinking Systems, Inc.
% Created: 2011/05/17
% Modified: 2011/09/08 Russell Carpenter, NASA GSFC
% Modified: 2012/11/28 Ravi Mathur, Emergent Space Technologies, Inc.

% Note that this requires gmat_startup_file's ROOT_PATH to be
% specified as a full absolute path (not a relative path).

% Check that ROOT_PATH of startup file is a valid GMAT installation
root_path = findgmatrootpath;
    
% Copy startup file to local directory if needed
gsf = which('gmat_startup_file.txt');
copygsf = ~strcmp(fileparts(gsf), pwd);
if copygsf
    disp('Local GMAT startup file missing ... copying global GMAT startup file:');
    disp(gsf);
    copyfile(gsf,'.');
end
    
% Load the CInterface library if needed
if ~(libisloaded('libCInterface'))
    [notfound, warnings] = loadlibrary('libCInterface', @interfacewrapper);
    
    if (~isempty(notfound) || ~libisloaded('libCInterface'))
        if(copygsf)
            delete('gmat_startup_file.txt'); % Delete if it was created
        end
        unloadlibrary('libCInterface');
        error('Unable to load the libCInterface library. Warnings were: %s\n', warnings);
    end
end

% Start the GMAT engine
retval = calllib('libCInterface', 'StartGmat');
msg = calllib('libCInterface', 'getLastMessage');
if retval ~= 0
    if(copygsf)
        delete('gmat_startup_file.txt'); % Delete if it was created
    end
    unloadlibrary('libCInterface');
    disp(['Error from GMAT: ', msg]);
    error('GMAT failed to start.  This may be caused by missing or misconfigured files; check gmat_startup_file.txt.\n');
end

% Success!
disp(['Message from GMAT: ', msg]);
    
end % function
