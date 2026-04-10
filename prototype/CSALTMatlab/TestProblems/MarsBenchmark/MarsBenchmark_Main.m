ClearAll
global igrid iGfun jGvar traj SEPForces config LV callCount
import MarsBenchmarkPathObject
import MarsBenchmarkPointObject
import LaunchVehicle
import exec.Trajectory
import exec.RadauPhase
import exec.ImplicitRungeKuttaPhase

callCount = 0;

init_eph('DE405.dat');

time2mjd = 2430000.5;

config.gravParam = 132712440017.99;
config.DU  = 149597871;
config.TU  = sqrt(config.DU^3/config.gravParam);
config.MU  = 1000;
config.VU = config.DU/config.TU;
config.TimeOffSet = 29580;
config.StateScaleMat = diag([ones(3,1)/config.DU;  ones(3,1)/config.VU; 1/config.MU]);
config.StateAccScaleMat = diag([
    ones(3,1)*(config.TU/config.DU);
    ones(3,1)*(config.TU^2/config.DU);
    (config.TU/config.MU)]);
config.StateUnScaleMat = inv(config.StateScaleMat);
config.StateAccUnScaleMat = inv(config.StateAccScaleMat);

config.maxFlightTime = 546 * 86400 / config.TU;
config.launchWindow = 365 * 86400 / config.TU;

SevenAU = 7.0*config.DU;

LV = LaunchVehicle();
LV.SetLaunchVehicleType(8)

% phaseTimes = [0 60 226 984 1149 2139 2636]*86400;
phaseTimes = [100 400 546]*86400;

%==========================================================================
%% =====  Initializations
%==========================================================================
MyPathConfig
MarsBenchmarkDynamicsConfig
pathObj = MarsBenchmarkPathObject();
pathObj.SEPForces = SEPForces;
pathObj.config = config;

% kernalPath = 'C:/Users/sphughe1/Documents/Repos/BitBucket/CollUtil/SPKFiles/';
% libPath = 'C:/Users/sphughe1/Documents/Repos/Misc/mice/miceWin64';
% repoPath = 'C:/Users/sphughe1/Documents/Repos/BitBucket/CollUtil/LowThrust/lowthrust';
folderLocation = '/TestProblems/DARE_MultiPhase/';

kernalPath = '/Users/jknittel/Universe/';
libPath = '/Users/jknittel/Utilities/mice';
repoPath = '/Users/jknittel/Collocation/LowThrust/lowthrust';

addpath(genpath(libPath))
cspice_furnsh( [kernalPath 'naif0011.tls'])
cspice_furnsh( [kernalPath 'PlanetsLight.bsp'])
cspice_furnsh( [kernalPath 'de421.bsp'])


%==========================================================================
%% =====  Define Properties for the Trajectory
%==========================================================================

%  Create trajectory and configure user function names
traj                      = Trajectory();
traj.pathFunctionObject   = pathObj;
traj.pointFunctionObject  = MarsBenchmarkPointObject();
traj.guessFunctionName    = 'MarsBenchmarkLoadGuessFunction';
traj.plotFunctionName     = 'MarsBenchmarkPlotFunction';
traj.showPlot             = true();
traj.plotUpdateRate       = 100;
traj.costLowerBound       = -Inf;
traj.costUpperBound       = Inf;
traj.maxMeshRefinementCount = 60;

bogusGuessI = config.StateScaleMat*[-2.612780114384178E+04;  1.328257092868841E+05;  5.757956041029914E+04; -2.981220584493626E-02; -4.955837697510423E-03; -2.146951323568623E-03; 3]*1000;

bogusGuessF = config.StateScaleMat*[-2.444427441837995E+05;  3.779124150799458E+04;  2.392934774236356E+03; -3.405553850853295E-03; -1.982966469562482E-02; -9.003561269272474E-03; 2]*1000;

% controlBoundU = [ 10000  10000 1]';
% controlBoundL = [-10000 -10000 0]';

controlBoundU = [1 1 1]';
controlBoundL = [-1 -1 -1]';
method = 1;
%==========================================================================
%% =====  Define Properties for Phase 1
%==========================================================================

