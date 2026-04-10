function [ci,ce,Ji,Je,iGfun,jGvar] = CON_SGR_P1_2(x,varargin)

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

ce1 = x(1) + x(2)*exp(-5*x(3)) + x(4) - 127;
ce2 = x(1) + x(2)*exp(-3*x(3)) + x(5) - 151;
ce3 = x(1) + x(2)*exp(-x(3))   + x(6) - 379; 
ce4 = x(1) + x(2)*exp(x(3))    + x(7) - 421; 
ce5 = x(1) + x(2)*exp(3*x(3))  + x(8) - 460; 
ce6 = x(1) + x(2)*exp(5*x(3))  + x(9) - 426;

Je1 = [1 exp(-5*x(3)) -5*x(2)*exp(-5*x(3)) 1 0 0 0 0 0]';
Je2 = [1 exp(-3*x(3)) -3*x(2)*exp(-3*x(3)) 0 1 0 0 0 0]';
Je3 = [1 exp(-x(3))       -x(2)*exp(-x(3)) 0 0 1 0 0 0]';
Je4 = [1 exp(x(3))          x(2)*exp(x(3)) 0 0 0 1 0 0]';
Je5 = [1 exp(3*x(3))    3*x(2)*exp(3*x(3)) 0 0 0 0 1 0]';
Je6 = [1 exp(5*x(3))    5*x(2)*exp(5*x(3)) 0 0 0 0 0 1]';

ce = [ce1 ce2 ce3 ce4 ce5 ce6]';
Je = [Je1, Je2, Je3, Je4, Je5, Je6];

iGfun = [1 1 1 1 2 2 2 2 3 3 3 3 4 4 4 4 5 5 5 5 6 6 6 6]';
jGvar = [1 2 3 4 1 2 3 5 1 2 3 6 1 2 3 7 1 2 3 8 1 2 3 9]';


