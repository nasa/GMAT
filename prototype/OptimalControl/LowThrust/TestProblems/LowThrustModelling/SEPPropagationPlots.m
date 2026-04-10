[m,n] = size(stateHist);
stateHist(m,1:6)'
if Force.UseFiniteBurn
    stateHist(m,7)
end
if Force.STM
    disp(reshape(stateHist(m,7:42),[6 6]))
end

figure(2)
subplot(1,2,1)
plot3(stateHist(:,1),stateHist(:,2),stateHist(:,3))
hold on
[x,y,z] = sphere(25);
surf(x*6378,y*6378,z*6378)
axis equal
subplot(1,2,2)
% plot(timeHist,stateHist(:,7),'.')
% xlabel('Elapased Time, secs')
% ylabel('Total Spacecraft Mass, kg')
% hold on
if Force.UseFiniteBurn
    plot(timeHist,stateHist(:,7),'-')
    xlabel('Elapased Time, secs')
    ylabel('Total Spacecraft Mass, kg')
    
    
    
    figure(1)
    subplot(3,2,1)
    plot( data.elapsedTime ,data.percentSun)
    grid on
    title('Percent Sun vs. Time')
    xlabel('Elapsed Time, days');
    ylabel('Percent Sun');
    axis([min(data.elapsedTime) max(data.elapsedTime) 0 1.1])
    subplot(3,2,2)
    plot(data.elapsedTime,data.totalPower)
    title('Total Power vs. Time')
    ylabel('Total Power, kW');
    xlabel('Elapsed Time, days');
    grid on
    subplot(3,2,3)
    plot(data.elapsedTime,data.busPower)
    title('Bus Power vs. Time')
    ylabel('Spacecraft Bus Power, kw');
    xlabel('Elapsed Time, days');
    grid on
    subplot(3,2,4)
    plot(data.elapsedTime,data.thrustPower)
    title('Available Thrust Power vs. Time')
    xlabel('Elapsed Time, days');
    ylabel('Available Power, kw');
    grid on
    subplot(3,2,5)
    plot(data.elapsedTime,data.thrustMag)
    title('Thrust Mag vs. Time')
    ylabel('Thrust Magnitude, milli-Newtons');
    xlabel('Elapsed Time, days');
    grid on
    subplot(3,2,6)
    plot(data.elapsedTime,data.massFlowRate)
    title('Mass Flow Rate vs. Time')
    xlabel('Elapsed Time, days');
    ylabel('Mass Flow Rate, kg/s');
    grid on
end
