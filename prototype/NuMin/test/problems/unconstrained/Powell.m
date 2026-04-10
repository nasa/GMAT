function [f, g] = Powell(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

fv = [ x(1) + 10*x(2);
       sqrt(5)*(x(3) - x(4));
       (x(2) - 2*x(3))^2;
       sqrt(10)*(x(1) - x(4))^2];
   
f = fv'*fv;
dfvdx1 = [ 1 0 0  2*sqrt(10)*(x(1) - x(4))];
dfvdx2 = [ 10  0 2*(x(2) - 2*x(3))  0];
dfvdx3 = [ 0 sqrt(5)  -4*(x(2) - 2*x(3)) 0];
dfvdx4 = [0 -sqrt(5) 0 -2*sqrt(10)*(x(1) - x(4))];
dfvdX = [dfvdx1;dfvdx2;dfvdx3;dfvdx4]';
g = ( 2*fv'*dfvdX )';
       