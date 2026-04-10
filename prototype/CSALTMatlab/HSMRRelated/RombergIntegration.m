function [res]=RombergIntegration(func,numFuncs,tInit,tFin,decimalDigits)
%   Numerical evaluation of an integral using the Romberg method.
%   Modified by Youngkwang Kim; youngkwangkim88@gmail.com
%   can handle the vector function but the function must be
%   row-sequential
%   The original file infomation is given as follows:
%
%   Q = rombint('F',A,B) approximates the integral of F(X) from A to B to
%   within a relative error of 1e-8 using Romberg's method of integration.
%   'F' is a string containing the name of the function.  The function
%   must return a vector of output values if a vector of input values is given.
%
%   Q = rombint('F',A,B,DECIMALDIGITS) integrates with accuracy 10^{-DECIMALDIGITS}.
%
%   Q = rombint('F',A,B,DECIMALDIGITS,P1,P2,...) allows coefficients P1, P2, ...
%   to be passed directly to the function F:   G = F(X,P1,P2,...).
%
%   Tested under MATLAB 5.2, works with all subsequent releases.
%   ---------------------------------------------------------
%   Author: Martin Kacenak,
%           Department of Applied Informatics and Process Control,
%           Faculty of BERG, Technical University of Kosice,
%           B.Nemcovej 3, 04200 Kosice, Slovak Republic
%   E-mail: ma.kac@post.cz
%   Date:   posted February 2001, updated June 2006.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if nargin<5, decimalDigits=8; end
if nargin<4
   warning ('Error in input format. At least three input parameters are needed.')
end

rom=zeros(2,decimalDigits);
romall=zeros(1,(2^(decimalDigits-1))+1);   
xVec=tInit:(tFin-tInit)/2^(decimalDigits-1):tFin;
for i=1:numel(romall)    
    romall(i,1:numFuncs)=feval(func,xVec(i));
end
h=tFin-tInit;
res=zeros(1,numFuncs);
for j=1:numFuncs
    rom(1,1)=h*(romall(1)+romall(end))/2;
    for i=2:decimalDigits
      st=2^(decimalDigits-i+1);
      rom(2,1)=(rom(1,1)+h*sum(romall((st/2)+1:st:2^(decimalDigits-1),j)))/2;
      for k=1:i-1
         rom(2,k+1)=((4^k)*rom(2,k)-rom(1,k))/((4^k)-1);
      end
      rom(1,1:i)=rom(2,1:i);
      h=h/2;
    end
    res(j)=rom(1,decimalDigits);
end