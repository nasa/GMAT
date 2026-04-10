function test_suite = testNonKep()                          %#ok<STOUT>
%TESTNONKEP Unit tests for Nonsingular Keplerian state representation

initTestSuite();

end

function ret = setup()                                      %#ok<*DEFNU>
addpath('..');
ret.tol = 1e-6;
ret.mu = 398600.4418;
end

%% Circular EqPro
function testCircularEqPro(inp)
roundtrip('Circular_EqPro.truth', inp);
end

function testCart2NonKepCircularEqPro(inp)
cart2nk('Circular_EqPro.truth', inp);
end

function testNonKep2CartCircularEqPro(inp)
nk2cart('Circular_EqPro.truth', inp);
end

%% Circular IncPro
function testCircularIncPro(inp)
roundtrip('Circular_IncPro.truth', inp);
end

function testCart2NonKepCircularIncPro(inp)
cart2nk('Circular_IncPro.truth', inp);
end

function testNonKep2CartCircularIncPro(inp)
nk2cart('Circular_IncPro.truth', inp);
end

%% Circular Polar
function testCircularPolar(inp)
roundtrip('Circular_Polar.truth', inp);
end

function testCart2NonKepCircularPolar(inp)
cart2nk('Circular_Polar.truth', inp);
end

function testNonKep2CartCircularPolar(inp)
nk2cart('Circular_Polar.truth', inp);
end

%% Circular IncRetro
function testCircularIncRetro(inp)
roundtrip('Circular_IncRetro.truth', inp);
end

function testCart2NonKepCircularIncRetro(inp)
cart2nk('Circular_IncRetro.truth', inp);
end

function testNonKep2CartCircularIncRetro(inp)
nk2cart('Circular_IncRetro.truth', inp);
end

%{
%% Circular EqRetro
function testCircularEqRetro(inp)
roundtrip('Circular_EqRetro.truth', inp);
end

function testCart2NonKepCircularEqRetro(inp)
cart2nk('Circular_EqRetro.truth', inp);
end

function testNonKep2CartCircularEqRetro(inp)
nk2cart('Circular_EqRetro.truth', inp);
end
%}

%% Elliptic EqPro
function testEllipticEqPro(inp)
roundtrip('Elliptic_EqPro.truth', inp);
end

function testCart2NonKepEllipticEqPro(inp)
cart2nk('Elliptic_EqPro.truth', inp);
end

function testNonKep2CartEllipticEqPro(inp)
nk2cart('Elliptic_EqPro.truth', inp);
end

%% Elliptic IncPro
function testEllipticIncPro(inp)
roundtrip('Elliptic_IncPro.truth', inp);
end

function testCart2NonKepEllipticIncPro(inp)
cart2nk('Elliptic_IncPro.truth', inp);
end

function testNonKep2CartEllipticIncPro(inp)
nk2cart('Elliptic_IncPro.truth', inp);
end

%% Elliptic Polar
function testEllipticPolar(inp)
roundtrip('Elliptic_Polar.truth', inp);
end

function testCart2NonKepEllipticPolar(inp)
cart2nk('Elliptic_Polar.truth', inp);
end

function testNonKep2CartEllipticPolar(inp)
nk2cart('Elliptic_Polar.truth', inp);
end

%% Elliptic IncRetro
function testEllipticIncRetro(inp)
roundtrip('Elliptic_IncRetro.truth', inp);
end

function testCart2NonKepEllipticIncRetro(inp)
cart2nk('Elliptic_IncRetro.truth', inp);
end

function testNonKep2CartEllipticIncRetro(inp)
nk2cart('Elliptic_IncRetro.truth', inp);
end

%{
%% Elliptic EqRetro
function testEllipticEqRetro(inp)
roundtrip('Elliptic_EqRetro.truth', inp);
end

function testCart2NonKepEllipticEqRetro(inp)
cart2nk('Elliptic_Polar.truth', inp);
end

function testNonKep2CartEllipticEqRetro(inp)
nk2cart('Elliptic_Polar.truth', inp);
end
%}

%{
Hintz states that e >= 1 is not supported.
%% Hyperbolic EqPro
function testHyperbolicEqPro(inp)
roundtrip('Hyperbolic_EqPro.truth', inp);
end

function testCart2NonKepHyperbolicEqPro(inp)
cart2nk('Hyperbolic_EqPro.truth', inp);
end

function testNonKep2CartHyperbolicEqPro(inp)
nk2cart('Hyperbolic_EqPro.truth', inp);
end

%% Hyperbolic IncPro
function testHyperbolicIncPro(inp)
roundtrip('Hyperbolic_IncPro.truth', inp);
end

function testCart2NonKepHyperbolicIncPro(inp)
cart2nk('Hyperbolic_IncPro.truth', inp);
end

function testNonKep2CartHyperbolicIncPro(inp)
nk2cart('Hyperbolic_IncPro.truth', inp);
end

%% Hyperbolic Polar
function testCart2NonKepHyperbolicPolar(inp)
cart2nk('Hyperbolic_Polar.truth', inp);
end

function testNonKep2CartHyperbolicPolar(inp)
nk2cart('Hyperbolic_Polar.truth', inp);
end

%% Hyperbolic IncRetro
function testCart2NonKepHyperbolicIncRetro(inp)
cart2nk('Hyperbolic_IncRetro.truth', inp);
end

function testNonKep2CartHyperbolicIncRetro(inp)
nk2cart('Hyperbolic_IncRetro.truth', inp);
end

%% Hyperbolic EqRetro
function testCart2NonKepHyperbolicEqRetro(inp)
cart2nk('Hyperbolic_EqRetro.truth', inp);
end

function testNonKep2CartHyperbolicEqRetro(inp)
nk2cart('Hyperbolic_EqRetro.truth', inp);
end
%}

%% Null Orbit
%{
function testCart2NonKepNull(inp)
cart2nk('Null.truth', inp);
end

function testNonKep2CartNull(inp)
nk2cart('Null.truth', inp);
end

function teardown(~)
rmpath('..');
end
%}

%% Common Functions
function cart2nk(tfilename, params)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    tfilename);
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
nkFF{1}(6) = nkFF{1}(6) * pi()/180;
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, params.mu), ...
    nkFF{1}, 'relative', params.tol);
end

function nk2cart(tfilename, params)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    tfilename);
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
nkFF{1}(6) = nkFF{1}(6) * pi()/180;
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, params.mu), ...
    cartFF{1}, 'relative', params.tol);
end

function roundtrip(tfilename, params)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    tfilename);
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    stateconv(stateconv(cartFF{1}, 1, 8, params.mu), 8, 1, params.mu), ...
    cartFF{1}, 'relative', params.tol);
end