function [f,g,iGfun,jGvar] = OBJ_SGR_P1_2(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f = x(4:9)'*x(4:9);
g = [0 0 0 2*x(4) 2*x(5) 2*x(6) 2*x(7) 2*x(8) 2*x(9)]';

iGfun = ones(6,1);
jGvar = [4 5 6 7  8 9]';