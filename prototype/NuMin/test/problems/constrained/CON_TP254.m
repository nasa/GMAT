function [ci,ce,Ji,Je] = CON_TP254(x,varargin)

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
ce1 = x(2)^2 + x(3)^2 -4;
ce2 = x(3) - 1 - x(1)^2;

Je1 = [0 2*x(2) 2*x(3)]';
Je2 = [-2*x(1) 0 1]';

ce = [ce1 ce2]';
Je = [ Je1, Je2];