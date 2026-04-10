
%% =====  Make figures
global constants
MU = constants.MU;
TU = constants.TU;
DU = constants.DU;
Isp = constants.Isp;
g0 = constants.g0;
mscale = constants.mscale;
PERIOD_DRO = constants.PERIOD_DRO;
PERIOD_L2Halo = constants.PERIOD_L2Halo;
day = constants.day;
m0 = constants.m0;
thrust_limit = constants.thrust_limit;
min_dist_moon = constants.min_dist_moon;
min_dist_earth = constants.min_dist_earth;
mustar = constants.mustar;

%Pull out solution:
x = traj.phaseList{1}.DecVector.GetStateArray();
u = traj.phaseList{1}.DecVector.GetControlArray();
u = [u; u(end,:)]; %add another row to 'u' so it's the same length as 'x' and 't'
t = traj.phaseList{1}.TransUtil.GetTimeVector();

%% =====  Analyze thrust profile
T_mag = ( u(:,1).^2 + u(:,2).^2 + u(:,3).^2 ).^(1/2); %normalized magnitude
t_days = t * TU / day;

figure(1)
plot(t_days, T_mag, 'r.-','markersize',15,'linewidth',2)
xlabel('Time (days)')
ylabel('Thrust (N)')
title('Thrust profile, DRO to L2 halo')
grid on
ylim([0, max(T_mag)+0.05])

%% =====  Mass

mass = x(:,7) .* mscale;
figure(2)
plot(t_days, mass,'k*-')
xlabel('Time (days)')
ylabel('Mass (kg)')
title('Mass')
grid on

fprintf('Mass:\n')
fprintf('\tInitial         = %.3f kg\n', mass(1))
fprintf('\tFinal           = %.3f kg\n', mass(end))
fprintf('\tPropellant used = %.3f kg\n', mass(1)-mass(end))

%% =====  Interpolated reconstruction
%Load DRO and L2 orbits:
DRO = load('DRO_points.mat');
L2 = load('L2_points.mat');
n_many = 1e3; %number of interpolation points

t_many = linspace(0,t(end),n_many);
x_many = interp1(t,x,t_many,'spline');
u_many = interp1(t,u,t_many,'spline');

t_ticks = 0:(day/TU):t(end); %1-day spacing
x_ticks = interp1(t,x,t_ticks,'spline');
u_ticks = interp1(t,u,t_ticks,'spline');

t_arrows = 0:(0.5 * day/TU):t(end); %0.5-day spacing
x_arrows = interp1(t,x,t_arrows,'spline');
u_arrows = interp1(t,u,t_arrows,'spline');

scrsz = get(groot,'ScreenSize');
figure('Position',[50 scrsz(4)/4 scrsz(3)*.35 scrsz(4)/2])
hold all
plot3(DRO.state_rot(:,1),DRO.state_rot(:,2),DRO.state_rot(:,3),'m--','linewidth',2)
plot3(L2.state_rot(:,1),L2.state_rot(:,2),L2.state_rot(:,3),'b-.','linewidth',2)

plot3(x_many(:,1),x_many(:,2),x_many(:,3),'k-','linewidth',2)

%plot thrust vectors:
quiver3(x_arrows(:,1),x_arrows(:,2),x_arrows(:,3), u_arrows(:,1),u_arrows(:,2),u_arrows(:,3),1,'r', 'linewidth',1)

%plot tick marks at 1-day intervals
plot3(x_ticks(:,1),x_ticks(:,2),x_ticks(:,3),'k+','markersize',8)

%point to Earth, label Earth & Moon
yL = ylim;
annotation('textarrow',[.3, .2],[1, 1]*abs(yL(1)) / (diff(yL)),'String','to Earth','fontsize',12)
% annotation('textbox',[0.3 0.3 0.1 0.1],'String','Moon','linestyle','none')

plot3(x(1,1),x(1,2),x(1,3),'m.','markersize',15)
plot3(x(end,1),x(end,2),x(end,3),'b.','markersize',15)
axis equal
grid on
axis tight
xL = xlim;
% xlim([0.2, xL(2)])
xlim([0.6, xL(2)])

%moon
[x2,y2,z2] = sphere(30);
moon_scale = 1738/DU;
x2 = moon_scale*x2 + (1- mustar); y2 = moon_scale*y2; z2 = moon_scale*z2;
moon = surf(x2,y2,z2);    %plot moon
set(moon,'FaceColor',[.5,.5,0.5],'FaceAlpha',0.7,'EdgeAlpha',0.2)


xlabel('X (DU)')
ylabel('Y (DU)')
zlabel('Z (DU)')
title(['Rotating frame, Time = ',num2str(t(end)*TU/day,3), ' days, Thrust = ', num2str(max(T_mag),3), ' N.'])

legend('DRO (start)','L2 halo (finish)','Transfer','Thrust vectors',...
    'Location','northwest')

%% plot 3D -- showing nodes clearly
scrsz = get(groot,'ScreenSize');
figure('Position',[50 scrsz(4)/4 scrsz(3)*.35 scrsz(4)/2])
hold all
% plot3(DRO.state_rot(:,1),DRO.state_rot(:,2),DRO.state_rot(:,3),'m--','linewidth',2)
% plot3(L2.state_rot(:,1),L2.state_rot(:,2),L2.state_rot(:,3),'b-.','linewidth',2)

L = length(t); %number of nodes to show
t_many = linspace(0,t(L),n_many);
x_many = interp1(t,x,t_many,'spline');
u_many = interp1(t,u,t_many,'spline');

plot3(x(1:L,1),x(1:L,2),x(1:L,3),'r.','markersize',15)
plot3(x_many(:,1),x_many(:,2),x_many(:,3),'k-','linewidth',1)

xlabel('X (DU)')
ylabel('Y (DU)')
zlabel('Z (DU)')
% title(['Rotating frame, Time = ',num2str(t(end)*TU/day,3), ' days, Thrust = ', num2str(T_lim,3), ' N.'])
title('Nodes highlighted')
%point to Earth
yL = ylim;
% annotation('textarrow',[.3, .2],[1, 1]*abs(yL(1)) / (diff(yL)),'String','to Earth','fontsize',12)

legend('Nodes', 'Interpolated trajectory',...
    'Location','Northwest')

%plot thrust vectors:
quiver3(x(:,1),x(:,2),x(:,3), u(:,1),u(:,2),u(:,3),1,'r')

%moon
[x2,y2,z2] = sphere(30);
moon_scale = 1738/DU;
x2 = moon_scale*x2 + (1- mustar); y2 = moon_scale*y2; z2 = moon_scale*z2;
moon = surf(x2,y2,z2);    %plot moon
set(moon,'FaceColor',[.5,.5,0.5],'FaceAlpha',0.7,'EdgeAlpha',0.2)

axis equal
grid on
axis tight
xL = xlim;

F = traj.phaseList{1}.Config.GetMeshIntervalFractions;
t_node_ends = (F+1)/2 * (t(end)-t(1));
x_node_ends = interp1(t,x,t_node_ends,'spline');
plot3(x_node_ends(:,1),x_node_ends(:,2),x_node_ends(:,3),'mo','markersize',20)

