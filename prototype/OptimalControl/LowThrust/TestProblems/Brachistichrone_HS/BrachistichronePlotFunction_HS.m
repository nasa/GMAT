function BrachistichronePlotFunction(traj)

stateArray = traj.phaseList{1}.DecVector.GetStateArray();
controlArray = traj.phaseList{1}.DecVector.GetControlArray(); 

if isa(stateArray,'gradient')
    stateArray = stateArray.x;
    controlArray = controlArray.x;
end

x  = stateArray(:,1);
y  = stateArray(:,2);
clf;
plot(x,y); 
axis([0 2 -2 0]); grid on;
xlabel('X Coordinate');ylabel('Y Coordinate')
drawnow;
%pause

return

