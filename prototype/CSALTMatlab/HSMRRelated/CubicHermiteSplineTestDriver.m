clear;
clc;
close all;
func=@(x) erf(x);
funcRate=@(x) (2*exp(-x.^2))/pi^(1/2);

meshPoints=linspace(0,2,10);

yVec=[func(meshPoints);
    2*func(meshPoints)];
yRateVec=[funcRate(meshPoints);
    2*funcRate(meshPoints)];

newMeshPoints=linspace(0,2,15);
[yValuePrimeArray, yRatePrimeArray] = CubicHermiteSpline ...
    (meshPoints,yVec,yRateVec,newMeshPoints);

figure;plot(newMeshPoints, yValuePrimeArray)

figure;plot(newMeshPoints, yRatePrimeArray)


