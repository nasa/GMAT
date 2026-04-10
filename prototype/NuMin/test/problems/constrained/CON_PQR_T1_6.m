function [ci,ce,Ji,Je] = CON_PQR_T1_6(x,varargin)

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

ci = 34 - x(1)^2 - x(2)^2 - x(3)^2 - x(4)^2 - x(5)^2;
Ji = [-2*x(1) -2*x(2) -2*x(3) -2*x(4) -2*x(5)]';