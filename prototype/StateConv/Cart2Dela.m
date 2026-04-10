function [ Dela ] = Cart2Dela(Cart,mu)

% % If mu is not provided, use the mu for Earth
if nargin < 2
    mu = 398600.4415;
end

if ( length(Cart) ~= 6 )
    Dela = [];
    disp('Error: Cartesian state have six elements')
    return
end

Kep = Cart2Kep(Cart,mu);
Dela = Kep2Dela(Kep,mu);

end