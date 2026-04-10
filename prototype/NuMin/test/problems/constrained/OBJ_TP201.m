function [f,g,iGfun,jGvar] = OBJ_TP201(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f      = 4*(x(1) - 5)^2 + (x(2) - 6)^2;

g(1,1) = 8*(x(1) - 5);
g(2,1) = 2*(x(2) - 6);

iGfun = [1 1]';
jGvar = [1 2]';