function [f, g] = HelicalValley(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

if x(1) >= 0
    theta = 0;
else
    theta = 5;
end

fv = [ 10*(x(3) - 5/pi*atan(x(2)/x(1)) - theta );
       10*(sqrt(x(1)^2 + x(2)^2) - 1);
       x(3)];
f = fv'*fv;


dfvdx1 = [50/pi/( 1 + x(2)/x(1) )*x(2)/x(1)^2  10/sqrt(x(1)^2 + x(2)^2)*x(1) 0];
dfvdx2 = [-50/pi/(1 + x(2)/x(1))/x(1) 10/sqrt(x(1)^2 + x(2)^2)*x(2) 0];
dfvdx3 = [10 0 1];
dfvdX = [dfvdx1;dfvdx2;dfvdx3]';
g = ( 2*fv'*dfvdX )';