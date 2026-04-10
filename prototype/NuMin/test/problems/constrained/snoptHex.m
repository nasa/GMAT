function Data = snoptHex()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  =  [ .1;
         .125;
         .666666;
         .142857;
         .111111;
         .2;
         .25;
        -.2;
        -.25 ];
Data.lb  = [0 -inf -1 -inf 0 0 0 -inf  inf]';
Data.ub  = [inf inf 1 inf inf inf inf 0 0]';
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = NaN*Data.x0;
Data.fstar = NaN;


