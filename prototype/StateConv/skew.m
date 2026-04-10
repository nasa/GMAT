% This function takes a 3X1 column vector and turns it into a
% 3X3 skew symmetric matrix

function [V]= skew(v)

V = [0 -v(3) v(2);v(3) 0 -v(1);-v(2) v(1) 0];                  