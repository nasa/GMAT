function elements = Cart2Kep2(cart,mu)
if nargin < 2
    mu = 398600.4418;
end

if isempty(cart)
    elements = [];
    return
end

twopi = 2*pi;

rv = cart(1:3);  % position vector
vv = cart(4:6);  % velocity vector
r  = norm(rv);   % magnitude of position vector
v  = norm(vv);   % magnitude of velocity vector

hv = cross(rv,vv);  % orbit angular momentum vector
h  = norm(hv);      

kv = [0 0 1]';      %  Inertial z axis
nv = cross(kv,hv);  %  vector in direction of RAAN,  zero vector if equatorial orbit
n = norm(nv);

%  Caculate eccentricity vector and eccentricity
ev = (v*v - mu/r)*rv/mu - dot(rv,vv)*vv/mu;
ECC  = norm(ev);    
E = v*v/2 - mu/r;   %  Orbit Energy

% Check eccentrity for parabolic orbit
if ( abs(1 - ECC) < 2*eps )
    disp('Warning:  Orbit is nearly parabolic and state conversion routine is near numerical singularity')
end
% Check Energy for parabolic orbit
if ( abs(E) < 2 * eps )
    disp('Warning:  Orbit is nearly parabolic and state conversion routine is near numerical singularity')
end
    

%  Determine SMA depending on orbit type
if ( ECC ~= 1 )
    SMA = -mu/2/E;
    p   = SMA*(1-ECC*ECC);
else
    SMA = inf;
    p   = h*h/mu;
end

%  Determine Inclination
INC     = acos(hv(3)/h);

%  Determine if we have a special case
fac = 2;   %  It is rare, due to round off error, that ECC is identically zero.
%  This factor (fac) is used to determine if orbit is circular or not
%  by comparing the eccentricity with fac*eps.  If you want be
%  rigourous in your definition of a circular orbit set fac = 0;
%  I chose fac = 2 so that orbits with eccentricity < 2*eps
%  will be considered circular.
if ( INC <= 1e-11 & ECC > fac*eps ) 
    
    %  Determine RAAN
    RAAN = 0;
    
    %  Determine AOP    
    AOP    = acos(ev(1)/ECC);
    if ( ev(2) < 0 )
        AOP = twopi - AOP;
    end
    
    %  Determine TA   
    TA = acos(dot(ev,rv)/ECC/r);
    if ( dot(rv,vv) < 0 )
        TA = twopi - TA;
    end
elseif ( INC >= pi - 1e-11 & ECC > fac*eps ) % added to express retrograde equatorial orbit by YK
    %  Determine AOP
    AOP  = 0;
    
    %  Determine RAAN    
    RAAN    = acos(ev(1)/ECC);
    if ( ev(2) < 0 )
        RAAN = twopi - RAAN;
    end
    
    %  Determine TA   
    TA = acos(dot(ev,rv)/ECC/r);
    if ( dot(rv,vv) < 0 )
        TA = twopi - TA;
    end
    
    
elseif ( n ~= 0 & ECC <= fac*eps )
    
    %  Determine RAAN
    RAAN = 0;
    
    %  Determine AOP    
    AOP  = 0;
    
    %  Determine TA  
    TA = acos(dot(nv/n,rv/r));
    if ( rv(3) < 0 )
        TA = twopi - TA;
    end
    
elseif ( n == 0 & ECC <= fac*eps  )
    
    RAAN = 0;
    AOP  = 0;
    TA   = acos(rv(1)/r);
    if ( rv(2) < 0 )
        TA = twopi - TA;
    end
    
else  %  Not a special case
    
    %  Determine RAAN
    RAAN    = acos(nv(1)/n);
    if ( nv(2) < 0 )
        RAAN = twopi - RAAN;
    end
    
    %  Determine AOP
    AOP    = acos(dot(nv/n,ev/ECC));
    if ( ev(3) < 0 )
        AOP = twopi - AOP;
    end
    
    %  Determine TA
    TA = acos(dot(ev/ECC,rv/r));
    if ( dot(rv,vv) < 0 )
        TA = twopi - TA;
    end
    
end
EA=2*atan(sqrt(1-ECC)/sqrt(1+ECC)*tan(TA/2));
MA=EA-ECC*sin(EA);

elements = real([SMA ECC INC RAAN AOP MA]);
elements(3:6)=rad2deg(elements(3:6));
%elements = [SMA ECC INC AOP RAAN TA];



