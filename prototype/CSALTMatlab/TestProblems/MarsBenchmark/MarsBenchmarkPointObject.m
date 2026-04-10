classdef MarsBenchmarkPointObject < userfunutils.OrbitPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            
			obj.InitializeBoundsFunctionsAndNames();
		
			obj.AddDeclinationConstraintWithSpice(1,'399',-28.5*pi/180,28.5*pi/180);
			
			obj.AddInitialBoundaryConditionWithLaunchVehicleAndSpice(1,'399',365);
			
			obj.EnforceTimeDirectionality(1)
			obj.ConstrainStatesBetweenPhases(1,2,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(1,2,0,0,2)
			obj.EnforceTimeDirectionality(2)
			
			obj.AddFinalBoundaryConditionWithSpice(2,'499',(1:6)',zeros(1,6))

			obj.AddFlightTimeConstraint(2,546)
		
			obj.FinalizeBoundsFunctionsAndNames();
			
			finalState = obj.GetFinalStateVec(2);
			
			obj.SetCostFunction(-finalState(7))
			
			
			%             global config LV
			%
			%             %%  Set constants.
			%             R = [1.0 0.0 0.0;
			%                 0.0 0.91748206 -0.397777156;
			%                 0.0 0.39777716 0.9174820621]';
			%             Rmat = [R zeros(3,3);zeros(3,3) R];
			%
			%
			%             %%  Define functions and bounds for each phase
			%
			%             %Phase 1 Functions and bounds
			%             phaseNum = 1;
			%
			% mjd = config.TimeOffSet + obj.GetInitialTime(phaseNum) * config.TU / 86400;
			% 		    [stateEarth] = cspice_spkezr('EARTH',(mjd - 21545.00074286953)*86400, 'J2000', 'NONE','SUN');
			%
			%             %initSolMat = Rmat*stateEarth;
			%             initSolMat = stateEarth;
			% initialState = obj.GetInitialStateVec(phaseNum);
			% initialStateBound = config.StateScaleMat(1:6,1:6)*initSolMat;
			% vInf = (initialState(4:6) - initialStateBound(4:6))*config.VU;
			% C3 = vInf(1)*vInf(1)+vInf(2)*vInf(2)+vInf(3)*vInf(3);
			% % if C3 > 100
			% % 	keyboard
			% % end
			% maxInitialMass = LV.GetMassToEscapeC3(C3) / config.MU;
			% % maxInitialMass = 1;
			%       	  	vInfECI = R * vInf;
			%             declination = asin(vInfECI(3)/norm(vInfECI))*180/pi;
			%
			%             boundError = initialStateBound(1:3) - initialState(1:3);
			%
			% phase1BoundaryFunctions = [boundError(1:3); (initialState(7)/maxInitialMass)-1; obj.GetInitialTime(phaseNum)/config.launchWindow;(obj.GetFinalTime(phaseNum)-obj.GetInitialTime(phaseNum))/config.maxFlightTime; declination/6];
			%             phase1ConLowerBound =     [     zeros(3,1);                                  0;                                                0;                                                                          1e-8;            -1];
			%             phase1ConUpperBound =     [     zeros(3,1);                                  0;                                                1;                                                                             1;             1];
			% phase1Descriptions  = {'Initial Boundary Condition - State 1',...
			% 					   'Initial Boundary Condition - State 2',...
			% 					   'Initial Boundary Condition - State 3',...
			% 					   'Initial Boundary Condition - State 7',...
			% 					   'Initial Time Bound',...
			% 					   'Phase 1 Time Directionality',...
			% 					   'Launch Declination'};
			%
			% phaseNum = 2;
			%
			% phase2BoundaryFunctions = [(obj.GetFinalTime(phaseNum)-obj.GetInitialTime(phaseNum))/config.maxFlightTime];
			%             phase2ConLowerBound =     [1e-8];
			%             phase2ConUpperBound =     [1];
			% phase2Descriptions  = {'Phase 2 Time Directionality'};
			%
			%             finalTimeLowerBound = [0];
			% finalTimeUpperBound = [1];
			%
			% mjd = config.TimeOffSet + obj.GetFinalTime(phaseNum) * config.TU / 86400;
			% 		    [stateMars] = cspice_spkezr('MARS', (mjd - 21545.00074286953)*86400, 'J2000', 'NONE','SUN');
			%             finalStateBound = config.StateScaleMat(1:6,1:6)*stateMars;
			%
			%    		 	finalTime              = obj.GetFinalTime(phaseNum);
			%             finalState             = obj.GetFinalStateVec(phaseNum);
			% finalStateError        = finalState(1:6)-finalStateBound(1:6);
			%             finalBoundaryFunctions = [finalTime/config.maxFlightTime;finalStateError ];
			%             finalConLowerBound     = [           finalTimeLowerBound;     zeros(6,1) ];
			%             finalConUpperBound     = [           finalTimeUpperBound;     zeros(6,1) ];
			% finalDescriptions      = {'Final Time',...
			% 					      'Final Boundary Condition - State 1',...
			% 				     	  'Final Boundary Condition - State 2',...
			% 						  'Final Boundary Condition - State 3',...
			% 						  'Final Boundary Condition - State 4',...
			% 						  'Final Boundary Condition - State 5',...
			% 						  'Final Boundary Condition - State 6'};...
			%
			% finalMass = finalState(7);
			% obj.SetCostFunction(-(finalMass)^2);
			%
			%             %  Linkage constraints
			%             link1 = [obj.GetInitialTime(2) - obj.GetFinalTime(1); ...
			%                      obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1)];
			%             link1LowerBound = zeros(8,1);
			%             link1UpperBound = zeros(8,1);
			% link1Descriptions = {'Phase 1 and 2 link - Time',...
			% 					 'Phase 1 and 2 link - State 1',...
			% 					 'Phase 1 and 2 link - State 2',...
			% 					 'Phase 1 and 2 link - State 3',...
			% 					 'Phase 1 and 2 link - State 4',...
			% 					 'Phase 1 and 2 link - State 5',...
			% 					 'Phase 1 and 2 link - State 6',...
			% 					 'Phase 1 and 2 link - State 7'};
			%
			%             %%  Set the functions and bounds
			%             obj.SetAlgFunctions([phase1BoundaryFunctions;...
			%             	phase2BoundaryFunctions;...
			%                 link1;...
			%                 finalBoundaryFunctions])
			%
			%             obj.SetAlgFuncLowerBounds([phase1ConLowerBound;...
			%             	phase2ConLowerBound;...
			%                 link1LowerBound;...
			%                 finalConLowerBound])
			%
			%             obj.SetAlgFuncUpperBounds([phase1ConUpperBound;...
			%             	phase2ConUpperBound;...
			%                 link1UpperBound;...
			%                 finalConUpperBound])
			%
			% obj.SetAlgFuncNames({phase1Descriptions{1:end},...
			%             	 phase2Descriptions{1:end},...
			%                  link1Descriptions{1:end},...
			%                  finalDescriptions{1:end}})
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

% Keeping this around for later as it will be needed later
% [finalStateAsteroid] = cspice_spkezr('2000336',...
%     (finalMJD - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');
% finalBoundError = config.StateScaleMat(1:6,1:6)*(finalState(1:6) - finalStateAsteroid);
% obj.boundaryFunctions = finalBoundError ;



