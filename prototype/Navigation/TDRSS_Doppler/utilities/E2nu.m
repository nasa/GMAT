
function nu = E2nu(E,e)
% Inputs: Eccentric Anomaly (Degrees)
%         Eccentricity 
%
% Outputs: True anomaly  (degrees between 0 and 360)

% Convert mean anomaly to true anomaly
E = d2r(E);

% avoid quadrant abiguity
nu = atan2((sin(E)*sqrt(1-e^2)), (cos(E) -e ));

% constrain between 0 and 2pi
nu = mod(nu,2*pi);

nu = r2d(nu);

end