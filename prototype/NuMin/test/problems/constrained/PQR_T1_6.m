function Data = PQR_T1_6()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  = [1.1 2.1 3.1 4.1 -1]';
Data.lb  = [1 2 3 4 -inf]'; 
Data.ub  = [];
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [1 2 3 4 2]';
Data.fstar = -1;