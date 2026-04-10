function LISAPlotFunction_snapier(traj)

global scaleUtill
pointobj     = traj.pointFunctionObject;
stateArray   = traj.phaseList{1}.DecVector.GetStateArray();
controlArray = traj.phaseList{1}.DecVector.GetControlArray();
timeArray    = traj.phaseList{1}.TransUtil.GetTimeVector();
%timeArray    = timeArray;%*(scaleUtill.earthScaling.TU/86400);

if isa(stateArray,'gradient')
    stateArray   = stateArray.x;
    controlArray = controlArray.x;
end


[rows, cols] = size(stateArray);

for ii=1:rows %length(stateArray)
    X1 = stateArray(ii,   1:6);
    X2 = stateArray(ii,  7:12);
    X3 = stateArray(ii, 13:18);

[~,~,~,~,~,~, s1(ii), s2(ii), s3(ii),~,~,~,...
    re1(ii), re2(ii), re3(ii), ang1(ii), ang2(ii), ang3(ii)] =  CalcSideData(X1, X2, X3);
end

%% Plots
clf('reset')


%% Ecliptic Inclination over time
for s=1:length(timeArray)
    xDim_LISA1  = pointobj.scaleUtil.UnScaleState(stateArray(s,1:6)',    3);
    xDim_LISA2  = pointobj.scaleUtil.UnScaleState(stateArray(s,7:12)',   3);
    xDim_LISA3  = pointobj.scaleUtil.UnScaleState(stateArray(s,13:end)', 3);
    [cos_rad_inc,lbb, ubb] = pointobj.constraint_eclipticInclination([xDim_LISA1; xDim_LISA2; xDim_LISA3], timeArray(s));
    inc1(s) = acos(cos_rad_inc(1))*(180/pi);
    inc2(s) = acos(cos_rad_inc(2))*(180/pi);
    inc3(s) = acos(cos_rad_inc(3))*(180/pi);
end

set(gcf,'WindowStyle','docked')
figure(1)
subplot(2,2,1)
hold on;
grid on;
xlabel('Time (years)');
ylabel('Ecliptic Inclination (degrees)');
plot(timeArray,inc1,'b')
plot(timeArray,inc2,'g')
plot(timeArray,inc3,'r')

hold off

%% Interior Angles over time
figure(1)
subplot(2,2,2) % SUBPLOT 2 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
hold on
grid on;
xlabel('Elapsed Years');
ylabel('Interior Angles (degrees)');

plot(timeArray, ang1, 'b');
plot(timeArray, ang2, 'g');
plot(timeArray, ang3, 'r');

plot(timeArray, 59.0*ones(length(ang3)), 'k');
plot(timeArray, 61.0*ones(length(ang3)), 'k');

ylim([58 62]);

%% Earth Distance Constraint
figure(1)
subplot(2,2,3) % SUBPLOT 3 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
hold on
grid on;
xlabel('Elapsed Years');
ylabel('Earth Distance (DU=5e7 km)');

plot(timeArray, re1, 'b');
plot(timeArray, re2, 'g');
plot(timeArray, re3, 'r');

%% Side Length Constraint
figure(1)
subplot(2,2,4) % SUBPLOT 3 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
hold on
grid on;
xlabel('Elapsed Years');
ylabel('Side length constraint: nominal=0.05');

plot(timeArray, s1, 'b');
plot(timeArray, s2, 'g');
plot(timeArray, s3, 'r');

plot(timeArray, 0.04*ones(length(s1)), 'k');
plot(timeArray, 0.06*ones(length(s1)), 'k');

%set(gcf,'WindowStyle','docked')
%figure(1)

%% Trajectories
%{
set(gcf,'WindowStyle','docked')
figure(2)
%subplot(2,2,1) % SUBPLOT 1 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
hold on
grid on;
axis equal
xlabel('X axis (km)');
ylabel('Y axis (km)');


% Plot Initial States
plot3( stateArray(1,1)*scaleUtill.earthScaling.DU,...
       stateArray(1,2)*scaleUtill.earthScaling.DU,...
       stateArray(1,3)*scaleUtill.earthScaling.DU,'k*');
plot3( stateArray(1,7)*scaleUtill.earthScaling.DU,...
       stateArray(1,8)*scaleUtill.earthScaling.DU,...
       stateArray(1,9)*scaleUtill.earthScaling.DU,'k*');
plot3(stateArray(1,13)*scaleUtill.earthScaling.DU,...
      stateArray(1,14)*scaleUtill.earthScaling.DU,...
      stateArray(1,15)*scaleUtill.earthScaling.DU,'k*');

% Plot LISA Trajectories
plot3( stateArray(:,1)*scaleUtill.earthScaling.DU,...  
       stateArray(:,2)*scaleUtill.earthScaling.DU,...  
       stateArray(:,3)*scaleUtill.earthScaling.DU,'b-');
plot3( stateArray(:,7)*scaleUtill.earthScaling.DU,... 
       stateArray(:,8)*scaleUtill.earthScaling.DU,...
       stateArray(:,9)*scaleUtill.earthScaling.DU,'g-');
plot3(stateArray(:,13)*scaleUtill.earthScaling.DU,...
      stateArray(:,14)*scaleUtill.earthScaling.DU,...
      stateArray(:,15)*scaleUtill.earthScaling.DU,'r-');

plot3(0, 0, 0,'ko', 'MarkerFaceColor', 'k');


%clf('reset')
%}
drawnow










