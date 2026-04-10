function [ci,ce,Ji,Je] = CON_TP225(x,varargin)

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

ci(1,1) = x(1)+ x(2) - 1.0;
ci(2,1) = x(1)^2 + x(2)^2-1.0;
ci(3,1) = 9.0*x(1)^2 + x(2)^2 - 9;
ci(4,1) = x(1)^2 - x(2);
ci(5,1) = x(2)^2 - x(1);

Ji(:,1) = [1 1]';
Ji(:,2) = [2*x(1) 2*x(2)]';
Ji(:,3) = [18*x(1) 2*x(2)]';
Ji(:,4) = [2*x(1) -1]';
Ji(:,5) = [-1   2*x(2)]';
