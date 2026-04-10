function [f,g] = nwEx16p3(x)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

d = [-8 -3 -3]';
G = [6 2 1; 2 5 2;1 2 4];
f = 0.5*x'*G'*x + d'*x;
g = G*x + d;