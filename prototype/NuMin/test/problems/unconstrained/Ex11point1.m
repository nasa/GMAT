function f = Ex11point1(x,c)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

A = [-3.933   0.107   0.126   0    -9.99   0    -45.83 -7.64;
       0     -0.987   0     -22.95   0   -28.37   0      0 ;
       0.002   0    -.235    0      5.67   0   -.921  -6.51;
       0      1.0     0     -1       0   -0.168   0      0;
       0       0     -1      0     -.196    0   -.0071   0];
   
Phi = [ -.727*x(2)*x(3) + 8.39*x(3)*x(4) - 684.4*x(4)*x(5) + 63.5*x(4)*x(2);
         0.949*x(1)*x(3) + .173*x(1)*x(5);
         -.716*x(1)*x(2) - 1.578*x(1)*x(4) + 1.132*x(4)*x(2);
         -x(1)*x(5);
          x(1)*x(4)];
      
f = A*[x;c] + Phi;      
   
   