function [f,g] = OBJ_snoptHex(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

f   = - ( x(2)*x(6) - x(1)*x(7) + x(3)*x(7) + x(5)*x(8) - x(4)*x(9) - x(3)*x(8) );

g   = -[   -x(7)  x(6)  x(7)  - x(8)  -x(9) x(8) x(2) (x(3)-x(1)) ( x(5)  - x(3))  (  - x(4) ) ]';
 