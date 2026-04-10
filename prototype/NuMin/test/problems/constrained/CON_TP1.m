function [ci,ce,Ji,Je,iGfun,jGvar] = CON_TP1(x,varargin);

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

ci = 0;
Ji = 0*x;

iGfun = [1 1]';
jGvar = [1 2]';
