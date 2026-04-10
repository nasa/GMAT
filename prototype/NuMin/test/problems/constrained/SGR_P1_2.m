function Data = SGR_P1_2()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  = [300 -100 -0.1997 -127 -151 379 421 460 426]';
Data.lb  = -inf*ones(9,1); 
Data.ub  =  inf*ones(9,1);
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [ 523.305537967879
         -156.947841715877
        -0.199664570532337
          29.6080090677228
         -86.6155561079669
          47.3266989102179
          26.2355959277788
          22.9159839031625
          -39.470737476753];
Data.fstar = 13390.0931194796;

