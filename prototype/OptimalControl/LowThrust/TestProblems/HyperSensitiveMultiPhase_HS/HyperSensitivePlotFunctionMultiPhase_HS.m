function HyperSensitivePlotFunctionMultiPhase(traj)

figure(8);clf;
for phaseIdx = 1:traj.numPhases
    stateArray = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    firstTime  = traj.phaseList{phaseIdx}.DecVector.GetFirstTime;
    lastTime   = traj.phaseList{phaseIdx}.DecVector.GetLastTime;
    time       = traj.phaseList{phaseIdx}.timeVector;
    if isa(stateArray,'gradient')
       stateArray = stateArray.x;
       firstTime = firstTime.x;
       lastTime = lastTime.x;
       time = time.x;
    end
    x    = stateArray(:,1);
    plot(time,x); hold on;
    plot(firstTime,x(1),'go','MarkerSize',9,'LineWidth',2)
    plot(lastTime,x(end),'rx','MarkerSize',9,'LineWidth',2)
end
grid on;
drawnow;
%keyboard
%pause
