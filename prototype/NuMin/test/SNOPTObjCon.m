function [F,G] = SNOPTObjCon(x)

%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

global objname conname

%-----  Call the constraint function to determine constraint dimensions
[ci,ce,Ji,Je] = feval(conname,x);

%-----  Call the objective function  
[f,g] = feval(objname,x);

%-----  Assemble the outputs
F = [f;ce;ci];
G = [g';Je';Ji'];
