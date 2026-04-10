
% Create a point group object with 50 points
testPointNum = 50;
pg = PointGroup();
pg.AddHelicalPointsByNumPoints(testPointNum);

% Test stored number of points
numPoints = pg.GetNumPoints();
if numPoints ~= testPointNum
    error('error in returned value for num points')
end

% Test the returned values for point locations.  Note
% this is a semi-rigouros test.  I plotted the points and they look correct,
% and I then outputted the data and am using it as truth.
truthData = [
    0                         0                         1
    0                         0                        -1
    0.433883739117558                         0         0.900968867902419
    0.134077448970272          0.41264795740226         0.900968867902419
    -0.351019318529051         0.255030463062816         0.900968867902419
    -0.351019318529051        -0.255030463062815         0.900968867902419
    0.134077448970272         -0.41264795740226         0.900968867902419
    0.433883739117558                         0        -0.900968867902419
    0.134077448970272          0.41264795740226        -0.900968867902419
    -0.351019318529051         0.255030463062816        -0.900968867902419
    -0.351019318529051        -0.255030463062815        -0.900968867902419
    0.134077448970272         -0.41264795740226        -0.900968867902419
    0.78183148246803                         0         0.623489801858733
    0.552838342998275         0.552838342998275         0.623489801858733
    4.78733711238551e-17          0.78183148246803         0.623489801858733
    -0.552838342998275         0.552838342998275         0.623489801858733
    -0.78183148246803      9.57467422477103e-17         0.623489801858733
    -0.552838342998275        -0.552838342998275         0.623489801858733
    -1.43620113371565e-16         -0.78183148246803         0.623489801858733
    0.552838342998275        -0.552838342998275         0.623489801858733
    0.78183148246803                         0        -0.623489801858733
    0.598917662600107         0.502551589793308        -0.623489801858733
    0.135763612173208         0.769953705483544        -0.623489801858733
    -0.390915741234015         0.677085925295762        -0.623489801858733
    -0.734681274773315         0.267402115690236        -0.623489801858733
    -0.734681274773315        -0.267402115690236        -0.623489801858733
    -0.390915741234015        -0.677085925295762        -0.623489801858733
    0.135763612173208        -0.769953705483544        -0.623489801858733
    0.598917662600107        -0.502551589793309        -0.623489801858733
    0.974927912181824                         0         0.222520933956314
    0.820161550378687          0.52708582340226         0.222520933956314
    0.404999691314914         0.886825622084767         0.222520933956314
    -0.138746708150268          0.96500455176578         0.222520933956314
    -0.638442008115133         0.736801354657499         0.222520933956314
    -0.935436481519112         0.274668933435062         0.222520933956314
    -0.935436481519112        -0.274668933435062         0.222520933956314
    -0.638442008115134        -0.736801354657499         0.222520933956314
    -0.138746708150268         -0.96500455176578         0.222520933956314
    0.404999691314914        -0.886825622084767         0.222520933956314
    0.820161550378687         -0.52708582340226         0.222520933956314
    0.974927912181824                         0        -0.222520933956314
    0.788733249245582         0.573048248828767        -0.222520933956314
    0.30126929315467         0.927211543798553        -0.222520933956314
    -0.30126929315467         0.927211543798553        -0.222520933956314
    -0.788733249245582         0.573048248828767        -0.222520933956314
    -0.974927912181824      1.19394234705288e-16        -0.222520933956314
    -0.788733249245582        -0.573048248828767        -0.222520933956314
    -0.30126929315467        -0.927211543798553        -0.222520933956314
    0.30126929315467        -0.927211543798553        -0.222520933956314
    0.788733249245582        -0.573048248828767        -0.222520933956314]*6378.1363;
maxDiff = -inf;
for pointIdx = 1:testPointNum
    diff = norm( truthData(pointIdx,:)' -  pg.GetPointPositionVector(pointIdx));
    if diff > maxDiff
        maxDiff = diff;
    end
end
if maxDiff >= 1e-11
    error('error in returned value for num points')
end

% Test lat/lon contraint setting
latUpper = pi/3;
latLower = -pi/3;
lonUpper = 2*pi - pi/6;
lonLower = pi/6;
pg = PointGroup();
pg.SetLatLonBounds(latUpper,latLower,lonUpper,lonLower);
pg.AddHelicalPointsByNumPoints(testPointNum);


[latVec,lonVec] = pg.GetLatLonVectors();
for pointIdx = 1:pg.GetNumPoints()
    if (latVec(pointIdx) < latLower || latVec(pointIdx) > latUpper) || ...
            (lonVec(pointIdx) < lonLower || lonVec(pointIdx) > lonUpper)
        error('latitude and or longitude violates constraint')
    end
end

%  Test setting your own lat lon using those from the previous test
pgCustom = PointGroup();
pgCustom.AddUserDefinedPoints(latVec,lonVec);
if pg.GetNumPoints() ~= pgCustom.GetNumPoints()
    error('error setting user defined points)')
end
[latVec2,lonVec2] = pgCustom.GetLatLonVectors();
for pointIdx = 1:pgCustom.GetNumPoints()
    if latVec2(pointIdx) ~= latVec(pointIdx)
        error('error setting user defined points)')
    end
    if lonVec2(pointIdx) ~= lonVec(pointIdx)
        error('error setting user defined points)')
    end
end

%  Test setting points based on angular separation
pgByAngle = PointGroup();
angle = 1*pi/180;
pgByAngle.AddHelicalPointsByAngle(angle);
v1 = pgByAngle.GetPointPositionVector(3);
v2 = pgByAngle.GetPointPositionVector(4);
angleOut = acos(dot(v1,v2)/norm(v1)/norm(v2));
if abs(angle - angleOut)>1e-6
    error('error in angle between points when setting based on angle')
end


