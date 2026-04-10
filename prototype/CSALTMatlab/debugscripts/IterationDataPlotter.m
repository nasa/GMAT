ClearAll

% lastDecVecLength = 0;
% delta0 = 1000;
% delta = 0;
% current_i = 1;
%
% load(['Iteration15070_data.mat'],'traj')
%
% finalLength = length(traj.decisionVector);
%
% while 1
%
% 	next_i = current_i + delta;
%
% 	if next_i > 15070
%
% 		next_i = 15070;
%
% 	end
%
% 	load(['Iteration',num2str(next_i),'_data.mat'],'traj')
%
% 	if length(traj.decisionVector) ~= lastDecVecLength
%
% 		if delta > 1
%
% 			delta = floor(delta/2);
%
% 		else
% 			lastDecVecLength = length(traj.decisionVector);
% 			disp(['New Iteration at ',num2str(next_i)])
% 			delta = delta0;
% 			current_i = next_i;
%
% 			if lastDecVecLength == finalLength
% 				disp('I ended things at this iteration')
% 				break
% 			end
%
% 			for p = 1:6
%
% 			    controlSol = traj.phaseList{p}.DecVector.GetControlArray();
% 			    	times = traj.phaseList{p}.TransUtil.GetTimeVector();
%
% 				figure(1); hold on;
% 				title(['Iteration ',num2str(current_i)])
% 				if isa(traj.phaseList{p},'RadauPhase') && traj.phaseList{p}.GetNumControlVars()
% 				    plot(times(1:end-1),controlSol(:,1),'DisplayName',['Phase ',num2str(p),' x-control'])
% 				    plot(times(1:end-1),controlSol(:,2),'DisplayName',['Phase ',num2str(p),' y-control'])
% 				    plot(times(1:end-1),controlSol(:,3),'DisplayName',['Phase ',num2str(p),' z-control'])
% 				end
% 			end
%
% 			pause
% 		end
% 	else
% 		current_i = next_i;
% 	end
%
% 	clearvars -except lastDecVecLength delta0 current_i next_i delta finalLength
% end


for current_i = 29:30
% for current_i = 30:8358

	load(['Iteration',num2str(current_i),'_data.mat'],'traj')
	
	if current_i == 29
		string = '-.';
	else
		string = '-';
	end
	
	for p = 1:6

	    controlSol = traj.phaseList{p}.DecVector.GetControlArray();
	    	times = traj.phaseList{p}.TransUtil.GetTimeVector();

		figure(1); hold on;
		title(['Iteration ',num2str(current_i)])
		if isa(traj.phaseList{p},'RadauPhase') && traj.phaseList{p}.GetNumControlVars()
		    plot(times(1:end-1),controlSol(:,1),string,'DisplayName',['Phase ',num2str(p),' x-control'])
		    plot(times(1:end-1),controlSol(:,2),string,'DisplayName',['Phase ',num2str(p),' y-control'])
		    plot(times(1:end-1),controlSol(:,3),string,'DisplayName',['Phase ',num2str(p),' z-control'])
		end
	end

	% pause

	clearvars -except lastDecVecLength delta0 current_i next_i delta finalLength
end