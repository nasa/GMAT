function cart = RANDV(elements,mu);

if (isempty(elements))
    cart = [];
    return
end

%  Break input into components
SMA = elements(1);
ECC = elements(2);
INC = elements(3);
AOP = elements(4);
RAAN = elements(5);
TA = elements(6);

%  Sanity check input
if ( ECC < 0 )
    cart = [];
    disp('Warning: ECC must be greater than 0');
    return
end
%  Signs on SMA and ECC are not consistent
if ( ECC > 1 & SMA > 0)
    cart = [];
    disp('Warning: If ECC > 1, SMA must be negative');
    return
end
%  Signs on SMA and ECC are not consistent
if ( ECC < 1 & SMA < 0)
    cart = [];
    disp('Warning: If SMA < 0, ECC must be greater than zero and less than one');
    return
end
%  Exaclty Parabolic orbit
if ( ECC == 1 )
    cart = [];
    disp('Warning: Parabolic orbits cannot be entered in Keplerian or Modified Keplerian format');
    return
end
%  Parabolic orbit to machine precision
if (abs(ECC - 1) < 2*eps)
    disp('Warning:  Orbit is nearly parabolic and state conversion routine is near numerical singularity');
end

    
%  Equatorial and Circular
if ( ECC == 0 & INC == 0)
    LAMtrue = AOP + RAAN + TA;
    TA = LAMtrue;
    AOP = 0;
    RAAN = 0;
%  Circular Inclined    
elseif ( ECC == 0 & INC ~= 0 )
    U = AOP + TA;
    TA = U;
    AOP = 0;
%  Elliptic Equatorial    
elseif ( INC == 0 & ECC ~= 0 )
    AOP = AOP + RAAN;
    RAAN = 0;
end

if ( ECC == 1 )
    %  Not supported yet!!
else
    p = SMA*(1-ECC*ECC);
end

%  Calculate terms used more than once
OnePlusECCcosTA = ( 1 + ECC*cos(TA));
sqrtmup = sqrt(mu/p);
sinTA = sin(TA);
cosTA = cos(TA);

%  position and velocity in perifocal system
rv_p = [p*cosTA/OnePlusECCcosTA p*sinTA/OnePlusECCcosTA 0]';
vv_p = [-sqrtmup*sinTA sqrtmup*(ECC + cosTA) 0]';

%  rotate to inertial system
Rot = R3(-RAAN)*R1(-INC)*R3(-AOP);
rv = Rot*rv_p;
vv = Rot*vv_p;

cart = real([rv;vv]);