%  Set mesh properities
if method == 1;
    phase1 = RadauPhase;
    phase1.initialGuessMode = 'UserGuessFunction';
    meshIntervalFractions =  [-1,-.5,0,.5,1];
    meshIntervalNumPoints = [6,6,3,3];
elseif method == 2
    phase1 = ImplicitRungeKuttaPhase;
    phase1.SetTranscription('RungeKutta8')
    phase1.initialGuessMode = 'UserGuessFunction';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [7];
end

%  Set time properties
timeLowerBound    = phaseTimes(1)/config.TU;
initialGuessTime  = phaseTimes(1)/config.TU;
finalGuessTime    = phaseTimes(2)/config.TU;
timeUpperBound    = phaseTimes(3)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase1.SetNumStateVars(numStateVars)
phase1.SetNumControlVars(numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions);
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound(stateLowerBound)
phase1.SetStateUpperBound(stateUpperBound)
phase1.SetStateInitialGuess(initialGuessState)
phase1.SetStateFinalGuess(finalGuessState)
phase1.SetTimeLowerBound(timeLowerBound)
phase1.SetTimeUpperBound(timeUpperBound)
phase1.SetTimeInitialGuess(initialGuessTime)
phase1.SetTimeFinalGuess(finalGuessTime)
phase1.SetControlLowerBound(controlLowerBound)
phase1.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% =====  Define Properties for Phase 2
%==========================================================================

%  Set mesh properities
if method == 1;
    phase2                         = RadauPhase;
    phase2.initialGuessMode         = 'UserGuessFunction';
    meshIntervalFractions   =  [-1,-.5,0,.5,1];
    meshIntervalNumPoints   = [3,3,3,3];
elseif method == 2
    phase2 = ImplicitRungeKuttaPhase;
    phase2.SetTranscription('RungeKutta8')
    phase2.initialGuessMode = 'UserGuessFunction';
    meshIntervalFractions = [0 1];
    meshIntervalNumPoints = [7];
end

%  Set time properties
timeLowerBound    = .5*phaseTimes(2)/config.TU;
initialGuessTime  =    phaseTimes(2)/config.TU;
finalGuessTime    =    phaseTimes(3)/config.TU;
timeUpperBound    =    phaseTimes(3)/config.TU;

%  Set state state properties
numStateVars      = 7;
stateLowerBound   = config.StateScaleMat*[-SevenAU -SevenAU -SevenAU -45 -45 -45 20]';
initialGuessState = bogusGuessI;
finalGuessState   = bogusGuessF;
stateUpperBound   = config.StateScaleMat*[SevenAU SevenAU SevenAU 45 45 45 5000]';

%  Set control properties
numControlVars    = 3;
controlUpperBound = controlBoundU;
controlLowerBound = controlBoundL;

phase2.SetNumStateVars(numStateVars)
phase2.SetNumControlVars(numControlVars);
phase2.SetMeshIntervalFractions(meshIntervalFractions);
phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase2.SetStateLowerBound(stateLowerBound)
phase2.SetStateUpperBound(stateUpperBound)
phase2.SetStateInitialGuess(initialGuessState)
phase2.SetStateFinalGuess(finalGuessState)
phase2.SetTimeLowerBound(timeLowerBound)
phase2.SetTimeUpperBound(timeUpperBound)
phase2.SetTimeInitialGuess(initialGuessTime)
phase2.SetTimeFinalGuess(finalGuessTime)
phase2.SetControlLowerBound(controlLowerBound)
phase2.SetControlUpperBound(controlUpperBound)

%==========================================================================
%% ===== Plot the boundary orbits
%==========================================================================

figure(1)
cnt = 0;
clear earthPos astPos
for time = 0:0.01:7
    mjd = config.TimeOffSet + time*365.25;
    cnt = cnt + 1;
    
    [stateEarth] = cspice_spkezr('EARTH', ...
        (mjd - 21545.00074286953)*86400, 'J2000', 'NONE','SUN');
    [stateAsteroid] = cspice_spkezr('MARS',...
        (mjd - 21545.00074286953)*86400, 'J2000', 'NONE','SUN');
    earthPos(cnt,:) = stateEarth(1:3)/config.DU;
    astPos(cnt,:) = stateAsteroid(1:3)/config.DU;
