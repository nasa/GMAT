function [ci,ce,Ji,Je,iGfun,jGvar] = CON_TP220(x,varargin)

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
 
ci = [ x(2) - exp(x(1));  x(3) - exp(x(2))];
    
Ji(1,1) = -exp(x(1));
Ji(2,1) = 1;
Ji(3,1) = 0;
Ji(1,2) = 0;
Ji(2,2) = -exp(x(2));
Ji(3,2) = 1;


iGfun = [1 1]';
jGvar = [1 2]';