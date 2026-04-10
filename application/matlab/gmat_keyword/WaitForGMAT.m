%$Header: /cygdrive/p/dev/cvs/matlab/gmat_keyword/WaitForGMAT.m,v 1.3 2006/08/28 17:38:29 wshoan Exp $
%------------------------------------------------------------------------------
%                               WaitForGMAT.m
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
% Author: Linda Jun
% Created: 2005/08/02
%
% Matlab function to wait for GMAT run completion.
%
%------------------------------------------------------------------------------
function WaitForGMAT()

global gmatChannel;
global gmatRunState;

% if (gmatChannel == 0) 
if (~isunix && gmatChannel == 0) 
   disp('channel is not valid');
else
   var = 'RunState';
   if isunix
      pause(5);
   else
      pause(1);
   end
   
   continueWait = true;
   
   while(continueWait)
      
      gmatRunState = Request(gmatChannel, var)
      
      %% On computer PICWIN:
      %% Close/Open to get new channel if String value was not received from Request().
      %% It could mean the Server is not responding to new Request so it has timed out.
      %% This happens when building and running a long script.
      if (isfloat(gmatRunState))
       if (~isunix)
           CloseGMAT;
           OpenGMAT;
        end
         continueWait = true;
      elseif(ischar(gmatRunState))
         if (strcmp(gmatRunState,'Idle'))
            continueWait = false;
         else
            continueWait = true;
         end
      end
      
      if isunix
         pause(5);
      else
         pause(1);
      end 

   end
end
