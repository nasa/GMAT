function [radeccomp] = myOpticalObservations_batch(X)

%...purpose:  to compute the right ascension and declination angle
%             measurements.
%
%...inputs:  X = 6 x 1 cartesian position/velocity state (m, m/s)
%
%...output:  radeccomp = 2 x 1 ra, dec vector for the given state (rad)
format long g


global eciPos  OWLT  speedoflight

% compute the apparent range and its magnitude
ltc = 0;
for ii=1:3
    rangeApparent =  X(1:3,1) - eciPos - ltc*X(4:6,1);
    range_mag = norm(rangeApparent);
    ltc = OWLT * range_mag / speedoflight;
end

% compute apparent Declination
decTop = asin(rangeApparent(3,1)/range_mag);
% compute apparent Right Ascension
foo = sqrt(rangeApparent(1,1)^2+rangeApparent(2,1)^2);
raTop = atan2((rangeApparent(2,1)/foo),(rangeApparent(1,1)/foo));
raTop = mod(raTop,2*pi); % RA is between 0 and 360 degrees

radeccomp = [raTop;decTop];