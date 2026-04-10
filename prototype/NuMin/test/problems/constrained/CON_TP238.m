function [ci,ce,Ji,Je,iGfun,jGvar] = CON_TP238(x,varargin) 

ce = [];
Je = [];

ci1 = x(1)*x(2) - 700;
ci2 = x(2) - x(1)^2/5;
ci3 = (x(2) - 50)^2 - 5*(x(1) - 55);

Ji1 = [x(2) x(1)]';
Ji2 = [-2*x(1)/5 1]';
Ji3 = [-5 2*(x(2) - 50)]';

ci = [ci1 ci2 ci3]';
Ji = [Ji1 Ji2 Ji3];

iGfun = [1 1 2 2 3 3]';
jGvar = [1 2 1 2 1 2]';
