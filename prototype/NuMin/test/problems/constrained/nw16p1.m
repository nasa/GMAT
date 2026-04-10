function [f,g] = nw16p1(x)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

d = [2 3]';
G = [ 8 2 ; 2 2 ];
f = 0.5*x'*G'*x + d'*x;
g = G*x + d;
