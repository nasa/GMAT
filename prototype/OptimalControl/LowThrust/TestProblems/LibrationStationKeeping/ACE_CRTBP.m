%% Initializations and configuration
close all; clear all

%  Constants
%clear all; close all;
clc
baseFreq = 2*pi/(365.24*86400);

%  Givens for ACE
om_xy = 2.057*baseFreq*1.011;
om_z  = 1.9851*baseFreq*1.015;
k     = -0.3138;
A_xy  =  260285.777055646;
A_z   =  163495.200129871;
dV_z  = -0.00012*cos(20*pi/180);

%  Free parameters for ACE
phi_xy = -1.95;
phi_z  = -1.21;

%% Compute the trajectory
t0 = 0;
stepSize = .1;
count = 0;
for t = 1:5:365*5.7
    count = count + 1;
    time = t*86400;
    timeVec(count) = t;
    X(count)  =  A_xy*k*cos(om_xy*time+phi_xy);
    Y(count)  =  A_xy*sin(om_xy*time+phi_xy);
    Z(count)  =  A_z*cos(om_z*time + phi_z);
    VX(count) = -A_xy*k*om_xy*sin(om_xy*time+phi_xy);
    VY(count) =  A_xy*om_xy*cos(om_xy*time+phi_xy);
    VZ(count) = -A_z*om_z*sin(om_z*time + phi_z);
    dAdZ(count) = VZ(count)/A_z/om_z^2;
    deltaA_z(count) = VZ(count)/A_z/om_z^2*dV_z;
end

NonlinearRLPPlots

%%  Plot the RLP coordinate evolution
figure(10)
time = RLPCoordinates(:,1) - RLPCoordinates(1,1);
plot(time,RLPCoordinates(:,4),timeVec,Z,'r+');%,time,RLPCoordinates(:,3),time,RLPCoordinates(:,4))
xlabel('time, days')
legend('z RLP Nonlinear Propagation' ,'z RLP Linear Propagation')
ylabel('ACE Spacecraft z RLP Coordinates, km')
title('Comparison of Nonlinear and Linear Propagation for ACE Orbit')
axis([0 2250 -2.1e5 2.1e5])
grid on

figure(11)
time = RLPCoordinates(:,1) - RLPCoordinates(1,1);
plot(time,RLPCoordinates(:,2),time,RLPCoordinates(:,3),timeVec,X,'g+',timeVec,Y,'b+');
hold on%,time,RLPCoordinates(:,3),time,RLPCoordinates(:,4))
xlabel('time, days')
ylabel('ACE Spacecraft RLP Coordinates, km')
title('Comparison of Nonlinear and Linear Propagation for ACE Orbit')
axis([0 2250 -3.1e5 3.1e5])
hold on
grid on
legend('x RLP Nonlinear Propagation', 'y RLP Nonlinear Propagation' ,'x RLP Linear Propagation','y RLP Linear Propagation')


%% Plot the results
figure(1)
plot3(X,Y,Z)
minx = min(X); maxx = max(X);
miny = min(Y); maxy = max(Y);
minz = min(Z); maxz = max(Z);
hold on
fac = 1.1;
plot3([0 maxx*fac],[0 0], [0 0], 'r-','LineWidth',4)
plot3([0 0],[0 maxy*fac], [0 0], 'g-','LineWidth',4)
plot3([0 0],[0 0], [0 maxz*fac], 'b-','LineWidth',4)
axis equal

figure(2)
subplot(2,1,1)
plot(timeVec,X,timeVec,Y,timeVec,Z)
grid on
subplot(2,1,2)
plot(timeVec,VX,timeVec,VY,timeVec,VZ)
grid on

figure(3)
subplot(3,1,1)
plot(timeVec,Z)
grid on
xlabel('time, days')
ylabel('z RLP coordinate, km')
subplot(3,1,2)
plot(timeVec,VZ)
xlabel('time, days')
ylabel('Vz RLP coordinate, km')
grid on
subplot(3,1,3)
plot(timeVec,deltaA_z)
ylabel('delta Az, km')
grid on