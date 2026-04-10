function [J,dJdX] = OBJ_Sample1(X,varargin)

J         = -10*X(1)^2 + 10*X(2)^2 + 4*sin(X(1)*X(2)) - 2*X(1) + X(1)^4;
dJdX(1,1) = -20*X(1) + 4*X(2)*cos(X(1)*X(2))- 2 + 4*X(1)^3;
dJdX(2,1) =  20*X(2) + 4*X(1)*cos(X(1)*X(2));