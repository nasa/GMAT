function [f,g] = Schaum11p3_n2(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f =  sin(x(1)*x(2)) - cos( x(1) - x(2) );
g(1,1) = x(2)*cos(x(1)*x(2)) + sin(  x(1) - x(2) );
g(2,1) = x(1)*cos(x(1)*x(2)) - sin(  x(1) - x(2) );