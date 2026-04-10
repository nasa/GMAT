function cart = Kep2Cart2(elements,mu)
elements(3:6)=deg2rad(elements(3:6));
if (isempty(elements))
    cart = [];
    return
end
%  Set mu.  If not provided assume Earth.
if nargin < 2
    mu = 398600.4418;
end
%  Break input into components
SMA = elements(1);
ECC = elements(2);
INC = elements(3);
AOP = elements(5);
RAAN = elements(4);
MA   = elements(6);

EA=DKEPLR(MA,ECC);

TA=2*atan(sqrt(1+ECC)/sqrt(1-ECC)*tan(EA/2));
%{
%  Sanity check input
if ( ECC < 0 )
    cart = [];
    disp('Warning: ECC must be greater than 0');
    return
end
%}
%  Signs on SMA and ECC are not consistent
if ( ECC > 1 && SMA > 0)
    cart = [];
    disp('Warning: If ECC > 1, SMA must be negative');
    return
end
%  Signs on SMA and ECC are not consistent
if ( ECC < 1 && SMA < 0)
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

%  Orbit is hyperbolic and TA is set higher/lower than the limiting value
if (ECC > 1 && SMA < 0)
   limitVal = pi - acos(1/ECC);
   modTA = mod(TA,2*pi);
   if modTA > pi
       modTA = modTA - 2*pi;
   end
   if abs(modTA) > limitVal % a modification is made to fix error by YK
        cart = [];
        disp(['Warning:  The TA value is not physically possible for a hyperbolic orbit ']);
        disp(['with the given SMA and ECC.']);
        disp(['The allowed values are: ' num2str(-limitVal*180/pi,16) ' < TA < ' num2str(limitVal*180/pi,16) ]);
        disp(['Or equivalently: '])
        disp(['The allowed values are:  TA <' num2str(limitVal*180/pi,16)  ' or  TA > ' num2str((2*pi-limitVal)*180/pi,16) ]);
        return
   end
end

    
%  Equatorial and Circular
if ( ECC == 0 && INC == 0)
    LAMtrue = AOP + RAAN + TA;
    TA = LAMtrue;
    AOP = 0;
    RAAN = 0;
%  Circular Inclined    
elseif ( ECC == 0 && INC ~= 0 )
    U = AOP + TA;
    TA = U;
    AOP = 0;
%  Equatorial    
elseif ( INC == 0 && ECC ~= 0 )
    AOP = AOP + RAAN;
    RAAN = 0;
%  Retrpgrade Equatorial - added by YK
elseif ( INC >= pi -1e-11 && ECC ~= 0 )
    
    RAAN = AOP + RAAN;
    AOP = 0;
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
if OnePlusECCcosTA < 1e-10 && ECC > 1
    disp('Warning: Conversion from Kep to Cart for hyperbolic orbit results is near singularity that occurs when TA = acos(1/ECC).')
end

%  position and velocity in perifocal system
rv_p = [p*cosTA/OnePlusECCcosTA p*sinTA/OnePlusECCcosTA 0]';
vv_p = [-sqrtmup*sinTA sqrtmup*(ECC + cosTA) 0]';

%  rotate to inertial system
Rot = R3(-RAAN)*R1(-INC)*R3(-AOP);
rv = Rot*rv_p;
vv = Rot*vv_p;

cart = real([rv;vv]).';



