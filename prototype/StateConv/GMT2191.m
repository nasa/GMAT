sma  = 7191.938817629013;
ecc  = 0.02454974900598137;
inc  = 1e-6;
raan = 306.6148021947984;
aop  = 314.1905515359921;
ta   = 99.8877493320488;
mu   = 398600.4415;

oe = oedeg2rad([sma ecc inc aop raan ta])
cart = stateconv(oe,2,1,mu);
oe2  = stateconv(cart,1,2,mu)
return
%oe2(6)*180/pi
%return
cart2 = stateconv(oe2,2,1,mu)
oe - oe2
cart - cart2
% GMAT DefaultSC.X = -1019.860555667956;
% GMAT DefaultSC.Y = -6850.100903584877;
% GMAT DefaultSC.Z = -1118.721548814026;
% GMAT DefaultSC.VX = 7.424798539036117;
% GMAT DefaultSC.VY = -1.298590097884125;
% GMAT DefaultSC.VZ = 1.182791230152587;

