function [ci,ce,Ji,Je] = CON_LPR_T1_5(x,varargin)

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
ci1 = -2*x(1)^2 + x(1)^3 + x(2);
ci2 = -2*(1-x(1))^2 + (1-x(1))^3 + x(2);

Ji1 = [-4*x(1) + 3*x(1)^2 , 1]';
Ji2 = [ 4*(1-x(1)) - 3*(1-x(1))^2, 1]';

ci = [ci1 ci2]';
Ji = [ Ji1, Ji2];

