function [f,g] = OBJ_TP242(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

g = zeros(3,1);
f = 0;
for i = 1:10
    ti  = 0.1*i;
    fac = ( exp(-x(1)*ti) - exp(-x(2)*ti) ) - x(3)*( exp(-ti) - exp(-10*ti)  );
    f   = f + fac^2;
    g1  = -2*ti*exp(-x(1)*ti)*fac;
    g2  =  2*ti*exp(-x(2)*ti)*fac;
    g3  = -2*( exp(-ti) - exp(-10*ti))*fac;
    g   =  g + [g1 g2 g3]';
end

