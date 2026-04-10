
function mjd = mjuliandate(y,mm,d,h,m,s)
% mjd = mjuliandate(y,mm,d,h,m,s)
% Compute the modified julian date
% Reference: Vallado Algorithm 14

    % compute  julian date
    JD = 367*y - floor( ( 7 * (y + floor( (mm + 9)/12))/4)) + floor(275*mm/9) + d +1721013.5;
    % compute part of day
    pod = ((((s/60) + m)/60) +h)/24;
    % compute modified julian date
    mjd = (JD - 2430000.0) + pod;
    
end