function ConwaySpiralPlotFunction(traj)



global configEarth configSun

DU = configSun.DU;

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
controlArray = traj.phaseList{1}.DecVector.GetControlArray();

if isa(stateArray,'gradient')
    stateArray = stateArray.x;
    controlArray = controlArray.x;
end

% Redimensionalize and convert to Cartesian for plotting
hold on
grid on;
set(gcf,'color','w');
xlabel('\bf X-Axis [AU]', 'interpreter', 'latex');
ylabel('\bf Y-Axis [AU]', 'interpreter', 'latex');
for i = 1:size(stateArray,1)
   modEq = configSun.StateUnScaleMat(1:6,1:6)*stateArray(i,1:6)';
   cart(i,:) = Mee2Cart( modEq,configSun.gravParam)';
   plot3(cart(:,1)/DU,cart(:,2)/DU,cart(:,3)/DU,'k*');
end

plot3(cart(:,1)/DU,cart(:,2)/DU,cart(:,3)/DU,'k*:');
hold on;
plot3(0, 0, 0,'ko','MarkerFaceColor', 'k');
drawnow
axis equal
return
r  = stateArray(:,1);
theta  = stateArray(:,2);
clf('reset')
axis([-3 3 -3 3]);
title('Optimal Spiral Transfer (Energy Change)')
xlabel('X-axis, non-dimensional')
ylabel('Y-axis, non-dimensional')
grid on; hold on
%polar(theta,r);
%axis([0 2 -2 0]); grid on;

%pause

%  Plot orbit
for i = 1:length(r)
    x(i) = r(i)*cos(theta(i));
    y(i) = r(i)*sin(theta(i));
end

for i = 1:length(r)
    xB(i) = cos(theta(i));
    yB(i) = sin(theta(i));
end

scale = 20;
plot(x,y); hold on;
plot(x,y,'g-')
uR = 0.01;
for i = 1:length(controlArray)-1
    uStartx(i) = x(i);
    uStarty(i) = y(i);
    uStopx(i) = uStartx(i) + scale*uR*cos(theta(i)+ pi/2 - controlArray(i));
    uStopy(i) = uStarty(i) + scale*uR*sin(theta(i)+ pi/2 - controlArray(i));
    hold on;
    plot([uStartx(i);uStopx(i)],[uStarty(i);uStopy(i)],'r-','LineWidth',1.2);
end

cnt = 0;
for ang = 0:.1:2*pi
    cnt = cnt+1;
    xBound(cnt) = 1.6*cos(ang);
    yBound(cnt) = 1.6*sin(ang);
end
hold on; plot(xBound,yBound,'k-')
cnt = 0;
for ang = 0:.1:2*pi
    cnt = cnt+1;
    xBound(cnt) = 1.0*cos(ang);
    yBound(cnt) = 1.0*sin(ang);
end
hold on; plot(xBound,yBound,'k-')
title('Optimal Spiral Transfer (Energy Change)')
xlabel('X-axis, non-dimensional')
ylabel('Y-axis, non-dimensional')
grid on;
axis equal
axis([-3 3 -3 3]);
drawnow;