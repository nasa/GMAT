function BrachistichronePlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();

controlArray = traj.phaseList{1}.DecVector.GetControlArray();

if isa(stateArray,'gradient')
    stateArray = stateArray.x;
    controlArray = controlArray.x;
end

r  = stateArray(:,1);
theta  = stateArray(:,2);
clf;
%polar(theta,r);
%axis([0 2 -2 0]); grid on;
xlabel('X Coordinate');ylabel('Y Coordinate')
drawnow;
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
axis([-3.5 4.5 -3.5 4.5]);
title('Optimal Spiral Transfer (Energy Change)')
xlabel('X-axis, non-dimensional')
ylabel('Y-axis, non-dimensional')
grid on;
axis equal;