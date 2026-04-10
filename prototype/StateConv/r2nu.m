function [nu1,nu2] = r2nu(r,a,e)

nu1 = acos(a/e/r*(1-e^2) - 1/e);
nu2 = 2*pi - nu1;
