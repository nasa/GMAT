function test_suite = testMee()                             %#ok<STOUT>
%TESTMEE Unit tests for Modified Equinoctial state representation

initTestSuite();

end

function ret = setup()                                      %#ok<*DEFNU>
addpath('..');
tol = 1e-7;
mu = 398600.4418;
ret = {tol, mu};
end

%% Circular EqPro
function testCircularEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeCircularEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Circular IncPro
function testCircularIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual(...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeCircularIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Circular Polar
function testCircularPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeCircularPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Circular IncRetro
function testCircularIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCircularIncRetroRetro(inp)
roundtrip('Circular_IncRetro.truth', inp, -1);
end

function testCart2MeeCircularIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Circular EqRetro
function testCircularEqRetro(inp)
roundtrip('Circular_EqRetro.truth', inp, -1);
end

%% Elliptic EqPro
function testEllipticEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeEllipticEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic IncPro
function testEllipticIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeEllipticIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic Polar
function testEllipticPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeEllipticPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic IncRetro
function testEllipticIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testEllipticIncRetroRetro(inp)
roundtrip('Elliptic_IncRetro.truth', inp, -1);
end

function testCart2MeeEllipticIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic EqRetro
function testEllipticEqRetro(inp)
roundtrip('Elliptic_EqRetro.truth', inp, -1);
end

%% Hyperbolic EqPro
function testHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic IncPro
function testHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic Polar
function testHyperbolicPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testCart2MeeHyperbolicPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic IncRetro
function testHyperbolicIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

function testHyperbolicIncRetroRetro(inp)
roundtrip('Hyperbolic_IncRetro.truth', inp, -1);
end

function testCart2MeeHyperbolicIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicIncRetro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncRetro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic EqRetro
function testHyperbolicEqRetro(inp)
roundtrip('Hyperbolic_EqRetro.truth', inp, -1);
end

%% Parabolic EqPro
function testParabolicEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Parabolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

%% Parabolic IncPro
function testParabolicIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Parabolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}), inp{2}), ...
    cartFF{1}, 'relative', inp{1});
end

%% Parabolic Polar
function testParabolicPolar(inp)
roundtrip('Parabolic_Polar.truth', inp, 1);
end

%% Parabolic IncRetro
function testParabolicIncRetro(inp)
roundtrip('Parabolic_IncRetro.truth', inp, 1);
end

function testParabolicIncRetroRetro(inp)
roundtrip('Parabolic_IncRetro.truth', inp, -1);
end

%% Parabolic EqRetro
function testParabolicEqRetro(inp)
roundtrip('Parabolic_EqRetro.truth', inp, -1);
end

%% Null
function testNull(inp)
roundtrip('Null.truth', inp, 1);
end

function testNullRetro(inp)
roundtrip('Null.truth', inp, -1);
end

function testCart2MeeNull(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Null.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartNull(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Null.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
meeFF{1}(6) = meeFF{1}(6) * pi()/180;
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

function teardown(~)
rmpath('..');
end

function roundtrip(truth, inp, retro)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', truth);
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
fclose(fid);
assertElementsAlmostEqual( ...
    Mee2Cart(Cart2Mee(cartFF{1}, inp{2}, retro), inp{2}, retro), ...
    cartFF{1}, 'relative', inp{1});
end