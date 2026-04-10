function [ci,ce,Ji,Je] = CON_LPR_T1_1(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

ci = [];
Ji = [];
ce1 = x(2) - x(1)^3 - x(3)^2;
ce2 = x(1)^2 - x(2) - x(4)^2;
Je1 = [-3*x(1)^2 1 -2*x(3) 0]'; 
Je2 = [2*x(1) -1 0 -2*x(4)]'; 

ce = [ce1 ce2]';
Je = [Je1 Je2];



