
function P = get_period(alt_p, alt_a)
% compute the period of an orbit given alt of perigee and alt of apogee
%
% Inputs: altitude of perigee (km)
%         altitude of apogee  (km)
%
% Output: Period (s)
%

rp = alt_p + RE;
ra = alt_a + RE; 

a = (rp + ra)/2;

P = 2*pi*sqrt(a^3/MU);

end