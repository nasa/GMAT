function [ci,ce,Ji,Je] = CON_SLR_T1_1(x,varargin)

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

ce  = [x(1) + 3*x(2);
       x(3) + x(4) - 2*x(5);
       x(2) - x(5)];
   
J1  = [1 3 0 0 0]';
J2  = [0 0 1 1 -2]';
J3  = [0 1 0 0 -1]';
Je  = [J1, J2, J3];

