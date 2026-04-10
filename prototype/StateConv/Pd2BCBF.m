% Developled by MHK from Yonsei Univ.
% convert from Planetodetic state to Body-Centered Body-Fixed state
% Planetodetic state = [ RMAG LON LAT(detic) VMAG AZI HFPA ]
% spehricalAZFPA = [ rmag RA(Àû°æ) DEC vmag AZI VFPA ]
function  [ BCBF ] = Pd2BCBF(Pd,Planet)

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

if ( length(Pd) ~= 6 )
    BCBF = [];
    disp('Error : Planetodetic state has six elements')
    return
end

rmag = Pd(1); lon = Pd(2); latd = Pd(3);
vmag = Pd(4); azi = Pd(5); hfpa = Pd(6); vfpa = (pi/2) - hfpa;

e2 = 2*f - f*f;

tol = 1; latg = latd;

while ( tol >= 1e-13 )
    
    latg_old = latg;
    
    x = rmag * cos(latg_old) * cos(lon);
    y = rmag * cos(latg_old) * sin(lon);
    
    r_xy = sqrt(x^2 + y^2);
    alt = r_xy/cos(latd) - Req/(sqrt(1-e2*(sin(latd))^2)); 

% % Calculate geocentric latitude (referr to "Orbital Mechanics with MATLAB" p.11)
    sin2 = sin(2*latd); sin4 = sin(4*latd); 
    h_hat = alt /Req; denom = ( h_hat + 1 );

    latg = latd + (-sin2/denom)*f + ( (-sin2)/(2*denom^2) +...
        (1/(4*denom^2) + 1/(4*denom))*sin4 )*f^2; % geocentric latitude
    
    tol = abs(latg - latg_old);
    
end

% Using sphAzFPA2cart
tmp = [ rmag lon latg vmag azi vfpa ]';
BCBF = sphAzFPA2cart(tmp);

end
