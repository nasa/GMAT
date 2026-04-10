%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_keyword/Poke.m,v 1.1 2007/08/22 19:15:16 shughes Exp $
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
function Poke(channel, dataType, dataStr)

%-----------------------------------------------------------
% if computer is 'PCWIN' call dde function to send string data
%-----------------------------------------------------------
%if (computer == 'PCWIN')
if ispc
   ddepoke(channel, dataType, dataStr);
else
   disp('Poke(): Only PC windows is supported at this time');
end
