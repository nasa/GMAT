
function E = KepEqtnE(M,e)
% Convert mean anomaly to true anomaly
%
% Inputs: M =  Mean anomaly in degrees
%         e = eccentricity
% Outputs: E = eccentric anomaly degrees

M = d2r(M);

if -pi < M <0 || M > pi
    En = M-e;
else
    En = M + e;
end

% set tolerance to something high
tol=pi;

while tol > sqrt(eps)
    
    E = En + (M - En + e *sin(En))/(1-e*cos(En));
    
    tol = abs(E-En);

    En =E;

end

E = r2d(E);
end
