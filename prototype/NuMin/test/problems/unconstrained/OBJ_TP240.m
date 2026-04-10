function [f,g] = OBJ_TP240(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f      = (x(1) - x(2) + x(3))^2 + (-x(1) + x(2) + x(3))^2 + (x(1) + x(2) - x(3))^2;

g(1,1) = 2*(x(1) - x(2) + x(3)) - 2*(-x(1) + x(2) + x(3)) + 2*(x(1) + x(2) - x(3));
g(2,1) = -2*(x(1) - x(2) + x(3)) + 2*(-x(1) + x(2) + x(3)) + 2*(x(1) + x(2) - x(3));
g(3,1) = 2*(x(1) - x(2) + x(3)) + 2*(-x(1) + x(2) + x(3))  + -2*(x(1) + x(2) - x(3));