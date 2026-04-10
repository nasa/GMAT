function [f,g] = OBJ_QLR_T1_2(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

D = [-74 80 18 -11 -4;
      14 -69 21 28 0;
      66 -72 -5 7 1;
      -12 66 -30 -23 3;
      3 8 -7 -4 1;
      4 -12 4 4 0 ];
d = [51 -61 -56 69 10 -12]';

f = x'*D'*D*x -2*d'*D*x + d'*d;
g = (2*x'*D'*D -2*d'*D)';
  
  