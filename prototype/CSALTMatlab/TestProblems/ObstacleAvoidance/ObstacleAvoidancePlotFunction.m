function ObstacleAvoidancePlotFunction(traj)

figure(1); clf
for phaseIdx = 1:traj.numPhases
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    figure(1);
    hold on; grid on;
    plot(stateSol(:,1),stateSol(:,2))
    title('state history')
    figure(2)
    hold on; grid on;
    %if isa(traj.phaseList{phaseIdx},'RadauPhase')
        plot(times(1:end-1),controlSol(:,1))
    %else
    %    plot(times,controlSol(:,1))
    %end
    title('control history')
end

%  Plot the exclusion regions
figure(1)
cnt = 0;
radius = pi/10;
xoffset = 0.4;
yoffset = 0.5;
for theta = 0:0.05:2*pi+.2
    cnt = cnt + 1;
    angle(cnt) = theta;
    x(cnt) = radius*cos(theta) + xoffset;
    y(cnt) = radius*sin(theta) + yoffset;
end
plot(x,y,'r-')

cnt = 0;
radius = pi/10;
xoffset = 0.8;
yoffset = 1.5;
for theta = 0:0.05:2*pi+.2
    cnt = cnt + 1;
    angle(cnt) = theta;
    x(cnt) = radius*cos(theta) + xoffset;
    y(cnt) = radius*sin(theta) + yoffset;
end
plot(x,y,'c-')
axis equal
axis([0 1.2 0 2.0])


end