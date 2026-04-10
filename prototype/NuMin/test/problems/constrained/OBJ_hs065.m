function [f,g] = OBJ_hs066(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.
if norm(imag(x)) ~= 0;
    keyboard
end

f =  (x(1) - x(2))^2 + (x(1) + x(2) - 10)^2/9 + (x(3) - 5)^2;

g = [ 2*(x(1) - x(2)) + 2/9*(x(1) + x(2) - 10) ;
     -2*(x(1) - x(2)) + 2/9*(x(1) + x(2) - 10);
      2*(x(3) - 5)];
       