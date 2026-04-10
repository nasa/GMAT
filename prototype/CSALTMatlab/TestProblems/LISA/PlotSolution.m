function PlotSolution(traj,satName)

global configEarth configSun initialState propDuration phaseList
global scaleUtil

if strcmp(satName, 'LISA1')
    trajColor = 'b-';
elseif strcmp(satName,'LISA2');
    trajColor = 'g-';
else strcmp(satName,'LISA3');
    trajColor = 'm-';
end
    

times = traj.phaseList{1}.TransUtil.GetTimeVector()*configSun.TU/86000;
stateVec = traj.phaseList{1}.DecVector.GetStateArray();
controlVec = traj.phaseList{1}.DecVector.GetControlArray();
numRevs = stateVec(end,1)/2/pi;

h = figure(2);
set(h,'color','w');
subplot(2,1,1); hold on;
plot(times(1:end-1),controlVec(:,1));
grid on
modControlDir = zeros(size(controlVec(:,1),1),3);
for pointIdx = 1:size(controlVec(:,1),1)
    if controlVec(pointIdx,1) < sqrt(eps)
        fac = 0;
    else
        fac = 1;
    end
    modControlDir(pointIdx,:) = controlVec(pointIdx,2:4)*fac;
    controlMag = norm(modControlDir(pointIdx,:));
    if controlMag <= sqrt(eps)
        controlAngle(pointIdx) = 0;
    else
        controlAngle(pointIdx) = acos(modControlDir(pointIdx,1)/controlMag);
    end
end
ylabel('\bf Control Magnitude', 'interpreter', 'latex');
axis([min(times) max(times) 0 1]);
subplot(2,1,2); hold on;
plot(times(1:end-1),modControlDir(:,1),times(1:end-1),modControlDir(:,2),times(1:end-1),modControlDir(:,3))
title('\bf Control Unit Direction vs Time (Direction is zero when Thrust is Off)', 'interpreter', 'latex')
ylabel('\bf Control Unit Direction ', 'interpreter', 'latex');
grid on
l = legend('Radial', 'In-Track', 'Cross-Track', 'location', 'southeast');
set(l, 'interpreter', 'latex');
axis tight;
xlabel('\bf Time [days]', 'interpreter', 'latex');

disp('Last Time In Days')
disp(times(end)*configSun.TU/86400)
disp('Final MLONG, Rad')
disp(stateVec(end,6))

R_Eq_Ec = [1.0 0.0 0.0;...
0.0 0.91748206 -0.397777156;...
0.0 0.39777716 0.9174820621];
BigR = [R_Eq_Ec' zeros(3,3); zeros(3,3) R_Eq_Ec'];
for iterIdx = 1:size(stateVec,1)
    dimMEE = configSun.StateUnScaleMat*stateVec(iterIdx,:)';
    dimMEEDegrees = dimMEE;
    dimMEEDegrees(6) = mod(dimMEEDegrees(6)*180/pi,360);
    meeState(iterIdx,:) = dimMEEDegrees;
    initState = Mee2Cart(dimMEE(1:6),configSun.gravParam);
    cartState(iterIdx,:) = initState;
    kepState(iterIdx,:) = cart2oe(BigR*initState,configSun.gravParam);
    if kepState(4) > pi
        kepState(4) = kepState(4) - 2*pi;
    end
    if kepState(5) > pi
        kepState(5) = kepState(4) - 2*pi;
    end
end


%=============  Plot the control angle
figure(6)
plot(times(1:end-1),controlAngle*180/pi,trajColor,'LineWidth',1.0);
hold on
title('\bf SAA Angle vs. Time', 'interpreter', 'latex')
ylabel('\bf SAA Angle [deg]', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
grid on
plot([times(1) times(end-1)],[120 120],'r-')
plot([times(1) times(end-1)],[60 60],'r-')
axisFactor = 1.05;
h = figure(3);
set(h,'color','w');
%h.PaperPosition = [2 4 4.5 8];
subplot(3,2,1); hold on;
plot(times,kepState(:,1)/configSun.DU,trajColor,'LineWidth',1.0);
grid on
title('\bf SMA vs. Time', 'interpreter', 'latex')
ylabel('\bf SMA [AU]', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
axis([0 500 0.9 1.3])
subplot(3,2,2); hold on;
plot(times,kepState(:,2),trajColor,'LineWidth',1.0);
grid on
title('\bf Eccentricity vs. Time', 'interpreter', 'latex')
ylabel('\bf Ecc, dimless', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
axis([0 500 min(kepState(:,2))*axisFactor max(kepState(:,2))*axisFactor])
subplot(3,2,3); hold on;
plot(times,kepState(:,3)*180/pi,trajColor,'LineWidth',1.0);
grid on
title('\bf Inclination vs. Time', 'interpreter', 'latex')
ylabel('\bf Inclination [deg]', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
axis([0 500 0 max(kepState(:,3))*180/pi*axisFactor*1.00000001])
subplot(3,2,4); hold on;

for i = 1:size(kepState(:,4))
    aop(i,1) = kepState(i,4);
    %raanDiff = raan(i,1) - meanRAAN;
    if aop(i,1)  > pi
        aop(i,1) = aop(i) - 2*pi;
    elseif aop(i,1) < -pi
        aop(i,1) = aop(i) + 2*pi;
    end
end
plot(times,aop*180/pi,trajColor,'LineWidth',1.0);
grid on
title('\bf AOP vs. Time', 'interpreter', 'latex')
ylabel('\bf AOP [deg]', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
axis([0 500 -180 180])

subplot(3,2,5); hold on;
meanRAAN = mean(kepState(:,5));
for i = 1:size(kepState(:,5))
    raan(i,1) = kepState(i,5);
    %raanDiff = raan(i,1) - meanRAAN;
    if raan(i,1)  > pi
        raan(i,1) = raan(i) - 2*pi;
    elseif raan(i,1) < -pi
        raan(i,1) = raan(i) + 2*pi;
    end
end
plot(times,raan*180/pi,trajColor,'LineWidth',1.0);
grid on
title('\bf RAAN vs. Time', 'interpreter', 'latex')
ylabel('\bf RAAN [deg]', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
axis([0 500 min(raan*180/pi)*axisFactor max(raan*180/pi)*axisFactor])
subplot(3,2,6); hold on;
masses = stateVec(:,7)*configSun.MU;
plot(times, masses,trajColor,'LineWidth',1.0);
axis([0 500 1700 1900])
title('\bf Mass vs. Time', 'interpreter', 'latex')
ylabel('\bf Mass [kg]', 'interpreter', 'latex')
xlabel('\bf Time [days]', 'interpreter', 'latex')
grid on

