function [ci,ce,Ji,Je] = CON_LGR_T1_1(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

ce = [];
Je = [];
ci1 = exp(exp(x(1)));
ci2 = x(2) - exp(exp(x(1)));
Ji1 = [ exp(x(1))*exp(exp(x(1))) , 0]';
Ji2 = [ -exp(x(1))*exp(exp(x(1))), 1]';

ci = [ci1 ci2]';
Ji = [Ji1, Ji2];

