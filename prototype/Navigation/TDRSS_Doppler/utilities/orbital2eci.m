
 function [r,v] = orbital2eci(a,e,i,raan,w,nu)
% INPUTS: a = semi-major axis (km)
%         e = eccentricity
%         i = inclination (degrees)
%         raan = right ascension of the ascending node (degrees)
%         w = argument of perigee (degrees)
%         nu = true anomaly (degrees)
%
%
%         TODO: Orbit error checking. Handle circular and equatorial orbits
%               Allow multidimensional inputs

% convert angles to radians
 i = d2r(i);
 raan = d2r(raan);
 w = d2r(w);
 nu = d2r(nu);

% evaluate eccentricity. Set threshold for circular orbits at
% sqrt of machine tolerance

if e<= sqrt(eps)
    circular=1;
    fprintf('Argument of perigee not well defined! Spec value correctly! Rotation may be invalid\n')
else
    circular=0;
end

% solve for semi-parameter from ecc
if circular
    p=a;
elseif e<(1-sqrt(eps))
    p=a.*(1-e.^2);
end
    
% evaluate inclination
if i<=sqrt(eps)
    equatorial=1;
    fprintf('Ascending node not well defined! Spec value correctly! Rotation may be invalid\n')
else
    equatorial=0;
end


% find the perifocal r, and v

rpqw = [ p.*cos(nu)./(1+e.*cos(nu)) ; p.*sin(nu)./(1+e.*cos(nu)); 0];
vpqw =  [ -sqrt(MU./p) .* sin(nu) ;sqrt(MU./p) .*(e +cos(nu)); 0];

% rotate into the geocentric equatorial system

ijkpqw = [ ((cos(raan).*cos(w)) - (sin(raan).*sin(w).*cos(i)))...
            ((-cos(raan).*sin(w))-(sin(raan).*cos(w).*cos(i)))...
            (sin(raan).*sin(i));
            ((sin(raan).*cos(w))+(cos(raan).*sin(w).*cos(i)))...
            ((-sin(raan).*sin(w))+(cos(raan).*cos(w).*cos(i)))...
             -cos(raan).*sin(i);
             (sin(w).*sin(i))...
             (cos(w).*sin(i))...
             cos(i)
            ];
        
% apply the rotation
r =  ijkpqw*rpqw;
v =  ijkpqw*vpqw;

end