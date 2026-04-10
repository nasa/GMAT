function [ci,ce,Ji,Je] = CON_TP369(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

ce = [];
Je = [];
 
C = [833.33252e+0,100.e+0,-83333.333e+0,1250.e+0,1.e+0,-1250.e+0...
     ,1250000.e+0,1.e+0,-2500.e+0,2.5e-3,2.5e-3,2.5e-3,2.5e-3,-2.5e-3...
     ,1.e-2,-1.e-2];  

ci(1,1)=1.0-C(10)*x(4)-C(11)*x(6);
ci(2,1)=1.0-C(12)*x(5)-C(13)*x(7)-C(14)*x(4);
ci(3,1)=1.0-C(15)*x(8)-C(16)*x(5);
ci(4,1)=1.0-C(1)/x(1)*x(4)/x(6)-C(2)/x(6)-C(3)/x(1)/x(6);
ci(5,1)=1.0-C(4)/x(2)*x(5)/x(7)-C(5)*x(4)/x(7)-C(6)/x(2)*x(4)/x(7);
ci(6,1)=1.0-C(7)/x(3)/x(8)-C(8)*x(5)/x(8)-C(9)/x(3)*x(5)/x(8);

Ji1 = [0 0 0 -C(10) 0 -C(11) 0 0]';
Ji2 = [0 0 0 -C(14) -C(12) 0 -C(13) 0]';
Ji3 = [0 0 0 0 -C(16) 0 0 -C(15)]';
Ji4 = [C(1)/x(1)^2*x(4)/x(6)+C(3)/x(1)^2/x(6) 0 0 -C(1)/x(1)/x(6) 0 ...
       -(-C(1)/x(1)*x(4)-C(2)-C(3)/x(1))/x(6)^2 0 0]';
   
dci5dx2 = C(4)/x(2)^2*x(5)/x(7)+C(6)/x(2)^2*x(4)/x(7);
dci5dx4 = -C(5)/x(7)-C(6)/x(2)/x(7);
dci5dx5 = -C(4)/x(2)/x(7);
dci5dx7 = C(4)/x(2)*x(5)/x(7)^2+C(5)*x(4)/x(7)^2+C(6)/x(2)*x(4)/x(7)^2;
Ji5 = [0 dci5dx2 0 dci5dx4 dci5dx5 0 dci5dx7 0]';

dci6dx3 =  C(7)/x(3)^2/x(8)+C(9)/x(3)^2*x(5)/x(8);
dci6dx5 = -C(8)/x(8)-C(9)/x(3)/x(8);
dci6dx7 = C(7)/x(3)/x(8)^2+C(8)*x(5)/x(8)^2+C(9)/x(3)*x(5)/x(8)^2;
Ji6 = [0 0 dci6dx3 0 dci6dx5 0 0 dci6dx7 ]';
   
Ji = [Ji1 Ji2 Ji3 Ji4 Ji5 Ji6];
       
    
       
       