function GroundTrack

%%  Define the initial state and some constants
% Prograde state
close all
TOF = 5.5;  %  Time of flight in days;
progradeLEO   = [7100 0 1300 0 7.35 1]';
retrogradeLEO = [2124 -4462 6651 -3.6 3.9 4]';
molniya       = [-52 -3079 -6150 10.043 -0.170 0]';
geo           = [-743.686572704364 39985.553832 0 -2.855551774845245 -0.053169 1.517940]';

bugState = [ -1122.002825819228 14375.83796022422 2240.587869013298  -2.462667103354754 3.061006787034337 4.917832567102448]

cartState = bugState;


%% Propagate the orbit for one day
odeOpt = odeset('AbsTol',1e-9,'RelTol',1e-9);
[t,xHist] = ode113(@OrbitDot,[0 TOF*86400],cartState,odeOpt);

%%  Plot the 3D orbit
figure(1)
[xSphere,ySphere,zSphere] = sphere(50);
plot3(xHist(:,1),xHist(:,2),xHist(:,3)); hold on;
surface(xSphere*6378,ySphere*6378,zSphere*6378);
axis equal

%%  Call the ground track plotter
PlotGroundTrack(t,xHist)

%% Plots the ground track
function PlotGroundTrack(t,xHist)

%%  Constants and initializations
refJulianDate  = 2451545;
d2r            = pi/180;
pi2            = 2*pi;
numEphemPoints = size(xHist,1);

% Setup ground track plot and earth map
figure(2); clf;  hold on;
hold on
load('topo.mat','topo','topomap1');
colormap(topomap1);
topoNew(:,1:180) = topo(:,181:360);
topoNew(:,181:360) = topo(:,1:180);
image([-180 180],[-90 90],topoNew,'CDataMapping', 'scaled');axis equal;
axis([-180 180 -90 90]); hold on; 

%%  Loop over epeheris and draw points
        
for ephIdx = 1:numEphemPoints
    
    % Compute longitude and latitude and determine direction of motion
    [posFixed,velFixed] = Inertial2Body(t(ephIdx),xHist(ephIdx,1:6)');
    long       = atan2(posFixed(2),posFixed(1));
    lat        = asin(posFixed(3)/sqrt(posFixed(1)^2+posFixed(2)^2 + posFixed(3)^2));
    dir        = sign(velFixed(2,1)*posFixed(1,1) - velFixed(1,1)*posFixed(2,1));
    figure(2);

    %  Draw new points depending upon special case
    if ephIdx > 2

        % Prograde case stepping off RHS of plot
        disp([long oldLong]*180/pi)
        mLong    = mod(long,2*pi);
        moldLong = mod(oldLong,2*pi);
        mLongM   = mod(long,-2*pi);;
        moldLongM   = mod(oldLong,-2*pi);
        if   moldLong < pi && mLong > pi &&  dir == 1 && lastDir == 1 
            m = (lat - oldLat)/ (mLong -moldLong);
            boundaryLatitude = m*(pi - mLong) + lat;
            plot([oldLong,pi]/d2r,[oldLat,boundaryLatitude]/d2r,'y-')
            plot([-pi,long]/d2r,[boundaryLatitude,lat]/d2r,'y-')   
        % Retrograde case stepping off LHS of plot    
        elseif dir == -1 && lastDir == -1 && moldLongM > -pi && mLongM  < -pi
            m = (lat - oldLat)/ (mLongM  - moldLongM);
            boundaryLatitude  = m*(-pi - mLongM) + lat;
            plot([oldLong,-pi]/d2r,[oldLat,boundaryLatitude]/d2r,'y-')
            plot([pi,long]/d2r,[boundaryLatitude,lat]/d2r,'y-')       
        % Regular case (not stepping off either side of plot)   
        else
            %  Just add the new point connecting with previous point
            plot([oldLong,long]/d2r,[oldLat,lat]/d2r,'y-')
        end
    end;
    
    %% Save data for next loop pass
    lastDir = dir;
    latVector(ephIdx,1) = lat;
    oldLong  = long;
    oldLat   = lat;
end

%%  Computes orbit acceleration
function Xdot = OrbitDot(t,X);

mu = 398600.4415;
rv = X(1:3);
Xdot(1:3,1) = X(4:6,1);
Xdot(4:6,1) = -mu/norm(rv)^3*rv;

%%  Converts state from earth inertial to earth fixed
function [posFixed,velFixed] = Inertial2Body(t,state);

refJulianDate  = 2451545;
d2r            = pi/180;
pi2            = 2*pi;
julianDate = refJulianDate + t/86400;
timeUT1    = (julianDate - 2451545)/36525;
GMT        = 67310.54841 +(876600*3600+8640184.812866)*timeUT1 ...
                 + 0.093104*timeUT1^2 - 6.2e-6*timeUT1^3;
GMT        = mod(GMT,86400)/240*d2r;
[R,Rdot]          = R3(GMT,0.000072921158553);
posFixed   = R*state(1:3,1);
velFixed   = R*state(4:6,1) + Rdot*state(1:3,1);

%%  Computes rotation matrix about z axis
function [R,Rdot] = R3(angle,angledot)
c = cos(angle); s = sin(angle);
R = [ c s 0; -s c 0; 0 0 1];

if nargin == 2
    Rdot = R3dot(angle,angledot);
end

%%  Computes rate of rotation matrix about z axis
function Rotmat = R3dot(angle,angledot)
c = cos(angle); s = sin(angle);
Rotmat = [ -s c 0; -c -s 0; 0 0 1]*angledot;