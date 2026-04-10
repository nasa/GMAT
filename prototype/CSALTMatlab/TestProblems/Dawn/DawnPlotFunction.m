function MarsBenchmarkPlotFunction(obj)

global config

numPhasesToPlot = 12;
% [constraintFunctions,ifEquality] = obj.GetScaledConstraintFunctions();
% constraintNames = obj.GetConstraintVectorNames();
%
% for i = 1:length(ifEquality)
% 	if ifEquality(i) && abs(constraintFunctions(i)) > 1e-6
% 		disp([constraintNames{i},' (equality) is infeasible with value: ',num2str(constraintFunctions(i))])
% 	elseif constraintFunctions(i) < -1e-6 || constraintFunctions(i) > 1+1e-5
% 		disp([constraintNames{i},' is infeasible with scaled value: ',num2str(constraintFunctions(i),20)])
% 	end
% end

%if ifInfeasibleConstraint
%	total_mesh_pt_constraints = 0;
%	for phaseIdx = 1:numPhasesToPlot
%		%total_mesh_pt_constraints = total_mesh_pt_constraints + obj.phaseList{phaseIdx}.GetMeshIntervalNumPoints() * (obj.phaseList{phaseIdx}.GetNumControlVars()+obj.phaseList{phaseIdx}.GetNumStateVars());
%		total_mesh_pt_constraints = total_mesh_pt_constraints + obj.phaseList{phaseIdx}.GetMeshIntervalNumPoints() * obj.phaseList{phaseIdx}.GetNumStateVars();
%	end
%	disp(['Expect ',num2str(total_mesh_pt_constraints),' collocation point state constraints'])
%end

% blah2 = [];
% blah3 = [];
% blah4 = [];
for phaseIdx = 1:numPhasesToPlot

    stateSol = obj.phaseList{phaseIdx}.DecVector.GetStateArray();
    controlSol = obj.phaseList{phaseIdx}.DecVector.GetControlArray();

    times = obj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
		
	figure(1); hold on;
    fac = .6;
    if obj.phaseList{phaseIdx}.GetNumControlVars()
        lineWidth = 2;
        lineColor = 'k';
    else
        lineWidth = 2;
        lineColor = 'b';
    end
    plot3(stateSol(:,1),stateSol(:,2),stateSol(:,3),lineColor,'LineWidth',lineWidth);
    for i = 1:length(controlSol)
        startX = stateSol(i,1);
        startY = stateSol(i,2);
        startZ = stateSol(i,3);
        
        if strcmp(config.Control,'Spherical')
            dir1 = controlSol(i,3) * sin(controlSol(i,1)) * cos(controlSol(i,2));
            dir2 = controlSol(i,3) * sin(controlSol(i,1)) * sin(controlSol(i,2));
            dir3 = controlSol(i,3) * cos(controlSol(i,1));
        elseif strcmp(config.Control,'Cartesian')
            dir1 = controlSol(i,1);
            dir2 = controlSol(i,2);
            dir3 = controlSol(i,3);
        end
        
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
	

    if isa(obj.phaseList{phaseIdx},'exec.RadauPhase') 
        timeVec = times(1:end-1)*config.TU/86400/365.25;
    else
        timeVec = times(1:end)*config.TU/86400/365.25;
    end
	
	if obj.phaseList{phaseIdx}.GetNumControlVars()
	% if isa(obj.phaseList{phaseIdx},'RadauPhase') && obj.phaseList{phaseIdx}.GetNumControlVars()
	    plot(timeVec,controlSol(:,1),'DisplayName',['Phase ',num2str(phaseIdx),' x-control'])
	    plot(timeVec,controlSol(:,2),'DisplayName',['Phase ',num2str(phaseIdx),' y-control'])
	    plot(timeVec,controlSol(:,3),'DisplayName',['Phase ',num2str(phaseIdx),' z-control'])
	% elseif obj.phaseList{phaseIdx}.GetNumControlVars()
		% plot(times*config.TU/86400/365.25,controlSol(:,1))
	end
	
	figure(1)
end

figure(1)
cnt = 0;
clear earthPos astPos
for time = 0:0.01:7
    mjd = config.TimeOffSet + time*365.25;
    cnt = cnt + 1;

    [stateEarth] = cspice_spkezr('EARTH', ...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateMars] = cspice_spkezr('MARS',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateCeres] = cspice_spkezr('2000001',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    [stateVesta] = cspice_spkezr('2000004',...
        (mjd - 21544.5)*86400, 'J2000', 'NONE','SUN');
    earthPos(cnt,:) = stateEarth(1:3)/config.DU;
    marsPos(cnt,:)  = stateMars(1:3)/config.DU;
    ceresPos(cnt,:) = stateCeres(1:3)/config.DU;
    vestaPos(cnt,:) = stateVesta(1:3)/config.DU;
end
hold on
plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
plot3(marsPos(:,1),marsPos(:,2),marsPos(:,3),'k-')
plot3(ceresPos(:,1),ceresPos(:,2),ceresPos(:,3),'k-')
plot3(vestaPos(:,1),vestaPos(:,2),vestaPos(:,3),'k-')
view([0 0 1])
axis equal
axis([-3 3 -3 3 -3 3])
grid on


% blah = find(obj.PointFuncManager.boundJacData.initTimeJacPattern{1});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.initTimeJacPattern{2});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.finalTimeJacPattern{1});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.finalTimeJacPattern{2});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.initStateJacPattern{1});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.initStateJacPattern{2});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.finalStateJacPattern{1});
% blah2 = [blah2;blah(:)];
% blah = find(obj.PointFuncManager.boundJacData.finalStateJacPattern{2});
% blah2 = [blah2;blah(:)];
%
% disp(blah2)
% disp(blah3)
% disp(blah4)
% figure(1)
% cnt = 0;
% clear earthPos astPos
% for time = 0:0.01:7
%     mjd = config.TimeOffSet + time*365.25;
%     cnt = cnt + 1;
%
%     [stateEarth] = cspice_spkezr('EARTH', ...
%         (mjd - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');
%     [stateAsteroid] = cspice_spkezr('1000012',...
%         (mjd - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');
%     earthPos(cnt,:) = stateEarth(1:3)/config.DU;
%     astPos(cnt,:) = stateAsteroid(1:3)/config.DU;
% end
% hold on
% plot3(earthPos(:,1),earthPos(:,2),earthPos(:,3),'k-')
% plot3(astPos(:,1),astPos(:,2),astPos(:,3),'k-')
% view([0 0 1])
% axis equal
% axis([-6 2 -6 2 -3 3])
% grid on

return