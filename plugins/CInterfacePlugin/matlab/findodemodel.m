function [retval] = findodemodel(modelname)

% FINDODEMODEL GMAT force model accessor
%  findodemodel(modelname) Locates a GMAT ODE model and sets it at the 
%  model for use in subsequent calls.
%
%   INPUTS
%   VARIABLE        SIZE    	DESCRIPTION (Optional/Default)
%   modelname       (1x1)       (Optional) Name of the desired ODE model. 
%                               If omitted, the first ODE model found is 
%                               selected.
%
%   OUTPUTS
%   retval          (1x1)       The ID for the ODE Model.
%
%   keyword: GMAT Propagator Forces
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
    if strcmp(modelname, '') == 1
        error('Please enter an ODE model name, or omit arguments to find the first model');
    end
else
    modelname = '';
end

% Set the ODE model to the one named in the script (uses the
% first model found)
retval = calllib('libCInterface','FindOdeModel',modelname);
if retval < 0
    error('Unable to locate the ODE model %s.\n', modelname);
end

%if verbosity == 1
%    msg = calllib('libCInterface','getLastMessage');
%    disp(msg);
%end

end     % function
