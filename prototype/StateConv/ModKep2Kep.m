function [kepel] = ModKep2Kep(modkepel)

if isempty(modkepel)
    kepel = [];
    return
end

%  This function converts from modified keplerian elements to keplerian
%  elements.

RadPer = modkepel(1);
RadApo = modkepel(2);
INC    = modkepel(3);
RAAN   = modkepel(4);
AOP    = modkepel(5);
TA     = modkepel(6);

if RadApo < RadPer & RadApo > 0
    disp('Warning: If RadApo < RadPer then RadApo must be negative')
    kepel = [];
    return
end
if RadPer <= 0
    disp('Warning: Radius of Periapsis must be greater than zero');
    kepel = [];
    return
end

if RadApo == 0
    disp('Warning: Radius of Apoapsis must not be zero');
    kepel = [];
    return
end

if (RadPer == 0)
    kepel = [];
    disp('Warning: Parabolic orbits are not currently supported');
    disp('Warning: RadPer must be greater than zero');
    return
end
rpbyra = RadPer/RadApo;

ECC = ( 1 - rpbyra ) / (1 + rpbyra);
SMA = RadPer/(1-ECC);


kepel = [SMA ECC INC RAAN AOP TA];



