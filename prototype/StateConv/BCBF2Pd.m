% Developled by MHK from Yonsei Univ.
% convert from Body-Centered Body-Fixed state to Planetodetic state
% Planetodetic state = [ RMAG LON LAT(detic) VMAG AZI HFPA ]
% spehricalAZFPA = [ rmag RA(Àû°æ) DEC vmag AZI VFPA ]
function [ Pd ] = BCBF2Pd(BCBF,Planet)

if (strcmp(Planet,'Earth') == 1) || (strcmp(Planet,'EARTH') == 1)...
        || (strcmp(Planet,'earth') == 1)
    Req = 6378.1363; % [km]
    f = 0.0033527; % flattening factor
elseif (strcmp(Planet,'Moon') == 1) || (strcmp(Planet,'MOON') == 1)...
        || (strcmp(Planet,'moon') == 1)
    Req = 1738.2;
    f = 0.00125;
elseif (strcmp(Planet,'Mars') == 1) || (strcmp(Planet,'MARS') == 1)...
        || (strcmp(Planet,'mars') == 1)
    Req = 3397;
    f = 0.0064763;
elseif (strcmp(Planet,'Jupiter') == 1) || (strcmp(Planet,'JUPITER') == 1)...
        || (strcmp(Planet,'jupiter') == 1)
    Req = 71492;
    f = 0.06487439;
elseif (strcmp(Planet,'Saturn') == 1) || (strcmp(Planet,'SATURN') == 1)...
        || (strcmp(Planet,'saturn') == 1)
    Req = 60268;
    f = 0.09796243;
elseif (strcmp(Planet,'Uranus') == 1) || (strcmp(Planet,'URANUS') == 1)...
        || (strcmp(Planet,'uranus') == 1)
    Req = 25559;
    f = 0.02292734;
elseif (strcmp(Planet,'Neptune') == 1) || (strcmp(Planet,'NEPTUNE') == 1)...
        || (strcmp(Planet,'neptune') == 1)
    Req = 25269;
    f = 0.01856029;
end

if ( length(BCBF) ~= 6 )
    Pd = [];
    disp('Error : BCBF state has six elements')
    return
end

Pc = cart2sphAzFPA(BCBF);
rmag = Pc(1); lon = Pc(2); latg = Pc(3);
vmag = Pc(4); azi = Pc(5); vfpa = Pc(6); hfpa = (pi/2) - vfpa;

rbcbf = BCBF(1:3); rx = rbcbf(1); ry = rbcbf(2); rz = rbcbf(3);
r_xy = sqrt( rx^2 + ry^2 );

% % latd = atan2(rz,r_xy);
latd = latg;
e2 = 2*f - f*f;

tol = 1;
while ( tol >= 1e-13 )
    latd_old = latd;
    C = Req / sqrt( 1 - e2*sin(latd_old)*sin(latd_old) );
    latd = atan( (rz + C*e2*sin(latd_old))/r_xy);
    tol = abs(latd - latd_old);
end

Pd = [ rmag lon latd vmag azi hfpa ]';

end