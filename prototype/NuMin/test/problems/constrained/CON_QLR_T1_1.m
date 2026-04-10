function [ci,ce,Ji,Je,iGfun,jGvar] = CON_QLR_T1_1(x,varargin)

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

ci1 = x(1) + 3*x(2);
ci2 = 18 - x(1) - 3*x(2);
ci3 = x(1) + x(2);
ci4 = 8 - x(1) - x(2);

Ji1 = [1 3]';
Ji2 = [-1 -3]';
Ji3 = [1 1]';
Ji4 = [-1 -1]';

ci = [ci1 ci2 ci3 ci4]';
Ji = [Ji1, Ji2, Ji3, Ji4];

iGfun = [1 1 2 2 3 3 4 4]';
jGvar = [1 2 1 2 1 2 1 2]';
