function OrbitRaisingPlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
stateArray = stateArray.x;
controlArray = traj.phaseList{1}.DecVector.GetControlArray(); 
controlArray = controlArray.x;

r      = stateArray(:,1);
theta  = stateArray(:,2);
uR     = controlArray(:,1);
uTheta = controlArray(:,2);

%  plot "Earth"
for i = 1:1:361
    xE(i) = 0.9*cos(i*pi/180);
    yE(i) = 0.9*sin(i*pi/180);
end
clf;
figure(2); hold on; 
axis([-2 2 -2 2]);
axis equal; grid on;
plot(xE,yE)

%  Plot orbit
for i = 1:length(r)
    x(i) = r(i)*cos(theta(i));
    y(i) = r(i)*sin(theta(i));
end

plot(x,y)
scale = .2;
for i = 1:length(uR)
    uStartx(i) = x(i);
    uStarty(i) = y(i);
    uStopx(i) = uStartx(i) + scale*uR(i)*cos(uTheta(i));
    uStopy(i) = uStarty(i) + scale*uR(i)*sin(uTheta(i));
    hold on;
    plot([uStartx(i);uStopx(i)],[uStarty(i);uStopy(i)],'r-','LineWidth',1.2);
end
%pause;
drawnow;