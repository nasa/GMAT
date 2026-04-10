function [ci,ce,Ji,Je,iGfun,jGvar] = CON_TP319(x,varargin)

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

ce = x(1)^2/100 + x(2)^2/16 - 1;
Je(1,1) = 2*x(1)/100;
Je(2,1) = 2*x(2)/16;

% ce = [];
% Je = [];

iGfun = [1 1]';
jGvar = [1 2]';
