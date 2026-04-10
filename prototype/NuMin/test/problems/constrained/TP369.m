function Data = TP369()

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Data.x0  = [5000 5000 5000 200 350 150 225 425]';
Data.lb  = [100 1000 1000 10 10 10 10 10]';
Data.ub  = [10000 10000 10000 1000 1000 1000 1000 1000]';
Data.A   = [];
Data.b   = [];
Data.Aeq = [];
Data.beq = [];
Data.xstar = [0.57930657e+3
              0.13599705e+4
              0.51099709e+4
              0.18201769e+3
              0.29560116e+3
              0.21798231e+3
              0.28641653e+3
              0.39560116e+3];
Data.fstar = 7049.2480;