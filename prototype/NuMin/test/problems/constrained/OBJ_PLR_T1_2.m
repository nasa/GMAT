function [f,g] = OBJ_PLR_T1_2(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = -(9 -(x(1) - 3)^2)  *  x(2)^3/(27*sqrt(3));
g = [2*(x(1) - 3)*x(2)^3/(27*sqrt(3));
    -(9 -(x(1) - 3)^2)*3*x(2)^2/(27*sqrt(3))];
    