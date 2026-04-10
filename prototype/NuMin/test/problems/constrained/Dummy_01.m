function Data = Dummy_01()
%  For unit testing OO User function interface.  This is NOT  a real
%  optimization problem
%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

numVars = 4;
Data.x0  = 2*ones(4,1);
Data.lb  = -100*ones(numVars,1);
Data.ub  =  100*ones(numVars,1);
Data.A   = [9 10 11 12;13 14 15 16];
Data.b   = [5;6];
Data.Aeq = [1 2 3 4;5 6 7 8];
Data.beq = [9;10];
Data.xstar = 'N/A';
Data.fstar = 1.91667;