function [f,g,iGfun,jGvar] = OBJ_TP246(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f      =  100*( x(3) - ((x(1) + x(2))/2)^2)^2 + (1 - x(1))^2 + (1 - x(2))^2;

g(1,1) = 200*(x(3)-(1/2*x(1)+1/2*x(2))^2)*(-1/2*x(1)-1/2*x(2))-2+2*x(1);
g(2,1) = 200*(x(3)-(1/2*x(1)+1/2*x(2))^2)*(-1/2*x(1)-1/2*x(2))-2+2*x(2);
g(3,1) = 200*x(3)-200*(1/2*x(1)+1/2*x(2))^2;

iGfun = [1 1 1]';
jGvar = [1 2 3]';