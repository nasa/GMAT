function [state] = cart2sphAzFPA(cart)

rMag = norm(cart(1:3,1));
vMag = norm(cart(4:6,1));
rightAsc = atan2(cart(2,1),cart(1,1));
dec  = asin(cart(3,1)/rMag);
flightPathAng = acos(dot(cart(1:3,1),cart(4:6,1))/rMag/vMag);
cosDec = cos(dec);
sinDec = sin(dec);
cosRightAsc = cos(rightAsc);
sinRightAsc = sin(rightAsc);


xHat = [cosDec*cosRightAsc cosDec*cosRightAsc sinDec]';
yHat = [cos(rightAsc + pi/2) sin(rightAsc + pi/2) 0]';
zHat = [-sinDec*cosRightAsc -sinDec*sinRightAsc cosDec]';

R = [xHat yHat zHat]';
newV = R*cart(4:6,1);
az = atan2(newV(2,1),newV(3,1));

state = [rMag rightAsc dec vMag az flightPathAng]';