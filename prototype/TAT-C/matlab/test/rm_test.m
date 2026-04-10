% This is Sreeja's RM call using zipfile m-files

clear; close; clc;
showPlots = false;
Re = 6378.1;

% Create the epoch object and set the initial epoch
date = AbsoluteDate();
date.SetGregorianDate(2016,4,18,0,0,0);

% Create 2 spacecraft state objects and set Keplerian elements
kepElems = [Re+710,0,98.2,0,0,0]; % (SMA,ECC,INC,RAAN,AOP,TA) in km and radians
state1 = OrbitState();
state1.SetKeplerianState(kepElems(1),kepElems(2),kepElems(3),kepElems(4),kepElems(5),kepElems(6));

kepElems = [Re+710,0,98.2,0,0,pi]; % (SMA,ECC,INC,RAAN,AOP,TA) in km and radians
state2 = OrbitState();
state2.SetKeplerianState(kepElems(1),kepElems(2),kepElems(3),kepElems(4),kepElems(5),kepElems(6));

% Create 2 spacecraft giving them a state and epoch
sat1 = Spacecraft(date,state1);
prop1 = Propagator(sat1);
sat2 = Spacecraft(date,state2);
prop2 = Propagator(sat2);

% Create the point group and initialize the coverage checker
pGroup = PointGroup('Helical',200);
numPoints = pGroup.GetNumPoints();
if showPlots
    figHandle = pGroup.PlotAllTestPoints();
end

% Create Lat\Lon Grid for the created points
for pointIdx = 1:pGroup.GetNumPoints()
    vec = pGroup.GetPointPositionVector(pointIdx);
    lonVec(pointIdx,1) = atan2(vec(2),vec(1))*180/pi;
    latVec(pointIdx,1) = asin(vec(3)/norm(vec))*180/pi;
end

% Initialize the coverage checker for both sats
covChecker1 = CoverageChecker(pGroup,sat1);
covChecker2 = CoverageChecker(pGroup,sat2);

% Propagate for a duration and collect data
startDate = date.GetJulianDate();
count = 0;
while date.GetJulianDate() < startDate + 7 % Propagate for N days
    
    % Propagate
    date.Advance(60); % Advance by timeStepInSeconds
    
    prop1.Propagate(date);
    % Compute points in view
    accessPoints1 = covChecker1.AccumulateCoverageData();
    
    prop2.Propagate(date);
    % Compute points in view
    accessPoints2 = covChecker2.AccumulateCoverageData();
    
    % Plotting stuff used for testing and visualization
    if showPlots
        % Save orbit for plotting // not needed now
    end
    
end

% Compute coverage data. Number of cells is equal to number of events?
coverageEvents1 = covChecker1.ProcessCoverageDate();
coverageEvents2 = covChecker2.ProcessCoverageDate();

% Evaluate the coverage events
pointPasses1 = zeros(pGroup.GetNumPoints(),1);
pointDuration = zeros(pGroup.GetNumPoints(),1);
pointWhen1 = cell(pGroup.GetNumPoints(),1);

for eventIdx = 1:length(coverageEvents1)
    currEvent = coverageEvents1{eventIdx};
    poiIndex = currEvent.poiIndex;
    eventDuration = (currEvent.endDate.GetJulianDate() - currEvent.startDate.GetJulianDate())*24; % in hours
    
    pointPasses1(poiIndex) = pointPasses1(poiIndex) + 1;
    pointDuration(poiIndex) = pointDuration(poiIndex) + eventDuration; % in hours
    pointWhen1{poiIndex} = [pointWhen1{poiIndex} [currEvent.startDate.GetJulianDate();currEvent.endDate.GetJulianDate()]];
end
pointAverageDuration1 = pointDuration/pointPasses1;

pointPasses2 = zeros(pGroup.GetNumPoints(),1);
pointDuration = zeros(pGroup.GetNumPoints(),1);
pointWhen2 = cell(pGroup.GetNumPoints(),1);
for eventIdx = 1:length(coverageEvents2)
    currEvent = coverageEvents2{eventIdx};
    poiIndex = currEvent.poiIndex;
    eventDuration = (currEvent.endDate.GetJulianDate() - currEvent.startDate.GetJulianDate())*24; % in hours
    
    pointPasses2(poiIndex) = pointPasses2(poiIndex) + 1;
    pointDuration(poiIndex) = pointDuration(poiIndex) + eventDuration; % in hours
    pointWhen2{poiIndex} = [pointWhen2{poiIndex} [currEvent.startDate.GetJulianDate();currEvent.endDate.GetJulianDate()]];
end
pointAverageDuration2 = pointDuration/pointPasses2;

% Evaluate the  ground points' metrics for the constellation
pointWhen = cell(pGroup.GetNumPoints(),1);
pointAverageRevisit = zeros(pGroup.GetNumPoints(),1);
pointFirst = zeros(pGroup.GetNumPoints(),1);
pointPasses = pointPasses1 + pointPasses2;

for poiIndex = 1:numPoints
    if isempty([pointWhen1{poiIndex} pointWhen2{poiIndex}])
        pointWhen{poiIndex} = [];
    else
        pointWhen{poiIndex} = (sortrows([pointWhen1{poiIndex} pointWhen2{poiIndex}]',1))';
    end
    
    if isempty(pointWhen{poiIndex})
        pointFirst(poiIndex) = NaN;
    else
        pointFirst(poiIndex) = pointWhen{poiIndex}(1);
    end
    
    gap = zeros(pointPasses(poiIndex)-1,1);
    for pass = 1:pointPasses(poiIndex)-1
        gap(pass) = pointWhen{poiIndex}(1,pass+1) - pointWhen{poiIndex}(2,pass);
    end
    pointAverageRevisit(poiIndex) = mean(gap)*24; % in hours
end
% check = [pointAverageRevisit pointPasses 24*7./pointPasses];

if sum(isnan(pointFirst)) > 0
    timeToFullCoverage = NaN;
else
    timeToFullCoverage = (nanmax(pointFirst)-startDate)*24; % in hours
end