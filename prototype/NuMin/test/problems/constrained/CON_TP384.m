function [f,g] = TP384(x,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

B = [3.85e+2,4.7e+2,5.6e+2,5.65e+2,6.45e+2,4.3e+2,4.85e+2,4.55e+2,3.9e+2,8.6e+2]'
A = [1.e+2,9.e+1,7.e+1,2*5.e+1,4.e+1,3.e+1,2.e+1,1.e+1,5.e+0,
     2*1.e+2,5.e+1,0.e+0,1.e+1,0.e+0,6.e+1,3.e+1,7.e+1,1.e+1,
     2*1.e+1,2*0.e+0,7.e+1,5.e+1,3.e+1,4.e+1,1.e+1,5.e+2,5.e+0,
     3.5e+1,5.5e+1,6.5e+1,6.e+1,9.5e+1,9.e+1,2.5e+1,3.5e+1,5.e+0,
     1.e+1,2.e+1,2.5e+1,3.5e+1,4.5e+1,5.e+1,0.e+0,4.e+1,2.5e+1,2.e+1,
     0.e+0,5.e+0,2*1.e+2,4.5e+1,3.5e+1,3.e+1,2.5e+1,6.5e+1,5.e+0,
     2*0.e+0,4.e+1,3.5e+1,0.e+0,1.e+1,5.e+0,1.5e+1,0.e+0,1.e+1,
     2.5e+1,3.5e+1,5.e+1,6.e+1,3.5e+1,6.e+1,2.5e+1,1.e+1,3.e+1,3.5e+1,
     0.e+0,5.5e+1,2*0.e+0,6.5e+1,2*0.e+0,8.e+1,0.e+0,9.5e+1,
     1.e+1,2.5e+1,3.e+1,1.5e+1,5.e+0,4.5e+1,7.e+1,2.e+1,0.e+0,7.e+1,
     5.5e+1,2.e+1,6.e+1,0.e+0,7.5e+1,1.5e+1,2.e+1,3.e+1,2.5e+1,2.e+1,
     5.e+0,0.e+0,1.e+1,7.5e+1,1.e+2,2.e+1,2.5e+1,3.e+1,0.e+0,1.e+1,
     4.5e+1,4.e+1,3.e+1,3.5e+1,7.5e+1,0.e+0,7.e+1,5.e+0,1.5e+1,3.5e+1,
     2.e+1,2.5e+1,0.e+0,3.e+1,1.e+1,5.e+0,1.5e+1,6.5e+1,5.e+1,1.e+1,
     0.e+0,1.e+1,4.e+1,6.5e+1,0.e+0,5.e+0,1.5e+1,2.e+1,5.5e+1,3.e+1];
e = [ 4.86e+2,6.4e+2,7.58e+2,7.76e+2,4.77e+2,7.07e+2,1.75e+2,...
      6.19e+2,6.27e+2,6.14e+2,4.75e+2,3.77e+2,5.24e+2,4.68e+2,5.29e+2];

A = reshape(A,10,15)

for i=1:15
   f = f - c(i))*x(i);
end
for i = 1:15
    g(i,1)=-c(i)
end

    4 DO 10 I=1,10
      SUM=.0e+0
      DO 9 J=1,15
    9 SUM=SUM+(DFLOAT(A(I,J))*X(J)**2)
   10 IF (INDEX1(I)) G(I)=DFLOAT(B(I))-SUM
      RETURN
    5 DO 12 J=1,10
      DO 11 I=1,15
      IF (.NOT.INDEX2(J)) GOTO 12
   11 GG((I-1)*10+J)=-.2e+1*DFLOAT(A(J,I))*X(I)
   12 CONTINUE
      RETURN
      END