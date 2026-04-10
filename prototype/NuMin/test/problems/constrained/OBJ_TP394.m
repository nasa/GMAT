function [f,g,iGfun,jGvar] = OBJ_TP394(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = 0;
g = zeros(20,1);
for i = 1:20
    f = f + i*(x(i,1)^2 + x(i,1)^4);
    g(i,1) = i*(2*x(i,1) + 4*x(i,1)^3);
end

iGfun = ones(20,1);
jGvar = (1:20)';