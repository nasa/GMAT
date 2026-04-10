function test_suite = testKep()                             %#ok<STOUT>
%TESTMEE Unit tests for Keplerian state representation

initTestSuite();

end

function ret = setup()                                      %#ok<*DEFNU>
addpath('..');
tol = 1e-7;
mu = 398600.4418;
ret = {tol, mu};
end

%% Elliptic EqPro
function testEllipticEqPro(inp)
cart = [-288.2349803269244, -19027.83272806163, 0, ...
    4.845817348531929, 4.067995228947069, 0];
kep = [2.1415960242660699e+005, 9.5022204883835470e-001, 0, ...
    6.1705435383455072e+000, 0, 4.8098838356971196e+000];
assertElementsAlmostEqual( ...
    Cart2Kep(Kep2Cart(kep, inp{2}), inp{2}), ...
    kep, 'relative', inp{1});
end

function testCart2KepEllipticEqPro(inp)
cart = [-288.2349803269244, -19027.83272806163, 0, ...
    4.845817348531929, 4.067995228947069, 0];
kep = [2.1415960242660699e+005, 9.5022204883835470e-001, 0, ...
    6.1705435383455072e+000, 0, 4.8098838356971196e+000];
assertElementsAlmostEqual(Cart2Kep(cart, inp{2}), kep, 'relative', inp{1});
end

function testKep2CartEllipticEqPro(inp)
cart = [-288.2349803269244, -19027.83272806163, 0, ...
    4.845817348531929, 4.067995228947069, 0];
kep = [2.1415960242660699e+005, 9.5022204883835470e-001, 0, ...
    6.1705435383455072e+000, 0, 4.8098838356971196e+000];
assertElementsAlmostEqual(Kep2Cart(kep, inp{2}), cart', 'relative', inp{1});
end

%% Elliptic EqRetro
function testEllipticEqRetro(inp)
cart = [8.0000000000000000e+003, 0, 0, ...
    0, -7.7324036541039414e+000, 0];
kep = [10000, 0.2, pi(), 0, 0, 0];
assertElementsAlmostEqual( ...
    Cart2Kep(Kep2Cart(kep, inp{2}), inp{2}), ...
    kep, 'relative', inp{1});
end

function testCart2KepEllipticEqRetro(inp)
cart = [8.0000000000000000e+003, 0, 0, ...
    0, -7.7324036541039414e+000, 0];
kep = [10000, 0.2, pi(), 0, 0, 0];
assertElementsAlmostEqual(Cart2Kep(cart, inp{2}), kep, 'relative', inp{1});
end

function testKep2CartEllipticEqRetro(inp)
cart = [8.0000000000000000e+003, 0, 0, ...
    0, -7.7324036541039414e+000, 0];
kep = [10000, 0.2, pi(), 0, 0, 0];
assertElementsAlmostEqual(Kep2Cart(kep, inp{2}), cart', 'relative', inp{1});
end

%% Utility functions
function teardown(~)
rmpath('..');
end