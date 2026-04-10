function PlotSolution(traj)

numPhases = traj.numPhases;
figure(101)

for phaseIdx = 1:numPhases
    
    % Extract the solution
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    timeVector = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    
    %if is(traj.phaseList{phaseIdx},'RadauPhase')
    if length(timeVector) > size(controlSol,1)
        controlTimeVector = timeVector(1:end-1);
    else
        controlTimeVector = timeVector;
    end
    
    % Interpolate to points
    numTimePoints = length(timeVector)*2;
    dt = (timeVector(end) - timeVector(1))/numTimePoints;
    newTimeVec = timeVector(1):dt:timeVector(end);
    controlInterpPoints = interp1(controlTimeVector,controlSol,newTimeVec);
    stateInterpPoints = interp1(timeVector,stateSol,newTimeVec);
    
    % Draw the points
    subplot(2,1,1)
    hold on
    plot(timeVector,stateSol,'b-')
    plot(newTimeVec,stateInterpPoints, 'b*')
    subplot(2,1,2)
    hold on
    plot(controlTimeVector,controlSol,'b-')
    plot(newTimeVec,controlInterpPoints, 'b*')
    
end

subplot(2,1,1)
xlabel('State Time History')
legend('State Collocation History', 'State Interpolation')
grid on

subplot(2,1,2)
xlabel('Control Time History')
legend('Control Collocation History', 'Control Interpolation')
grid on







