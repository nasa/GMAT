function [f,g] = NandW4p3(x,varargin)
    
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

lengthX = size(x,1);

f = 0;

for i =1:lengthX;

  f = f + (1 - x(2*i-,1))^2  + 10*(x(2*i,1) - x(2*i -1 )^2)^2;
 
end