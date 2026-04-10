% This algorithm given by David Vallado will take certain time and date parameters
% and convert them to the widely used time conventions (UT1,TAI,TDT, and TDB)

function [UT1,TAI,TDT,TDB,thetaGST] = convtime(yearUTC,monthUTC,dayUTC,hourUTC,minuteUTC,secondUTC,UT1minusUTC,dAT)

%...purpose:  to perform all of the time conversions (including GST)
%             necessary for earth orientation transformations.
%

format long g

% Earth's angular velocity in rad/sec
%omegaearth = 7.2921158553*(10^(-5));

matlabUTC = datenum(yearUTC,monthUTC,dayUTC,hourUTC,minuteUTC,secondUTC);
matlabUT1 = matlabUTC + (UT1minusUTC)/86400;
%UT1 = datevec(matlabUT1,'FFF');
UT1 = datevec(matlabUT1);
%[yearUT1,monthUT1,dayUT1,hourUT1,minuteUT1,secondUT1] = datevec(matlabUT1,'FFF');
[yearUT1,monthUT1,dayUT1,hourUT1,minuteUT1,secondUT1] = datevec(matlabUT1);
julianDateUT1 = julian_mod(yearUT1,monthUT1,dayUT1,hourUT1,minuteUT1,secondUT1);
T_UT1 = (julianDateUT1-2451545.0)/36525;
thetaGST = 67310.54841 + (3.164400184812866e9)*T_UT1 + 0.093104*T_UT1^2 - 6.2e-6*T_UT1^3;
thetaGST = mod((thetaGST/86400)*360,360); % degrees

matlabTAI = matlabUTC + (dAT/86400);
%TAI = datevec(matlabTAI,'FFF');
TAI = datevec(matlabTAI);

matlabTDT = matlabTAI + (32.184/86400);
%TDT = datevec(matlabTDT,'FFF');
TDT = datevec(matlabTDT);
%[yearTDT,monthTDT,dayTDT,hourTDT,minuteTDT,secondTDT] = datevec(matlabTDT,'FFF');
[yearTDT,monthTDT,dayTDT,hourTDT,minuteTDT,secondTDT] = datevec(matlabTDT);
julianDateTDT = julian_mod(yearTDT,monthTDT,dayTDT,hourTDT,minuteTDT,secondTDT);
T_TDT = (julianDateTDT-2451545.0)/36525;
Msun = 6.240035939 + 628.3019560*T_TDT;

matlabTDB = matlabTDT + ((0.001658*sin(Msun) + 0.00001385*sin(2*Msun))/86400);
%TDB = datevec(matlabTDB,'FFF');
TDB = datevec(matlabTDB);

%...end of function [UT1,TAI,TDT,TDB,thetaGST] = convtime