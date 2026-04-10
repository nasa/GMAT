%% This is a usage example that drives O-C code and computes standard
%  statistical products typical of O-C analysis.  This script is an example
%  of how R-M might use the O-C data.

clear all; close all; clc;
showPlots = true;
% Create the epoch object and set the initial epoch
date = AbsoluteDate();
date.SetGregorianDate(2017,1,15,22,30,20.111);

% Create the spacecraft state object and set Keplerian elements
state = OrbitState();
state.SetKeplerianState(6700,0.002,90*pi/180,pi/4 + pi/6,.2345,pi/6);

% Create a spacecraft giving it a state and epoch
sat1 = Spacecraft(date,state);

% Create the propagator
prop = Propagator(sat1);

% Create the point group and initialize the coverage checker
pGroup = PointGroup();
pGroup.AddHelicalPointsByNumPoints(200)
covChecker = CoverageChecker(pGroup,sat1);
if showPlots
    figHandle = pGroup.PlotAllTestPoints();
end

% Propagate for a duration and collect data
startDate = date.GetJulianDate();
count = 0;
while date.GetJulianDate() < startDate + 1
    
    % Propagate
    date.Advance(20);
    prop.Propagate(date);
    
    % Compute points in view
    accessPoints = covChecker.AccumulateCoverageData();
    
    orbitState = sat1.GetOrbitState();
    cartesianState = orbitState.GetCartesianState();
    keplerianState = orbitState.GetKeplerianState();
    
    %% Plotting stuff used for testing and visualization
    if showPlots
        % Save orbit for plotting. Kludge cause output classes not done yet.
        count = count + 1;
        orbEpoch = sat1.GetOrbitEpoch();
        jDate = orbEpoch.GetJulianDate();
        
        cb = Earth();
        orbitState(count,:) = cb.GetBodyFixedState(cartesianState(1:3,1),jDate)';
        pGroup.PlotSelectedTestPoints(accessPoints); drawnow;
        if count >= 2
            figure(figHandle); hold on;
            plot3(orbitState(count-1:count,1)/6378,orbitState(count-1:count,2)/6378,...
                orbitState(count-1:count,3)/6378,'k-','MarkerSize',3)
        end
    end
    
end

% Compute coverage data
coverageEvents = covChecker.ProcessCoverageEvents();

%%  Compute coverate stats.  Shows how R-M might use data for coverage analysis

% Create Lat\Lon Grid
for pointIdx = 1:pGroup.GetNumPoints()
    vec = pGroup.GetPointPositionVector(pointIdx);
    lonVec(pointIdx,1) = atan2(vec(2),vec(1))*180/pi;
    latVec(pointIdx,1) = asin(vec(3)/norm(vec))*180/pi;
end

% Compute total coverage statistics from all coverage events
totalCoverageDuration = zeros(pGroup.GetNumPoints(),1);
numPassVec = zeros(pGroup.GetNumPoints(),1);
minPassVec = numPassVec;
maxPassVec = numPassVec;
for eventIdx = 1:length(coverageEvents)
    currEvent = coverageEvents{eventIdx};
    poiIndex = currEvent.poiIndex;
    eventDuration = (currEvent.endDate.GetJulianDate() - currEvent.startDate.GetJulianDate())*24;
    totalCoverageDuration(poiIndex) = totalCoverageDuration(poiIndex) + eventDuration;
    vec = pGroup.GetPointPositionVector(pointIdx);
    %     lat = atan2(vec(2),vec(1))*180/pi;
    %     lon = asin(vec(3)/norm(vec));
    %     Z(lat,lon) = totalCoverageDuration;
    
    % Save the maximum duration if necessary
    if eventDuration > maxPassVec(poiIndex)
        maxPassVec(poiIndex) = eventDuration;
    end
    
    if minPassVec(poiIndex) == 0 ||  (eventDuration< maxPassVec(poiIndex))
        minPassVec(poiIndex) = eventDuration;
    end
    numPassVec(poiIndex) = numPassVec(poiIndex) + 1;
end



%%  Write the simple coverage report to the MATLAB command window
disp('       =======================================================================')
disp('       ==================== Brief Coverage Analysis Report ===================')
disp('       lat (deg): Latitude of point in degrees                  ')
disp('       lon (deg): Longitude of point in degrees                  ')
disp('       numPasses: Number of total passes seen by a point                           ')
disp('       totalDur : Total duration point was observed in hours                         ');
disp('       minDur   : Duration of the shortest pass in minutes                         ');
disp('       maxDur   : Duration of the longest pass in hours                            ')
disp('       =======================================================================')
disp('       =======================================================================')
disp('  ')

format short g
data = [latVec,lonVec, numPassVec, totalCoverageDuration, minPassVec, maxPassVec];
headerCount = 1;
for passIdx = 1:10:pGroup.GetNumPoints()
    disp('       lat (deg)    lon (deg)     numPasses   totalDur    minDur      maxDur')
    dataEnd = passIdx + 9;
    disp(data(passIdx:dataEnd,:))
end
if dataEnd + 1 < pGroup.GetNumPoints()
    disp('       lat (deg)    lon (deg)     numPasses   totalDur    minDur      maxDur')
    disp(data(dataEnd + 1 :end,:))
end

figure(2)
subplot(3,1,1)
xlin= -180:5:180;
ylin= -90:5:90;
[X,Y]=meshgrid(xlin,ylin);
Z=griddata(lonVec,latVec,maxPassVec*60,X,Y,'cubic');
h = contour(X,Y,Z);
clabel(h);
grid on
subplot(3,1,2)
xlin= -180:5:180;
ylin= -90:5:90;
[X,Y]=meshgrid(xlin,ylin);
Z=griddata(lonVec,latVec,minPassVec*60,X,Y,'cubic');
h = contour(X,Y,Z);
clabel(h);
grid on
subplot(3,1,3)
xlin= -180:5:180;
ylin= -90:5:90;
[X,Y]=meshgrid(xlin,ylin);
Z=griddata(lonVec,latVec,totalCoverageDuration*60,X,Y,'cubic');
h = contour(X,Y,Z);
clabel(h);
grid on