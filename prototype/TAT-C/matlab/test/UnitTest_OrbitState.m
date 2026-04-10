%% Unit test script for Orbit State class

% Create the spacecraft state object and set Keplerian elements
state = OrbitState();

%  Set Keplerian and test conversion to Cartesian
state.SetKeplerianState(6900,0.002,pi/3,pi/4,pi/4,pi/4);
truthCart = [-2436.063522947054 2436.063522947055 5967.112612227063 ...
    -5.385803634090905 -5.378203080755706 0.009308738717021944]';
cartOut = state.GetCartesianState();
diff = norm(cartOut - truthCart);
if diff > 1e-11
    error('error in conversion from Keplerian to Cartesian')
end

% Set Cartesian state and test conversion to Keplerian
truthKep = [6900,0.002,pi/3,pi/4,pi/4,pi/4];
state.SetCartesianState(truthCart);
kepOut = state.GetKeplerianState();
diff = norm(kepOut - truthKep);
if diff > 1e-11
    error('error in conversion from Cartesian to Keplerian')
end