end
hold on
plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
plot3(astPos(:,1),astPos(:,2),astPos(:,3),'k-')
view([0 0 1])
axis equal
axis([-5 2 -5 2 -3 3])
grid on

%==========================================================================
%% =====  Define the linkage configuration and optimize
%==========================================================================

traj.phaseList = {phase1,phase2};
[z,F,xmul,Fmul] = traj.Optimize();

%==========================================================================
%% =====  Plot the solution
%==========================================================================
%clf
numPhasesToPlot = 2;
% [constraintFunctions,ifEquality] = traj.GetScaledConstraintFunctions();
% constraintNames = traj.GetConstraintVectorNames();
% 
% for i = 1:length(ifEquality)
%     if ifEquality(i) && abs(constraintFunctions(i)) > 1e-6
%         disp([constraintNames{i},' (equality) is possibly infeasible with value: ',num2str(constraintFunctions(i))])
%     elseif constraintFunctions(i) < -1e-6 || constraintFunctions(i) > 1+1e-6
%         disp([constraintNames{i},' is possibly infeasible with scaled value: ',num2str(constraintFunctions(i),20)])
%     end
% end

for phaseIdx = 1:numPhasesToPlot
    disp(['Phase ',num2str(phaseIdx)])
    frac_str = 'meshIntervalFractions   =  [';
    num_str = 'meshIntervalNumPoints   = [';
    fracs = traj.phaseList{phaseIdx}.GetMeshIntervalFractions();
    nums = traj.phaseList{phaseIdx}.GetMeshIntervalNumPoints();
    for i_f = 1:length(fracs)
        frac_str = [frac_str,num2str(fracs(i_f))];
        if i_f ~= 1
            num_str = [num_str,num2str(nums(i_f-1))];
        end
        if i_f ~= length(fracs)
            frac_str = [frac_str,','];
            if i_f ~= 1
                num_str = [num_str,','];
            end
        else
            frac_str = [frac_str,'];'];
            num_str = [num_str,'];'];
        end
    end
    disp(frac_str)
    disp(num_str)
    
    stateSol = traj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    
    times = traj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
    
    timeTypeVec = ones(1,length(times));
    timeTypeVec(end) = 2;
    
    if traj.phaseList{phaseIdx}.GetNumControlVars()
        nThruster = zeros(1,length(times)-1);
        power_in = zeros(1,length(times)-1);
        % 		net_mdot = zeros(1,length(times)-1);
        % 		net_thrust = zeros(1,length(times)-1);
        
        for i = 1:length(controlSol)
            [Xdot,A]= pathObj.SEPForces.ComputeForcesGivenControl(times(i),config.StateUnScaleMat*stateSol(i,:)',controlSol(i,:)');
            nThruster(i) = pathObj.SEPForces.Spacecraft.ThrusterSet.numToFire;
            power_in(i) = traj.pathFunctionObject.SEPForces.Spacecraft.GetThrustPower();
            % 			net_mdot(i) = -pathObj.SEPForces.Spacecraft.ThrusterSet.net_mdot*1e6;
            % 			net_thrust(i) = abs(pathObj.SEPForces.Spacecraft.ThrusterSet.net_thrust)*1e4;
        end
        
        figure(3); hold on;
        if isa(traj.phaseList{phaseIdx},'exec.RadauPhase') 
            timeVec = times(1:end-1);
        else
            timeVec = times;
        end
        plot(timeVec,nThruster')
        plot(timeVec,power_in')
        % 		plot(times(1:length(times)-1),net_mdot')
        % 		plot(times(1:length(times)-1),net_thrust')
    end
    
    
    %     SphericalControlCAESARNewGuessFunction(times,timeTypeVec,traj.phaseList{phaseIdx}.GetNumControlVars(),1);
    
    figure(1); hold on;
    fac = .3;
    controlSol = traj.phaseList{phaseIdx}.DecVector.GetControlArray();
    if traj.phaseList{phaseIdx}.GetNumControlVars()
        lineWidth = 2;
        lineColor = 'b';
    else
        lineWidth = 2;
        lineColor = 'b';
    end
    plot3(stateSol(:,1),stateSol(:,2),stateSol(:,3),lineColor,'LineWidth',lineWidth);
    for i = 1:length(controlSol)
        startX = stateSol(i,1);
        startY = stateSol(i,2);
        startZ = stateSol(i,3);
        
        %         dir1 = controlSol(i,3) * sin(controlSol(i,1)) * cos(controlSol(i,2));
        %         dir2 = controlSol(i,3) * sin(controlSol(i,1)) * sin(controlSol(i,2));
        %         dir3 = controlSol(i,3) * cos(controlSol(i,1));
        dir1 = controlSol(i,1);
        dir2 = controlSol(i,2);
        dir3 = controlSol(i,3);
        
        endX = stateSol(i,1) + fac*dir1;
        endY = stateSol(i,2) + fac*dir2;
        endZ = stateSol(i,3) + fac*dir3;
        plot3([startX;endX],[startY;endY],[startZ;endZ],'r-');
    end
    axis([-3 3 -3 3 -3 3])
    xlab = xlabel('Heliocentric X, AU');
    ylab = ylabel('Heliocentric Y, AU');
    set(xlab,'fontsize',15)
    set(ylab,'fontsize',15)
    
    figure(2); hold on;
    if isa(traj.phaseList{phaseIdx},'RadauPhase') && traj.phaseList{phaseIdx}.GetNumControlVars()
        % 	    plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,1),'DisplayName',['Phase ',num2str(phaseIdx),' x-control'])
        % 	    plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,2),'DisplayName',['Phase ',num2str(phaseIdx),' y-control'])
        % 	    plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,3),'DisplayName',['Phase ',num2str(phaseIdx),' z-control'])
        plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,1),'k','DisplayName','control azimuth','linewidth',2)
        plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,2),'b','DisplayName','control declination','linewidth',2)
        plot(times(1:end-1)*config.TU/86400/365.25,controlSol(:,3),'r','DisplayName','control magnitude','linewidth',2)
        plot(times(1:end-1)*config.TU/86400/365.25,(controlSol(:,3).*controlSol(:,3)+controlSol(:,2).*controlSol(:,2)+controlSol(:,1).*controlSol(:,1)).^.5,'g','DisplayName','control magnitude','linewidth',2)
    end
    
    figure(4); hold on;
    plot(times*config.TU/86400/365.25,stateSol(:,7)*config.MU,'k','linewidth',2)
    
    
    
