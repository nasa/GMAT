function UnitTest_CustomSensor()




close all
clear all
showPlots = true;


%% TROPICS Sensor

%  Use spherical law of cosines to compute clock angle
%  See here: https://en.wikipedia.org/wiki/Spherical_law_of_cosines
a = 57*pi/180;
b = 1.25*pi/180;
% C = pi/2, so sin(a)sin(b)*cos(C) is zero
cosc = cos(a)*cos(b);
c = acos(cosc);
coneAngle = c;

%  Use spherical law of sines to compute 
%  See here: https://en.wikipedia.org/wiki/Law_of_sines#Spherical_case
sinClock = sin(a)/sin(coneAngle)*sin(b)
clockAngle = asin(sinClock)

coneAngleVec = [coneAngle coneAngle coneAngle coneAngle coneAngle]'
clockAngleVec = [clockAngle, (pi - clockAngle), (pi + clockAngle), -clockAngle, clockAngle]'
tropicsSensor = CustomSensor(coneAngleVec,clockAngleVec);
if showPlots
    tropicsSensor.PlotSensorGeometry()
end
CppSyntax('tropicsClockAngle',coneAngleVec)
CppSyntax('tropicsConeAngle',clockAngleVec)
return
%% Big Square Shaped Sensor Test

% Set up the sensor
fac = 1.0;
squareClockAngle = [0 pi/4 pi/2 3*pi/4 pi 5*pi/4 6*pi/4 7*pi/4];
squareConeAngle =  fac*[5  7   5     7     5   7      5      7 ]*pi/180;
bigSquareSensor = CustomSensor(squareConeAngle,squareClockAngle );
CppSyntax('squareClockAngle',squareClockAngle)
CppSyntax('squareConeAngle',squareConeAngle)

% Try a point inside the FOV,, isInView should be true
viewConeAngle = 0.055;
viewClockAngle = 2*pi/4 - 0.001;
isInView = bigSquareSensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
if showPlots
    bigSquareSensor.PlotSensorGeometry()
    bigSquareSensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
    bigSquareSensor.PlotCandidatePoints()
end

% Try a point outside the FOV, isInView should be false
viewConeAngle = 2.0;
viewClockAngle = 0;
isInView = bigSquareSensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
if showPlots
    bigSquareSensor.PlotSensorGeometry()
    bigSquareSensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
    bigSquareSensor.PlotCandidatePoints()
end

%% Diamond Shaped Sensor Test

% Square Sensor
diamondClockAngle = [0 pi/4 pi/2 3*pi/4 pi 5*pi/4 6*pi/4 7*pi/4];
diamondConeAngle =  [5  2   5     2     5   2      5      2 ]*pi/180*3;
diamondSensor = CustomSensor(diamondConeAngle,diamondClockAngle );
CppSyntax('diamondClockAngle',diamondClockAngle)
CppSyntax('diamondConeAngle',diamondConeAngle)

% Try a point inside the FOV, isInView should be true
viewConeAngle = 0.21;
viewClockAngle = pi/2+.055;
isInView = diamondSensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
if showPlots
    diamondSensor.PlotSensorGeometry()
    diamondSensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
    diamondSensor.PlotCandidatePoints()
end

% Try a point inside the FOV, isInView should be false
viewConeAngle = 0.24;
viewClockAngle = pi/2+.055;
isInView = diamondSensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
if showPlots
    diamondSensor.PlotSensorGeometry()
    diamondSensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
    diamondSensor.PlotCandidatePoints()
end

%% Comb shaped sensor test
cl = 2;
cu = 6;
coords = [5 0
    5 cu
    4 cu
    4 cl
    3 cl
    3 cu
    2 cu
    2 cl
    1 cl
    1 cu
    0 cu
    0 cl
    -1 cl
    -1 cu
    -2 cu
    -2 cl
    -3 cl
    -3 cu
    -4 cu
    -4 cl
    -5 cl
    -5 cu
    -6 cu
    -6 0];

coords(:,2) = coords(:,2) - 1;

for pointIdx = 1:length(coords)
    combClockAngle(pointIdx,1) = mod(atan2(coords(pointIdx,2),coords(pointIdx,1)),2*pi);
    combConeAngle(pointIdx,1) = sqrt(coords(pointIdx,2)^2 + coords(pointIdx,1)^2)/20;
end
CppSyntax('combClockAngle',combClockAngle)
CppSyntax('combConeAngle',combConeAngle)
combSensor = CustomSensor(combConeAngle,combClockAngle);

% Try a point inside the FOV, isInView should be true
viewConeAngle = 0.21;
viewClockAngle = pi/2-.015;
isInView = combSensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
if showPlots
    combSensor.PlotSensorGeometry()
    combSensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
    combSensor.PlotCandidatePoints()
end

% Try a point outside the FOV, isInView should be false
viewConeAngle = 0.21;
viewClockAngle = pi/2+.055;
isInView = combSensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
if showPlots
    combSensor.PlotSensorGeometry()
    combSensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
    combSensor.PlotCandidatePoints()
end

%  Try a region in the FOV
showPlots = true;
regionConeAngleVec = [
    0.1047
    0.0436
    0.1047];
regionClockAngleVec = [  2.5409
    4.4608
    6.3807];
inView = combSensor.CheckRegionVisibility(regionConeAngleVec,regionClockAngleVec);
if showPlots
    combSensor.PlotRegionGeometry(regionConeAngleVec,regionClockAngleVec);
    axis equal
    grid on
end

%  Try a region NOT in the FOV
regionConeAngleVec  = [ 6.0 3.5 6.0 ]' *pi/180;
regionClockAngleVec = [-pi/9 pi/2+.3 (pi + pi/9)]';
inView = combSensor.CheckRegionVisibility(regionConeAngleVec,regionClockAngleVec);
if showPlots
    combSensor.PlotRegionGeometry(regionConeAngleVec,regionClockAngleVec);
    axis equal
    grid on
end

end

function CppSyntax(arrayName,arrayData)

numRows = size(arrayData,1);
numCols = size(arrayData,2);
disp([arrayName ' = new Rmatrix(' num2str(numRows) ',' num2str(numCols) ');']);
for rowIdx = 1:numRows
    for colIdx = 1:numCols
        lhsString = [arrayName '[' num2str(rowIdx-1) ',' num2str(colIdx-1) ']'];
        rhsString = num2str(arrayData(rowIdx,colIdx),16);
        disp([lhsString '=' rhsString ';']);
    end
end
end