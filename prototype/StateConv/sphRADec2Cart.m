function [cart] = sphRADec2cart(state)

rMag = state(1); vMag = state(4);
rightAsc = state(2,1);
cosRA = cos(state(2,1)); sinRA = sin(state(2,1));
cosDEC = cos(state(3,1)); sinDEC = sin(state(3,1));
cosRAV = cos(state(5,1)); sinRAV = sin(state(5,1));
cosDECV = cos(state(6,1)); sinDECV = sin(state(6,1));

cart(1:3,1) = rMag*[cosDEC*cosRA cosDEC*sinRA sinDEC]';
cart(4:6,1) = vMag*[cosDECV*cosRAV cosDECV*cosRAV*tan(state(5,1)) sinDECV]';