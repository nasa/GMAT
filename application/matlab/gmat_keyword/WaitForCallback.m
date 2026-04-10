%$Header$
%------------------------------------------------------------------------------
%                               WaitForCallback.m
%------------------------------------------------------------------------------
% GMAT: Goddard Mission Analysis Tool
%
% ** Legal **
%
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
%
% Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
% number S-67573-G
%
% Author: Wendy Shoan
% Created: 2006.08.24
%
% Matlab function to wait for a callback to GMAT to complete.
%
%------------------------------------------------------------------------------
function WaitForCallback()

global gmatChannel;
global gmatCallbackStatus;
if (~isunix && gmatChannel == 0) 
   disp('channel is not valid');
else
    var = 'CallbackStatus';
%   if isunix
%      pause(5);
%   else
%      pause(1);
%   end

   continueWait = true;
   
   while(continueWait)
      
      gmatCallbackStatus = Request(gmatChannel, var)
      
      if (isfloat(gmatCallbackStatus))
         %% Skip Close/Open to stay with the same channel
         %%CloseGMAT;
         %%OpenGMAT;
         continueWait = true;
      elseif(ischar(gmatCallbackStatus))
         if (strcmp(gmatCallbackStatus,'Completed'))
            continueWait = false;
         else
            continueWait = true;
         end
      end
      
%      if isunix
%         pause(5);
%      else
%         pause(1);
%      end 

   end
end
