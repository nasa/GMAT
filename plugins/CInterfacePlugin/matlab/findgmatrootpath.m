function root_path = findgmatrootpath
% FINDGMATROOTPATH Extracts GMAT's ROOT_PATH from gmat_startup_file.txt

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
% Author: Russell Carpenter, NASA GSFC
% Created: 2011/08/08
% Modified: 2014/11/12 Ravi Mathur, Emergent Space Technologies Inc.
%           Handle cases where ROOT_PATH is a relative path

% Make sure gmat_startup_file.txt can be read
fid = fopen('gmat_startup_file.txt','r');
if fid == -1
    error('GMAT:findgmatrootpath', ...
        ['gmat_startup_file.txt not found in MATLAB path. Ensure you have a valid GMAT ' ...
        'installation and add its appropriate paths to your MATLAB path.']);
else
    % Extract ROOT_PATH value
    gsfl = '';
    while ~strncmp(gsfl,'ROOT_PATH',9)
        gsfl = fgetl(fid);
    end
    fclose(fid);
    ps = strfind(gsfl,'=');
    root_path = strtrim(gsfl(ps+1:end));
    
    % If ROOT_PATH is a relative path, then convert it to absolute
    W = what(root_path);
    if(~strcmp(W.path, root_path))
        gsf_path = fileparts(which('gmat_startup_file.txt'));
        root_path = fullfile(gsf_path, root_path);
    end
    
    % ROOT_PATH is valid if it contains bin/gmat_startup_file.txt
    if ~exist(fullfile(root_path, 'bin', 'gmat_startup_file.txt'))
        error('GMAT:findgmatrootpath', ...
            'The ROOT_PATH variable in gmat_startup_file.txt does not point to a valid GMAT installation.');
    end
end
