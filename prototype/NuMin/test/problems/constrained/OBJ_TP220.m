function [f,g,iGfun,jGvar] = OBJ_TP220(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f      = x(1);

g(1,1) = 1;
g(2,1) = 0;

iGfun = 1;
jGvar = 1;