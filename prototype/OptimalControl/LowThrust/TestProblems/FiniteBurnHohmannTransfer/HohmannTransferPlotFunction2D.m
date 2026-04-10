function HohmannTransferPlotFunction2D(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
stateArray = stateArray.x;
controlArray = traj.phaseList{1}.DecVector.GetControlArray(); 
controlArray = controlArray.x;

x      = stateArray(:,1);
y      = stateArray(:,2);
ux      = controlArray(:,1);
uy      = controlArray(:,2);

%[Xe,Ye,Ze] = sphere(25);

figure(4); clf; hold on; 
%surf(Xe,Ye,Ze);  
plot(x,y);
axis([-2 2 -2 2 ]); axis equal;

cnt = 0; d2r = pi/180;
xe = zeros(72,1);
ye = zeros(72,1);
for theta = 1:5:360
    cnt = cnt + 1;
    xe(cnt) = cos(theta*d2r*pi);
    ye(cnt) = sin(theta*d2r*pi);
end
plot(xe,ye,'g-');

scale = .2;
uStopx = zeros(length(ux),1);
uStopy = zeros(length(ux),1);
uStartx = zeros(length(ux),1);
uStarty = zeros(length(ux),1);
for i = 1:length(ux)
    uStartx(i) = x(i);
    uStarty(i) = y(i);
    uStopx(i) = uStartx(i) + scale*ux(i);
    uStopy(i) = uStarty(i) + scale*uy(i);
    plot([uStartx(i);uStopx(i)],[uStarty(i);uStopy(i)],'r-','LineWidth',1.2);
end
%pause
drawnow;