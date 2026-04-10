%$Header: /cygdrive/p/dev/cvs/matlab/gmat_keyword/Request.m,v 1.3 2005/12/22 19:17:12 jgurgan Exp $
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
function data = Request(channel, var)

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to request data
%-----------------------------------------------------------
if strcmp(computer,'PCWIN') || strcmp(computer,'PCWIN64')
   %data = ddereq(gmatChannel, var); % receive as numeric
   data = ddereq(channel, var, [1 1]); % receive as string, default timeout of 3 sec
elseif isunix 
   data = SendGMAT('Request',var);
else
   disp('Request(): unknown platform is not supported at this time');
end
