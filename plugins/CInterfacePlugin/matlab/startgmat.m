function [odeId] = startgmat(filename, odemodelname)

% STARTGMAT GMAT engine initialization routine
%  [odeId] = startgmat() Loads the GMAT interface and core library, 
%  initializes the objects in GMAT using the default script
%  (GmatConfig.script), populates GMAT's sandbox with the configuration
%  described in the script, runs the script, forcing GMAT to establish all
%  object-to-object interconnections, and then locates the first ODE model 
%  in the Mission Control Sequence for use by ODTBX.
%
%  [odeId] = startgmat(filename) Loads the GMAT interface and core 
%  library, initializes the objects in GMAT using the specified script, 
%  populates GMAT's sandbox with the configuration described in the script, 
%  runs the script, forcing GMAT to establish all object-to-object 
%  interconnections, and then locates the first ODE model in the Mission 
%  Control Sequence for use by ODTBX.
% 
%  [odeId] = startgmat(filename, odemodelname) Loads the GMAT interface and core 
%  library, initializes the objects in GMAT using the specified script, 
%  populates GMAT's sandbox with the configuration described in the script, 
%  runs the script, forcing GMAT to establish all object-to-object 
%  interconnections, and then locates the named ODE model in the Mission 
%  Control Sequence for use by ODTBX.
%
%   INPUTS
%   VARIABLE        SIZE    	DESCRIPTION (Optional/Default)
%   filename        (1x1)       (Optional) GMAT script used to configure
%                               and populate the GMAT sandbox.
%   odemodelname    (1x1)       (Optional) Name of the desired ODE model. 
%                               If omitted, the first ODE model found is 
%                               selected.
%
%   OUTPUTS
%   odeId           (1x1)       The ID for the first ODE model in the GMAT
%                               script.
%
%   keyword: GMAT Integrators Forces 
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
% Created: 2011/05/17
% Modified: 2011/09/07, Russell Carpenter, NASA GSFC

% Prepare configuration script options
if (nargin >= 1)
    if strcmp(filename, '') == 1
        filename = 'GmatConfig.script';
    end
else
    filename = 'GmatConfig.script';
end

if CheckForFile(filename) == 0
    error('The GMAT configuration script does not exist, so GMAT cannot be loaded');
end


% Load the library and start the GMAT engine
opengmat();

% Load and populate a configuration
preparescript(filename);

% Show the run status
ode = calllib('libCInterface','getLastMessage');
disp(ode);

% Set the ODE model to the one named in the script (uses the
% first model found by default)
if exist('odemodelname', 'var')
    odeId = findodemodel(odemodelname);
else
    odeId = findodemodel();
end

ode = calllib('libCInterface','getLastMessage');
disp(ode);

end %function
