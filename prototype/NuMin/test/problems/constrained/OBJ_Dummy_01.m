function [f,g,iGfun,jGvar] = OBJ_Dummy_01(x,varargin)

%  For unit testing OO User function interface.  This is NOT  a real
%  optimization problem
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

A = [100 200 300 400];
f = A*x;
g = A;

iGfun = [];
jGvar = [];