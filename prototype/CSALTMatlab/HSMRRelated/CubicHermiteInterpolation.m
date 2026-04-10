function [yValueVec, yRateVec]=CubicHermiteInterpolation(intvVec,h,yValue0,yRate0,yValue1,yRate1)
% mesh interval is assumed to be [0,h] without loss of the generality
Coeff=[1 0 0 0;
    0 1 0 0;
    -3/h^2 -2/h 3/h^2 -1/h;
    2/h^3 1/h^2 -2/h^3 1/h^2]*[yValue0;yRate0;yValue1;yRate1];

yValueVec=Coeff(4)*intvVec.^3+Coeff(3)*intvVec.^2+Coeff(2)*intvVec+Coeff(1);
yRateVec=3*Coeff(4)*intvVec.^2+2*Coeff(3)*intvVec+Coeff(2);
end