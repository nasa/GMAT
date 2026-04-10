function HyperSensitivePlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();

time =  traj.phaseList{1}.TransUtil.timeVector;
controlArray = traj.phaseList{1}.DecVector.GetControlArray();
if isa(time,'gradient')
    time = time.x;
    stateArray = stateArray.x;
end
x  = stateArray(:,1);
figure(1)
subplot(2,1,1)
plot(time,x); grid on;
xlabel('Time'); ylabel('X')
drawnow;
numControls = length(controlArray);
subplot(2,1,2)
plot(time(1:numControls),controlArray(1:numControls)); grid on;
xlabel('Time'); ylabel('X')
title('Radau')
%pause
return

