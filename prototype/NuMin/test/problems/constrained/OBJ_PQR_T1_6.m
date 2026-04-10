function [f,g] = OBJ_PQR_T1_6(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

x1 = x(1); x2 = x(2); x3 = x(3); x4 = x(4); x5 = x(5);

f = x1*x2*x3*x4 - 3*x1*x2*x4 - 4*x1*x2*x3 +12*x1*x2 - x2*x3*x4 + 3*x2*x4 +...
    4*x2*x3 -12*x2 -2*x1*x3*x4 + 6*x1*x4 +8*x1*x3 - 24*x1 + 2*x3*x4 -...
    6*x4 -8*x3 +24 + 1.5*x5^4 - 5.75*x5^3 + 5.25*x5^2;

g1 = x2*x3*x4 - 3*x2*x4 - 4*x2*x3 +12*x2  -2*x3*x4 + 6*x4 +8*x3 - 24;
g2 = x1*x3*x4 - 3*x1*x4 - 4*x1*x3 +12*x1 - x3*x4 + 3*x4 + 4*x3 -12  ;
g3 = x1*x2*x4 - 4*x1*x2 - x2*x4 + 4*x2 - 2*x1*x4 + 8*x1 + 2*x4 - 8;
g4 = x1*x2*x3  - 3*x1*x2  - x2*x3 + 3*x2 - 2*x1*x3 + 6*x1 + 2*x3 - 6;
g5 = 4*1.5*x5^3 - 3*5.75*x5^2 + 2*5.25*x5;   

g = [g1 g2 g3 g4 g5]';
     