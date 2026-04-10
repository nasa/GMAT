function [f,g] = OBJ_TP210(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f      = 10^6*(x(2) - x(1)^2)^2 + (1 - x(1))^2;

g(1,1) = 20^6*(x(2) - x(1)^2)*(-2*x(1)) - 2*(1 - x(1));
g(2,1) = 20^6*(x(2) - x(1)^2);
