function [ci,ce,Ji,Je] = fminconstaint(x,varargin)

%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

[ci,ce,Ji,Je] = feval(varargin{1},x);
ci = -ci;
Ji = -Ji;