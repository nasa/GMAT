
close all
clc

to = 0;
tf = 360;
d2r = pi/180;
timeStep = 1;
timeVec = to:timeStep:tf;

Omega_IB_B = zeros(3,length(timeVec));
Omega_IB_I = zeros(3,length(timeVec));
Q = zeros(4,length(timeVec));

%% Input by user
BodySpinAxis             = [-1 5 1]';
NutationReferenceVector = [1 0 0]';
InitialPrecessionAngle = 0;         % deg.
PrecessionRate   = 5;                % deg./sec.
SpinRate         = 10;                      % deg./sec.
InitialSpinAngle = 0;               % deg.
NutationAngle    = 15;                 % deg.
ViewAxis = [1 1 1];

%% Main test function
% Define Inertial frame axes
origin = [0 0 0];
InertialX = [1 0 0];
InertialY = [0 1 0];
InertialZ = [0 0 1];

R_next = eye(3);
R_save = eye(3);
fid = fopen('AttitudeHistory.txt','wt');

%  Covert to radians
PrecessionRate = PrecessionRate*d2r;                % deg./sec.
InitialPrecessionAngle = InitialPrecessionAngle*d2r;         % deg.
SpinRate = SpinRate*d2r;                      % deg./sec.
InitialSpinAngle = InitialSpinAngle*d2r;               % deg.
NutationAngle = NutationAngle*d2r;                 % deg.

for i=1:length(timeVec)
    time = timeVec(i);
    
   % [ R, Rdot, Omega_IB_B(:,i) ] = PrecessingSpinPrototype(time, BodySpinAxis, ...
   %     NutationReferenceVector, PrecessionRate, InitialPrecessionAngle, SpinRate, InitialSpinAngle, NutationAngle);
     
    
    [R, Rdot, Omega_IB_B(:,i)] = PrecessingSpinner(NutationReferenceVector,BodySpinAxis,InitialSpinAngle,SpinRate,...
                                          InitialPrecessionAngle,PrecessionRate,NutationAngle, time);
    Q(:,i) = dcm2quat(R');
    Omega_IB_I(:,i) = R*Omega_IB_B(:,i);
    
    Omega_IB_I_Now =  Omega_IB_I(:,i);
    acos(Omega_IB_I_Now'*NutationReferenceVector/norm(NutationReferenceVector)/norm(Omega_IB_I_Now))*180/pi 
    
    %fprintf(fid,'%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n',Q(2,i),Q(3,i),Q(4,i),Q(1,i), ...
    %    Omega_IB_B(1,i),Omega_IB_B(2,i),Omega_IB_B(3,i));
    
    % To check relationship between R and Rdot
    display(norm(R_next - R));
    R_next = R + Rdot*timeStep;
    
    figure(1)
    scale = 3;
    % Draw Inertial x axis
    xAxisArray = [origin; (origin + scale*InertialX)];
    plot3(xAxisArray(:,1),xAxisArray(:,2),xAxisArray(:,3),'r--','LineWidth',2); hold on
    % Draw Inertial y axis
    yAxisArray = [origin; (origin + scale*InertialY)];
    plot3(yAxisArray(:,1),yAxisArray(:,2),yAxisArray(:,3),'g--','LineWidth',2);
    % Draw Inertial z axis
    zAxisArray = [origin; (origin + scale*InertialZ)];
    plot3(zAxisArray(:,1),zAxisArray(:,2),zAxisArray(:,3),'b--','LineWidth',2);
    % Draw Inertial precession axis
    NutationReferenceVector = NutationReferenceVector/norm(NutationReferenceVector);
    zAxisArray = [origin; (origin + scale*0.75*NutationReferenceVector')];
    plot3(zAxisArray(:,1),zAxisArray(:,2),zAxisArray(:,3),'c--','LineWidth',2);
    xlim([-2 3]); ylim([-2 3]); zlim([-2 3]);
    
    % Draw the attitude
    scale = 2;
    % Draw body x axis
    xAxisArray = [origin; (origin + scale*R(:,1)')];
    plot3(xAxisArray(:,1),xAxisArray(:,2),xAxisArray(:,3),'r-','LineWidth',4);
    % Draw body y axis
    yAxisArray = [origin; (origin + scale*R(:,2)')];
    plot3(yAxisArray(:,1),yAxisArray(:,2),yAxisArray(:,3),'g-','LineWidth',4);
    % Draw body z axis
    zAxisArray = [origin; (origin + scale*R(:,3)')];
    plot3(zAxisArray(:,1),zAxisArray(:,2),zAxisArray(:,3),'b-','LineWidth',4);
    % Draw body spin axis
    BodySpinAxis = BodySpinAxis/norm(BodySpinAxis);
    SpinAxisArray = [origin; origin + (scale*R*BodySpinAxis)'];
    plot3(SpinAxisArray(:,1),SpinAxisArray(:,2),SpinAxisArray(:,3),'m-','LineWidth',4);
    legend('Inertial_X', 'Inertial_Y', 'Inertial_Z','Inertial_{Reference}',...
        'Body_x', 'Body_y', 'Body_z', 'Body_{spin}');
    hold off
    view(ViewAxis)
    drawnow
    % pause(0.05)
    
end
fclose(fid);
return
% figure()
% plot(timeVec, Q(1,:),'r','LineWidth',2); hold on
% plot(timeVec, Q(2,:),'g','LineWidth',2)
% plot(timeVec, Q(3,:),'b','LineWidth',2)
% plot(timeVec, Q(4,:),'k','LineWidth',2)
% legend('Q_1','Q_2','Q_3','Q_4')
% hold off;
%
figure()
plot(timeVec, Omega_IB_B(1,:),'r','LineWidth',2); hold on
plot(timeVec, Omega_IB_B(2,:),'g','LineWidth',2)
plot(timeVec, Omega_IB_B(3,:),'b','LineWidth',2)
legend('\omega_x','\omega_y','\omega_z')
hold off;