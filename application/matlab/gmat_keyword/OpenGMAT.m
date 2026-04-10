% Copyright (c) 2002-2025 United States Government as represented by the
% Administrator of the National Aeronautics and Space Administration.
% All Rights Reserved.
%
% Licensed under the Apache License, Version 2.0 (the "License");
% You may not use this file except in compliance with the License.
% You may obtain a copy of the License at:
% http:%www.apache.org/licenses/LICENSE-2.0
% Unless required by applicable law or agreed to in writing, software
% distributed under the License is distributed on an "AS IS" BASIS,
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
% express or implied.   See the License for the specific language
% governing permissions and limitations under the License.
function OpenGMAT()


global gmatService;
global gmatTopic;
global gmatChannel;
global isInitialized;
if isempty(isInitialized)
    isInitialized = false;
end
gmatService = '4242';
gmatTopic = 'GMAT-MATLAB';

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to initialize
% conversation with GMAT
%-----------------------------------------------------------
%if strcmp(computer,'PCWIN')
if ispc
    if ~isInitialized
        gmatChannel = ddeinit(gmatService, gmatTopic);
    end
else
    %   disp('OpenGMAT(): Only PC windows is supported at this time');
    CallGMAT('Open','');
    return;
end

if (gmatChannel == 0)
    disp('GMAT Server has not started. Please start the server first!');
else
    CallGMAT('Open', '');
end

isInitialized = true;
