function [ci,ce,Ji,Je,iGfun,jGvar] = FMINCON_CON_PQR_P1_4(x,varargin)

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
 
ci = -(48 - x(1)^2 -2*x(2)^2 -4*x(3)^2);
Ji = -[ -2*x(1), - 4*x(2), - 8*x(3)]';

iGfun = [1 1 1]';
jGvar = [1 2 3]';