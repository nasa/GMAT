function [ci,ce,Ji,Je,iGfun,jGvar] = CON_PLR_T1_4(x,varargin)

ce = [];
Je = [];
 
ci = 72 - x(1) - 2*x(2)  - 2*x(3) ;
Ji = [ -1 -2 -2]';

iGfun = [1 1 1]';
jGvar = [1 2 3]';