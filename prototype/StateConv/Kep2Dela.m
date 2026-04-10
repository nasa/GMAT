function [ Dela ] = Kep2Dela(Kep,mu)

% % If mu is not provided, use the mu for Earth
if nargin < 2
    mu = 398600.4415;
end

if ( length(Kep) ~= 6 )
    Dela = [];
    disp('Error: Keplerian Orbital Elements have six elements')
    return
end

% % Initializations
SMA = Kep(1); % semi-major axis
ECC = Kep(2); % eccentricity
INC = Kep(3); % inclination
AOP = Kep(4); % argument of periapsis
RAAN = Kep(5); % right ascension of the ascending node
TA = Kep(6); % true anmaly

% % Sanity check input
if ( ECC < 0 )
    Dela = [];
    disp('Error: ECC must be greater than 0')
    return
end

if ( abs(1-ECC) < 2*eps ) % eps is machine precision
    Dela = [];
    disp('Error: A nearly parabolic orbit was encounterd')
    disp('while converting from the Keplerian elements to Modified Equinoctial')
    disp('Modified Equinoctial Elements cannot be defined at parabolic orbit')
    return
end

if ( ECC > 1 && SMA > 0 )
    Dela = [];
    disp('Error: If ECC > 1, SMA must be negative')
    return
end

if ( ECC < 1 && SMA < 0 )
    Dela = [];
    disp('Error: If SMA > 0, ECC must be less than 1')
    return
end

if ( SMA*(1-ECC) < 0.001 )
    Dela = [];
    disp('Error: A nearly singular conic section was encountered')
    disp('while converting from the Keplerian elements to Modified Equinoctial.')
    disp('the radius of periapsis must be greater than 1 meter')
    return
end

if ( ECC > 1 && SMA < 0)
    limitVal = pi - acos(1/ECC);
    modTA = mod(TA,2*pi);
    
    if modTA > pi
        modTA = modTA - 2*pi;
    end
    
    if abs(modTA) > limitVal
        Dela = [];
        disp('Error: The TA value is not physically possible for a hyperbolic orbit')
        disp('with the given SMA and ECC.')
        return
    end
end

L = sqrt( mu*SMA );
G = L * sqrt( 1-ECC^2 );
H = G * cos(INC);
MA = nu2M(TA,ECC);
% % sinEA = (sqrt(1-ECC^2)*sin(TA)) / (1+ECC*cos(TA));
% % cosEA = (ECC+cos(TA)) / (1+ECC*cos(TA));
% % % EA = atan3(sinEA,cosEA);
% % EA = mod(atan2(sinEA,cosEA),2*pi);
% % MA = EA - ECC*sinEA;
ll = MA;
gg = AOP;
hh = RAAN;

% % if ( ll >= (2*pi) )
% % %     ll = mod(ll,2*pi);
% %     ll = ll - 2*pi;
% % end
% % 
% % if ( gg >= (2*pi) )
% % %     gg = mod(gg,2*pi);
% %     gg = gg - 2*pi;
% % end
% % 
% % if ( hh >= (2*pi) )
% % %     hh = mod(hh,2*pi);
% %     hh = hh - 2*pi;
% % end

Dela = [ L G H ll gg hh ]';

end