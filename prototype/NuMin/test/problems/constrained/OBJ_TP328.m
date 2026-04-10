function [f,g] = OBJ_TP328(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = 0.1*(12 + x(1)^2 + (1 + x(2)^2)/x(1)^2 + (x(1)^2*x(2)^2 + 100)/x(1)^4/x(2)^4);


g(1,1) = 1/5*x(1)-1/5*(1+x(2)^2)/x(1)^3+1/5/x(1)^3/x(2)^2-2/5*(x(1)^2*x(2)^2+100)/x(1)^5/x(2)^4;
g(2,1) = 1/5*x(2)/x(1)^2+1/5/x(1)^2/x(2)^3-2/5*(x(1)^2*x(2)^2+100)/x(1)^4/x(2)^5;

iGfun = [1 1]';
jGvar = [1 2]';


