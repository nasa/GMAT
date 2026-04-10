TOF = 370*86400; 
initMass = 1900
ThrustinNewtons = 0.09;
Isp = 1660;
g = 9.81;

mDot = -ThrustinNewtons/Isp/g

deltaM = mDot*TOF

initMass + deltaM


GM = 398600.4415
C3 = 0.25
SMA = -GM/C3

rp =  6628.0
%rp = SMA*(1-ECC)
%rp/SMP = 1 - ECC
ECC = 1 - rp/SMA
ra =  SMA*(1+ECC)
