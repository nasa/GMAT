function [retval] = preparescript(filename);

% STARTGMAT GMAT engine initialization routine
%  [retval] = preparescript(filename) Loads a script into the GMAT 
%  interface and initializes the objects in GMAT, populates GMAT's sandbox 
%  with the configuration described in the script, and runs the script, 
%  forcing GMAT to establish all object-to-object interconnections.
%
%   INPUTS
%   VARIABLE        SIZE    	DESCRIPTION (Optional/Default)
%   filename        (1x1)       GMAT script used to configure and populate
%                               the GMAT sandbox.
%
%   OUTPUTS
%   retval          (1x1)       Integer return code indicating the status
%                               of the GMAT startup process.
%
%   keyword: GMAT Script 
%
%
% (This file is part of GMAT, The General Mission Analysis Tool, and is
%  distributed under the NASA Open Source Agreement.  See file source for
%  more details.)

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
% Created: 2011/08/30

% Load the library
if ~(libisloaded('libCInterface'))
    error('Please load the GMAT interface first.');
end

% Prepare configuration script options
if (nargin >= 1)
    if strcmp(filename, '') == 1
        filename = 'GmatConfig.script';
    end
else
    error('Please enter a script name for the preparescript command');
end

if CheckForFile(filename) == 0
    error('The GMAT configuration script does not exist, so it cannot be loaded');
end

% Load and populate a configuration
retval = calllib('libCInterface', 'LoadScript', filename);
if retval ~= 0
    error('The configuration script "%s" failed to load\n', filename); 
end
retval = calllib('libCInterface','RunScript');
if retval ~= 0
    error('The configuration script "%s" failed to run\n', filename); 
end

%if verbosity == 1
%    % Show the run status
%    msg = calllib('libCInterface','getLastMessage');
%    disp(msg);
%end

end  % function
