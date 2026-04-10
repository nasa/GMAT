cartState = [-5.31484348103882E+10
    -1.30332439062922E+11
    -5.64981086004718E+10
    3.08670612779478E+04
    -1.21667783491300E+04
    -2.88270677432904E+03 ]/1000;
controlVec =  0*[2.37938550172423e-05       0.00133985713836812      0.000464696223693711]';
m0 = 1.69700000000000E+03;

% Contants
sunGrav = 132712440017.99


controlMag_Newtons = .236/1000*.8;
g = 9.81;
Isp = 4190;

R = [1.0 0.0 0.0;
    0.0 0.91748206 -0.397777156;
    0.0 0.39777716 0.9174820621]';
Rmat = [R zeros(3,3);zeros(3,3) R];

testDyn = ModEqDynamics(sunGrav)
meeElems = Cart2Mee(Rmat*cartState,sunGrav)


pertForces = (controlMag_Newtons)*controlVec/m0;

derivs = testDyn.ComputeOrbitDerivatives(meeElems,pertForces)

