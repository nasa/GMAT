function [ci,ce,Ji,Je,iGfun,jGvar] = CON_LQR_T1_4(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

ce = x(1)^2 + x(2)^2 + x(3)^2 - 1;
Je = [2*x(1) 2*x(2)  2*x(3)]';

ci = 1 -2*x(2) + x(1);
Ji = [1 -2 0]';
 
iGfun = [1 1 1 2 2]';
jGvar = [1 2 3 1 2]';