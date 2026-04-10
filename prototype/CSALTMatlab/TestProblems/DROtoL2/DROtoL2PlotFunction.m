function DROtoL2PlotFunction(traj)

persistent plotData

if isempty(plotData)
    plotData = 1;
end

if plotData
   PlotDROtoL2Solution
   plotData = 0;
end

global Nphase phase_type

% Loop over the phases and get state and control data
stateArray = [];
controlArray = [];
for ind = 1:Nphase
    % Extract the state data
    stateArray_temp = traj.GetStateArray(ind);
    
    % Extract the control data
    if phase_type(ind) == 1 %thrust not allowed (coasting)
        controlArray_temp = zeros(length(stateArray_temp)-1, 3);
    else
        controlArray_temp = traj.GetControlArray(ind);
    end
    
    if ind == Nphase %keep the extra point at the end
        stateArray = [stateArray; stateArray_temp];
        controlArray = [controlArray; controlArray_temp];
    else %discard extra point at the end (same as start for next phase)
        stateArray = [stateArray; stateArray_temp(1:end-1,:)];
        controlArray = [controlArray; controlArray_temp];
    end
end

% Extract the times
time = traj.GetTimeArray(1);
clf; hold all;

% Plot the data
plot3(stateArray(:,1),stateArray(:,2),stateArray(:,3),'k','linewidth',1);
quiver3(stateArray(1:end-1,1),stateArray(1:end-1,2),stateArray(1:end-1,3), ...
    controlArray(:,1),controlArray(:,2),controlArray(:,3),'r')

% Set axes, labels, etc.
grid on;
xlabel('X (DU)');ylabel('Y (DU)');zlabel('Z (DU)')
axis equal
xlim([0.8 1.3])
ylim([-0.25 0.25])
zlim([-0.25 0.25])
drawnow;


