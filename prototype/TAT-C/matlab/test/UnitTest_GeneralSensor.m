%clear all
sensorType = 'Comb'
switch sensorType
    % Conical Sensor
    case 'Big'
        fixedConeAngle = pi/2 + 1.0;
        clockAngle = 0:.3:2*pi;
        coneAngle = fixedConeAngle*ones(length(clockAngle),1); 
    case 'Conical'
        fixedConeAngle = 16*pi/180;
        clockAngle = 0:.3:2*pi;
        coneAngle = fixedConeAngle*ones(length(clockAngle),1);
    case 'Square'
        % Square Sensor
        clockAngle = [0 pi/4 pi/2 3*pi/4 pi 5*pi/4 6*pi/4 7*pi/4];
        coneAngle =  [5  7   5     7     5   7      5      7 ]*pi/180*3;
    case 'Big Square'
        % Square Sensor
        fac = 20.0
        clockAngle = [0 pi/4 pi/2 3*pi/4 pi 5*pi/4 6*pi/4 7*pi/4];
        coneAngle =  fac*[5  7   5     7     5   7      5      7 ]*pi/180;
    case 'Diamond'
        % Square Sensor
        clockAngle = [0 pi/4 pi/2 3*pi/4 pi 5*pi/4 6*pi/4 7*pi/4];
        coneAngle =  [5  2   5     2     5   2      5      2 ]*pi/180*3;
    case 'Comb'
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
        %         figure(2)
        %         for pointIdx = 1:length(coords)-1
        %             hold on
        %             plot([coords(pointIdx,1) coords(pointIdx+1,1)]', [coords(pointIdx,2) coords(pointIdx+1,2)]')
        %         end
        
        for pointIdx = 1:length(coords)
            clockAngle(pointIdx,1) = mod(atan2(coords(pointIdx,2),coords(pointIdx,1)),2*pi);
            coneAngle(pointIdx,1) = sqrt(coords(pointIdx,2)^2 + coords(pointIdx,1)^2)/20;
        end
end
viewConeAngle = 0.055;
viewClockAngle = 2*pi/4 - 0.001;
sensor = CustomSensor(coneAngle,clockAngle);
isInView = sensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
sensor.PlotSensorGeometry()
sensor.PlotTargetGeometry(viewConeAngle,viewClockAngle)
sensor.PlotCandidatePoints()

% Special case failure mode
viewConeAngle = 5*pi/180;
viewConeAngle = 2.55;
viewClockAngle = pi/4;

% viewConeAngle = 5*pi/180;
% viewClockAngle = 0.5*pi/4.2;
% isInView = sensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
% sensor.PlotTargetGeometry(viewConeAngle,viewClockAngle);


%v = VideoWriter('c:\Temp\bRegionCheck4','MPEG-4');
%v.open;
for i = 0:0.01:2*pi
    regionConeAngleVec  = [ 6.0 2.5 6.0 ]' *pi/180;
    regionClockAngleVec = [-pi/9 pi/2 (pi + pi/9)]' +i
    inView = sensor.CheckRegionVisibility(regionConeAngleVec,regionClockAngleVec);
    sensor.PlotRegionGeometry(regionConeAngleVec,regionClockAngleVec);
    axis equal
    grid on
    drawnow
    axis([-.2 .2 -.2 .2])
    if inView
        xsx= 5;
    end
    %     figure(1)
   % mFrames(pointIdx) = getframe;
    %[X,map] = frame2im(getframe);
    %writeVideo(v,X)
end
v.close()
return

xVec = -7:.1:7;
numXValues =  length(xVec);
dy = (5-3)/(numXValues-1);
yVec = 3:dy:5;
v = VideoWriter('c:\Temp\bmovie2.avi','MPEG-4');
v.open;
for pointIdx = 1:length(xVec)
    viewClockAngle = atan2(yVec(pointIdx),xVec(pointIdx));
    viewConeAngle  = sqrt(yVec(pointIdx)^2 + xVec(pointIdx)^2)/20;
    isInView = sensor.CheckTargetVisibility(viewConeAngle,viewClockAngle);
    sensor.PlotTargetGeometry(viewConeAngle,viewClockAngle);
    axis([-.2 .2 -.2 .2])
    axis equal
    grid on
    drawnow
    %     figure(1)
    mFrames(pointIdx) = getframe;
    [X,map] = frame2im(getframe);
    writeVideo(v,X)
end
v.close()
close all
%movie(mFrames)

regionConeAngleVec  = [ 5 2 5 ]' *pi/180;
regionClockAngleVec = [-pi/9 pi/2 (pi + pi/9)]'
sensor.CheckRegionVisibility(regionConeAngleVec,regionClockAngleVec)
sensor.PlotRegionGeometry(regionConeAngleVec,regionClockAngleVec)

%
% v = VideoWriter('c:\Temp\movie.avi')