end

figure(4)
% lg = legend('EMTG initial guess','GMAT optimal control');
% set(lg,'fontsize',15)
xlab = xlabel('Time since launch, years');
ylab = ylabel('Wet Mass, kg');
set(xlab,'fontsize',15)
set(ylab,'fontsize',15)

figure(2)
lg = legend('x control','y control','z control','net magnitude');
set(lg,'fontsize',15,'location','eastoutside')
xlab = xlabel('Time since launch, years');
% ylab = ylabel('rad or pct');
ylab = ylabel('Percent of Available Thrust');
set(xlab,'fontsize',15)
set(ylab,'fontsize',15)
set(gcf,'color','w')
axis([0,1.6,-1,1])


%figure(2)
%legend on
figure(1)
cnt = 0;
clear earthPos astPos
for time = 0:0.01:7
    mjd = config.TimeOffSet + time*365.25;
    cnt = cnt + 1;
    
    [stateEarth] = cspice_spkezr('EARTH', ...
        (mjd - 21545.00074286953)*86400, 'J2000', 'NONE','SUN');
    [stateAsteroid] = cspice_spkezr('MARS',...
        (mjd - 21545.00074286953)*86400, 'J2000', 'NONE','SUN');
    earthPos(cnt,:) = stateEarth(1:3)/config.DU;
    astPos(cnt,:) = stateAsteroid(1:3)/config.DU;
end
hold on
plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
plot3(astPos(:,1),astPos(:,2),astPos(:,3),'k-')
view([0 0 1])
axis equal
axis([-2 2 -2 2 -3 3])
grid on
set(gcf,'color','w')
