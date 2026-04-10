function [ci,ce,Ji,Je] = CON_PLR_T1_2(x,varargin)

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

sqrt3 = sqrt(3);
ci1 = x(1)/sqrt3 - x(2);
ci2 = x(2) + sqrt3*x(2);
ci3 = 6 - x(1) - sqrt3*x(2);

Ji1 = [1/sqrt3 -1]';
Ji2 = [0 (1 + sqrt3)]';
Ji3 = [-1 -sqrt3]';

ci = [ci1 ci2 ci3]';
Ji = [Ji1 Ji2 Ji3];
