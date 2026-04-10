function timeVec = TestTimePartial(tLow,tHigh)

% Set the point index
pointIdx = 3;

% Compute nominal value
tLow = 10;
tHigh = 100;
nonDimPoints = [ 0.0 0.3 0.5 0.6 0.7 0.88 0.99 1.0];
timeVec = ComputeTimeVec(tLow,tHigh,nonDimPoints)
time = timeVec(pointIdx);
derivAnalytic = nonDimPoints(pointIdx);

%  Compute perturbed value
dt = .000001;
tLow = tLow + dt;
timeVec = ComputeTimeVec(tLow,tHigh,nonDimPoints);
timeNew= timeVec(pointIdx);

%  Compute derivative
dt3dtLow = (timeNew - time) / dt
derivAnalytic

function timeVec = ComputeTimeVec(tLow,tHigh,nonDimPoints)

timeVec = (tHigh - tLow)*nonDimPoints + tLow;