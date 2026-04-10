function AutoNGCPlotFunction(traj)

% Define constants used in plotting
RE = 6378.1363;

% Extract state and control information from traj object
stateArray = traj.phaseList{1}.DecVector.GetStateArray();
controlArray = traj.phaseList{1}.DecVector.GetControlArray();

% Check if stateArray is of the class 'gradient'. If so then extract only
% state and control information not gradient values
if isa(stateArray,'gradient')
    stateArray = stateArray.x;
    controlArray = controlArray.x;
end

% Initialize and label configuration space plot
hold on
grid on;
axis equal
set(gcf,'color','w');
xlabel('\bf X-Axis [RE]', 'interpreter', 'latex');
ylabel('\bf Y-Axis [RE]', 'interpreter', 'latex');

% Redimensionalize and convert to Cartesian states
scaleUtil = traj.phaseList{1}.orbitScaleUtil;
cart = zeros(size(stateArray,1),6); % preallocate matrix for storing Cartesian states
for i = 1:size(stateArray,1)
   modEq = scaleUtil.UnsScaleModEqElements(stateArray(i,1:6)');
   cart(i,:) = Mee2Cart( modEq,scaleUtil.gravParam)';
%    plot3(cart(:,1)/RE,cart(:,2)/RE,cart(:,3)/RE,'k*');
end

% Plot states. Plot star at each state and connect with dotted line
plot3(cart(:,1)/RE,cart(:,2)/RE,cart(:,3)/RE,'k*:');

% Plot marker at origin
plot3(0, 0, 0,'ko','MarkerFaceColor', 'k');

% Draw plotted data on figure immediately
drawnow

% return


% r  = stateArray(:,1);
% theta  = stateArray(:,2);
% clf('reset')
% axis([-3 3 -3 3]);
% title('Optimal Spiral Transfer (Energy Change)')
% xlabel('X-axis, non-dimensional')
% ylabel('Y-axis, non-dimensional')
% grid on; hold on
% 
% 
% %  Plot orbit
% for i = 1:length(r)
%     x(i) = r(i)*cos(theta(i));
%     y(i) = r(i)*sin(theta(i));
% end
% 
% for i = 1:length(r)
%     xB(i) = cos(theta(i));
%     yB(i) = sin(theta(i));
% end
% 
% scale = 20;
% plot(x,y); hold on;
% plot(x,y,'g-')
% uR = 0.01;
% for i = 1:length(controlArray)-1
%     uStartx(i) = x(i);
%     uStarty(i) = y(i);
%     uStopx(i) = uStartx(i) + scale*uR*cos(theta(i)+ pi/2 - controlArray(i));
%     uStopy(i) = uStarty(i) + scale*uR*sin(theta(i)+ pi/2 - controlArray(i));
%     hold on;
%     plot([uStartx(i);uStopx(i)],[uStarty(i);uStopy(i)],'r-','LineWidth',1.2);
% end
% 
% cnt = 0;
% for ang = 0:.1:2*pi
%     cnt = cnt+1;
%     xBound(cnt) = 1.6*cos(ang);
%     yBound(cnt) = 1.6*sin(ang);
% end
% hold on; plot(xBound,yBound,'k-')
% cnt = 0;
% for ang = 0:.1:2*pi
%     cnt = cnt+1;
%     xBound(cnt) = 1.0*cos(ang);
%     yBound(cnt) = 1.0*sin(ang);
% end
% 
% 
% hold on; plot(xBound,yBound,'k-')
% title('Optimal Spiral Transfer (Energy Change)')
% xlabel('X-axis, non-dimensional')
% ylabel('Y-axis, non-dimensional')
% grid on;
% axis equal
% axis([-3 3 -3 3]);
% drawnow;