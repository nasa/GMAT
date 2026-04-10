function Data = TP328()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  =  [.5 .5]';
Data.lb  =  [1 1]';
Data.ub  =  [3 3]';
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [1.743452071 2.029694651]';
Data.fstar = 1.7441520055;