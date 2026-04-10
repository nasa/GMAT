function [ month, day, hour, minute, second ] = doy_2_mmdd( year, doy )
%
MM = [ 31 28 31 30 31 30 31 31 30 31 30 31 ];

%...converts day of year to month and day
if ( year/4 == fix( year/4 ) ), MM(2) = 29; end  
%
CMM = cumsum(MM); %CMM(12) = CMM(12) + 1;
%  
month = min( find( CMM - fix(doy)  >= 0 ) );
%
if ( month == 1 )
    dayf = doy;
else
    dayf = doy - CMM(month-1);
end

day = fix(dayf);
hourf = 24 * ( dayf - day ); hour = fix(hourf);
minf = 60 * ( hourf - hour ); minute = fix(minf);
second = 60 * ( minf - minute );

%  function [ month, day ] = doy_2_mmdd( year, doy )