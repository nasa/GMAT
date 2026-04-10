function [f,g,iGfun,jGvar] = OBJ_PLR_T1_4(x,varargin)

f = -x(1)*x(2)*x(3);
g = [-x(2)*x(3); -x(1)*x(3); -x(1)*x(2)];

iGfun = [1 1 1]';
jGvar = [1 2 3]';