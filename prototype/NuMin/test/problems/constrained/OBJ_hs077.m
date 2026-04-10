function [f,g] = OBJ_hs077(x,varargin)

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

f =  (x(1)-1)^2 + (x(1) - x(2))^2 + (x(3)-1)^2 + (x(4)-1)^4 + (x(5)-1)^6;

g = [ 2*(x(1)-1) + 2*(x(1) - x(2)) ;
     -2*(x(1) - x(2)) ;
      2*(x(3)-1);
      4*(x(4)-1)^3;
      6*(x(5)-1)^5];
       