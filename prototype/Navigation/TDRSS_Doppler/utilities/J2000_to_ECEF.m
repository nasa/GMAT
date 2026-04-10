function [r_ecef,v_ecef,a_ecef] = J2000_to_ECEF(r_eci,v_eci,a_eci,eqeterms,epoch)
%
% Convert mean equator mean equniox frame (j2000 to ECEF (ITRF) using the
% IAU76-FK5 Reduction
%
% Inputs: r_eci, j2000 position vector in m
%         v_eci, j2000 velocity vector in m/s
%         a_eci, j2000 acceleration in m/s^2
%         eqeterms, - terms for ast calculation      0,2
%         epoch, serial date
%
% Outputs: r_ecef (itrf) position vector in m
%          v_ecef (itrf) position vector in m/s
%          a_ecef (itef) acceleration vector in m/s^2
%

% define global variables if this is used in a loop. 
global observed predicted taiminusutc

% convert to ecef                     
if ~exist('taiminusutc','var') 
    taiminusutc = get_tai_minus_utc;
end

% find tai
tai = epoch +taiminusutc/86400;
% find dynamic time tt
tt = tai+32.184/86400;
[Y,MO,D,H,MI,S] = datevec(tt);
JDtt = jdayall(Y,MO,D,H,MI,S,'g');
% find julian centeries
Ttt = (JDtt -2451545.0)/36525;
% find jdut1
[year, mon, day, hour, min, sec ]  = datevec(epoch);
day_num  = datenum([year,mon,day]);

if ~exist('observed','var') || ~exist('predicted','var') 
    [observed,predicted] = earth_orientation;
end

% find earth orientation data at the epoch. Use observed if available, if not, use
% predicted
idx_epoch = find(observed.utc_num==day_num);
if isempty(idx_epoch)
%     fprintf('Astronomical observed data not available, using predicted\n')
    idx_epoch = find(predicted.utc_num==day_num);
    data=predicted;
else
    data=observed;
end

% pull out needed data for conversion
x=d2r(data.x(idx_epoch)/60);
y=d2r(data.y(idx_epoch)/60);
dpsi= d2r(data.dpsi(idx_epoch)/60);
deps= d2r(data.deps(idx_epoch)/60);
UT1minusUTC=data.UT1minusUTC(idx_epoch);
lod=data.lod(idx_epoch);

% compute ut1 parameters
ut1 = epoch+UT1minusUTC/86400;
[Y,MO,D,H,MI,S] = datevec(ut1);
JDut1 = jdayall(Y,MO,D,H,MI,S,'g');

% if empty still, throw error
if isempty(idx_epoch)
    error('Astronomical data not available for this epoch!')
end

% lets convert to j2000, our common inertial frame
[r_ecef,v_ecef,a_ecef] = eci2ecef  ( r_eci,v_eci,a_eci, Ttt,JDut1,lod,x,y,eqeterms,dpsi,deps );


end
