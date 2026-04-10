function Data = TP319()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

% Won't converge starting from here:
Data.x0  = [0 0]';
% Will converge starting from here:
%Data.x0  = [1 1]';
Data.lb  = -inf*ones(2,1);
Data.ub  =  inf*ones(2,1);
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [8.492]';
Data.fstar = 452.404;