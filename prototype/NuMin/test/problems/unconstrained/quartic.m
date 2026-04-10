 function [f,g] = quartic(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = (x + 7)*(x + 2)*(x - 3)*(x - 9);

 
g = 4*x^3  - 9*x^2   -2*67*x +   75;