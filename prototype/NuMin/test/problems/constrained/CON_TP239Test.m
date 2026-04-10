function [ci,ce,Ji,Je,iGfun,jGvar] = CON_TP239(x,varargin) 

ce = [];
Je = [];

ci1 = x(1)*x(2) - 700;
 

Ji1 = [x(2) x(1)]';
 

ci = -[ci1  ]';
Ji = -[Ji1  ];

iGfun = [1 1 ]';
jGvar = [1 2 ]';