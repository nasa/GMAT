function [f,g] = OBJ_LLR_T1_1(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = -.5*x(1) - x(2) - .5*x(3) - x(4);
g = [-.5 -1 -.5 -1]';