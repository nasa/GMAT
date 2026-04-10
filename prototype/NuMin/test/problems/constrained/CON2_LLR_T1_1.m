function [ci,ce,Ji,Je] = CON_LLR_T1_1(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

g1 = 10 - x(1) - x(2) - x(3) - x(4);
g2 = 10 - 0.2*x(1) - 0.5*x(2) - x(3) -2*x(4);
g3 = 10 - 2*x(1) - x(2) - 0.5*x(3) - 0.2*x(4);
g4 = x(1) + x(2) + x(3) -2*x(4) - 6;

J1 = [-1 -1 -1 -1]';
J2 = [-0.2 -0.5 -1 -2]';
J3 = [-2 -1 -.5 -.2]';
J4 = [1 1 1 -2]';

Je = J4;
ce = g4;

c4 = eye(4)*x;
Ji = [eye(4) J1 J2 J3 ];
ci = [c4;g1;g2;g3];



