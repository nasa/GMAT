function OrbitRaisingPlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
stateArray = stateArray.x;
controlArray = traj.phaseList{1}.DecVector.GetControlArray(); 
controlArray = controlArray.x;

x      = stateArray(:,1);
y      = stateArray(:,2);
z      = stateArray(:,3);
ux      = controlArray(:,1);
uy      = controlArray(:,2);
uz      = controlArray(:,3);

[Xe,Ye,Ze] = sphere(25);

figure(4); clf; hold on; 
surf(Xe,Ye,Ze);  
plot3(x,y,z);
axis([-4 4 -4 4 -4 4]); axis equal;

scale = .2;
for i = 1:length(ux)
    uStartx(i) = x(i);
    uStarty(i) = y(i);
    uStartz(i) = z(i);
    uStopx(i) = uStartx(i) + scale*ux(i);
    uStopy(i) = uStarty(i) + scale*uy(i);
    uStopz(i) = uStartz(i) + scale*uz(i);
    plot3([uStartx(i);uStopx(i)],[uStarty(i);uStopy(i)],[uStartz(i);uStopz(i)],'r-','LineWidth',1.2);
end
%pause
drawnow;