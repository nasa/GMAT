function [cart] = sphAzFPA2cart(state)

rMag = state(1); vMag = state(4);
rightAsc = state(2,1);
cosRA = cos(state(2,1)); sinRA = sin(state(2,1));
cosDEC = cos(state(3,1)); sinDEC = sin(state(3,1));
cosAZ = cos(state(5,1)); sinAZ = sin(state(5,1));
cosFPA = cos(state(6,1)); sinFPA = sin(state(6,1));

cart(1:3,1) = rMag*[cosDEC*cosRA cosDEC*sinRA sinDEC]';



cart(4,1)    = vMag*(cosFPA*cosDEC*cosRA - sinFPA*(sinAZ*sinRA + cosAZ*sinDEC*cosRA) );
cart(5,1)    = vMag*(cosFPA*cosDEC*sinRA + sinFPA*(sinAZ*cosRA - cosAZ*sinDEC*sinRA) );
cart(6,1)    = vMag*(cosFPA*sinDEC + sinFPA*cosAZ*cosDEC);


