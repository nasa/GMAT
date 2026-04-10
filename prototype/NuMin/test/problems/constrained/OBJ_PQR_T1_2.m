function [f,g] = OBJ_PQR_T1_2(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = 100*(x(2) - x(1)^2)^2 + (1 - x(1))^2;
g = [ -400*(x(2) - x(1)^2)*x(1)   - 2*(1 - x(1));
      200*(x(2) - x(1)^2)];
  

   