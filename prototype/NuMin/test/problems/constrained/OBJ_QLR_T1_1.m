function [f,g,iGfun,jGvar] = OBJ_QLR_T1_1(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = 2*x(1)^2 + x(2)^2 - 48*x(1) - 40*x(2);
g = [4*x(1) - 48; 2*x(2) - 40];

iGfun = [1 1]';
jGvar = [1 2]';