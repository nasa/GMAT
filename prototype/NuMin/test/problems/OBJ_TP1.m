function [f,g,iGfun,jGvar] = OBJ_TP1(x,varargin)

f      = 100*(x(2)-x(1)^2)^2+(1.0 - x(1))^2;
g(2,1) = 200*(x(2)-x(1)^2);       
g(1,1) = -2.0*(x(1)*(g(2,1)-1.0)+1.0)  ;

iGfun = [1 1]';
jGvar = [1 2]';
