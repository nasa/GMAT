function [ci,ce,Ji,Je] = CON_QLR_T1_2(x,varargin);

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

A = [-1 -1 -1 -1 -1;
     10 10 -3 5 4;
     -8 1 -2 -5 3;
     8 -1 2 5 -3;
     -4 -2 3 -5 1];
 
b = [5 -20 40 -11 30]';

ci = A*x + b;
Ji = A';

