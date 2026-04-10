function OrbitRaisingPlotFunction(traj)

%  Plot Earth
[Xe,Ye,Ze] = sphere(25);
figure(4); clf; hold on;
surf(Xe,Ye,Ze);
axis([-4 4 -4 4 -4 4]); axis equal;

%  Loop over the phases and  plot them
for phaseIdx = 1:traj.numPhases
    if traj.phaseList{phaseIdx}.phaseNum == 2
        lineWidth = 1;
    else
        lineWidth = 3;
    end
    stateArray = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlArray = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    if isa(stateArray,'gradient')
        stateArray = stateArray.x;
        controlArray = controlArray.x;
    end
    
    
    x      = stateArray(:,1);
    y      = stateArray(:,2);
    z      = stateArray(:,3);
    if phaseIdx ~= 2
        ux      = controlArray(:,1);
        uy      = controlArray(:,2);
        uz      = controlArray(:,3);
    else
        ux      = 0;
        uy      = 0;
        uz      = 0;
    end
    plot3(x,y,z,'LineWidth',lineWidth);
    
    if phaseIdx ~= 2
        scale = .2;
    else
        scale = .2/1000;
    end
    for i = 1:length(ux)
        uStartx(i) = x(i);
        uStarty(i) = y(i);
        uStartz(i) = z(i);
        uStopx(i) = uStartx(i) + scale*ux(i);
        uStopy(i) = uStarty(i) + scale*uy(i);
        uStopz(i) = uStartz(i) + scale*uz(i);
        plot3([uStartx(i);uStopx(i)],[uStarty(i);uStopy(i)],...
            [uStartz(i);uStopz(i)],'r-','LineWidth',1.2);
    end
end
xlabel('X Coordinate');ylabel('Y Coordinate'); grid on;
axis([ -2 2 -2 2 -2 2])
drawnow;
