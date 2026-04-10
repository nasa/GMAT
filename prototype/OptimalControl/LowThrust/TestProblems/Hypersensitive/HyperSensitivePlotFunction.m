function HyperSensitivePlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();

time =  traj.phaseList{1}.timeVector;
if isa(time,'gradient')
    time = time.x;
    stateArray = stateArray.x;
end
x  = stateArray(:,1);
figure(1)
plot(time,x); grid on;
xlabel('Time'); ylabel('X')
drawnow;
title('Radau')
%pause
return

