function [ci,ce,Ji,Je,iGfun,jGvar] = CON_hs077(x,varargin)

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

ce(1,1) = x(1)^2*x(4) + sin(x(4)-x(5)) - 2*sqrt(2) ;
ce(2,1) = x(2) + x(3)^4*x(4)^2 - 8 - sqrt(2);

Je(1,1) = 2*x(1)*x(4);
Je(2,1) = 0;
Je(3,1) = 0;
Je(4,1) = x(1)^2 + cos(x(4)-x(5));
Je(5,1) = -cos(x(4)-x(5));

Je(1,2) = 0;
Je(2,2) = 1;
Je(3,2) = 4*x(3)^3*x(4)^2;
Je(4,2) = 2*x(3)^4*x(4);
Je(5,2) = 0;

iGfun = [];
jGvar = [];