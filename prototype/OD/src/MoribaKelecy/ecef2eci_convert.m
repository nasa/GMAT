function [eciPos,eciVel,secondsPastJ2K] = ecef2eci_convert(year,month,day,...
                                        hour,minute,second,ecefPos,ecefVel)
%
%...purpose:  to convert an ecef site vector to eci
%
%...inputs:  year = 4-digit year
%            month = 2-digit month
%            day = 2 digit day of month
%            hour = hour of day
%            minute = minute of hour
%            second = tick, tick, tick...
%
%...output:  eciPos = 3 x 1 inertial site position vector
%            eciVel = 3 x 1 inertial site velocity vector
%            secondsPastJ2K = number of second past j2k
%
%   t. kelecy, boeing-lts, 12/17/2007 (constructed from code provided by m. jah)
format long g

%...establish global variables and parameters
global xp yp dEpsilon dPsi UT1minusUTC dAT
global Tmod2moj2k Ttod2mod Tecefwopm2ecitod Tecefwpm2ecef
global deg2rad omegaearth

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ECEF to ECEF with Polar Motion rotation matrix (Polar Motion matrix) %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Tecef2ecefwpm = [1,0,xp;0,1,-yp;-xp,yp,1];
Tecef2ecefwpm = orthodcm(Tecef2ecefwpm);
% ECEF with Polar Motion to ECEF
Tecefwpm2ecef = Tecef2ecefwpm';

%...time conversions
[UT1,TAI,TDT,TDB,thetaGST] = convtime(year,month,day,hour,minute,second,UT1minusUTC,dAT);
tinitial = julian_mod(TDB(1),TDB(2),TDB(3),TDB(4),TDB(5),TDB(6));

juliandateTDB = tinitial;

T_TDB = (juliandateTDB-2451545)/36525; % Julian Centuries past J2000
Zeta = deg2rad*(0.6406161*T_TDB + 0.0000839*T_TDB^2 + 0.0000050*T_TDB^3);
Theta = deg2rad*(0.6406161*T_TDB + 0.0003041*T_TDB^2 + 0.0000051*T_TDB^3);
Eta = deg2rad*(0.5567530*T_TDB + 0.0001185*T_TDB^2 + 0.0000116*T_TDB^3);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Earth Mean of Epoch (J2000) to Mean of Date rotation matrix %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Tmoj2k2mod = [cos(Zeta)*cos(Theta)*cos(Eta) - sin(Zeta)*sin(Eta), -cos(Zeta)*cos(Theta)*sin(Eta) - sin(Zeta)*cos(Eta), -cos(Zeta)*sin(Theta);...
    sin(Zeta)*cos(Theta)*cos(Eta) + sin(Zeta)*sin(Eta), -sin(Zeta)*cos(Theta)*sin(Eta) + cos(Zeta)*cos(Eta), -sin(Zeta)*sin(Theta);...
    sin(Theta)*cos(Eta), -sin(Theta)*sin(Eta), cos(Theta)];
Tmoj2k2mod = orthodcm(Tmoj2k2mod);
% Earth mean of date to mean of J2000
Tmod2moj2k = Tmoj2k2mod';

% mean obliquity of the ecliptic (Moyer and Vallado agree on this one)
EpsilonBar = 23.439291 - 0.0130042*T_TDB - 0.16e-6*T_TDB^2 + 0.504e-6*T_TDB^3;
% Obliquity of the ecliptic
Epsilon = EpsilonBar + dEpsilon;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Earth mean of date to Earth true of date rotation matrix (Nutation matrix) %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Tmof2tod = [1,-deg2rad*dPsi*cos(deg2rad*Epsilon), -deg2rad*dPsi*sin(deg2rad*Epsilon);...
    deg2rad*dPsi*cos(deg2rad*Epsilon), 1, -deg2rad*dEpsilon;...
    deg2rad*dPsi*sin(deg2rad*Epsilon), deg2rad*dEpsilon, 1];
Tmof2tod = orthodcm(Tmof2tod);
% Earth true of date to Earth mean of date rotation matrix;
Ttod2mod = Tmof2tod';

% Apparent Sidereal Time angle
thetaAST = thetaGST + dPsi*cos(deg2rad*Epsilon);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ECI true of date to ECEF without polar motion %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Tecitod2ecefwopm = [cos(deg2rad*thetaAST),sin(deg2rad*thetaAST),0;-sin(deg2rad*thetaAST),cos(deg2rad*thetaAST),0;0,0,1];
Tecitod2ecefwopm = orthodcm(Tecitod2ecefwopm);

% ECEF without polar motion to ECI true of date
Tecefwopm2ecitod = Tecitod2ecefwopm';

% rotate ecef position vector to eme2000
eciPos = Tmod2moj2k*Ttod2mod*Tecefwopm2ecitod*Tecefwpm2ecef*ecefPos;
eciVel = Tmod2moj2k*Ttod2mod*Tecefwopm2ecitod*(Tecefwpm2ecef*ecefVel + cross([0;0;omegaearth],ecefPos));

secondsPastJ2K = (juliandateTDB - 2451545.0)*86400;

%...end of function 