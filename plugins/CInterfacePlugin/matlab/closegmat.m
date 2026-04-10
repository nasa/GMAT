function closegmat()

% CLOSEGMAT GMAT engine shutdown routine
%  closegmat Closes the GMAT interface and unloads the core library from 
%  the workspace. 
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
% Modified: 2011/09/08 Russell Carpenter, NASA GSFC

% Unload the library
fprintf(1,'Unloading libCInterface...\n');
unloadlibrary('libCInterface');

if libisloaded('libCInterface')
   error('Unloading failed\n');
else
   fprintf(1,'Unloaded\n');
end    

% Trash any local copy of gmat_startup_file.txt, unless pwd is the GMAT
% ROOT_PATH specified in the startup file, which would delete the original
% copy.
root_path = findgmatrootpath;
if ~strcmp(root_path,pwd)
    % We're not in the directory used as GMAT's ROOT_PATH
    gsf = which('gmat_startup_file.txt');
    if strcmp(fileparts(gsf),pwd)
        % GMAT's startup file is present in the local directory, and this
        % is not the directory used as GMAT's ROOT_PATH, so the startup
        % file must be a temporary local copy, and it's OK to delete it.
        delete(gsf)
    end
end

end %function
