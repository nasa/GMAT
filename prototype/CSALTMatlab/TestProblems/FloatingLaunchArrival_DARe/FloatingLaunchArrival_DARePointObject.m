classdef FloatingLaunchArrival_DARePointObject < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            global config LV
            
            
            %%  Set constants.
            R = [1.0 0.0 0.0;
                0.0 0.91748206 -0.397777156;
                0.0 0.39777716 0.9174820621]';
            Rmat = [R zeros(3,3);zeros(3,3) R];
            
            
            %%  Define functions and bounds for each phase

            %Phase 1 Functions and bounds
            phaseNum = 1;
			
			mjd = config.TimeOffSet + obj.GetInitialTime(phaseNum) * config.TU / 86400;
		    [stateEarth] = cspice_spkezr('EARTH',(mjd - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');

            %initSolMat = Rmat*stateEarth;
            initSolMat = stateEarth;
			initialState = obj.GetInitialStateVec(phaseNum);
			initialStateBound = config.StateScaleMat*[initSolMat;1697];
			vInf = (initialState(4:6) - initialStateBound(4:6))*1000;
			initialMass = LV.GetMassToEscapeC3(vInf(1)*vInf(1)+vInf(2)*vInf(2)+vInf(3)*vInf(3));
			iniitalStateBound(7) = initialMass / config.MU;

            boundError = [initialStateBound(1:3);initialStateBound(7)] - [initialState(1:3);initialState(7)];
            phase1BoundaryFunctions = [boundError; obj.GetInitialTime(phaseNum);               obj.GetFinalTime(phaseNum)];
            phase1ConLowerBound =     [zeros(4,1);                            0; 1.03212593380217 - config.launchWindow/2];
            phase1ConUpperBound =     [zeros(4,1);          config.launchWindow; 1.03212593380217 + config.launchWindow/2];
            
            %  Phase 2 Functions and bounds
            phaseNum = 2;
            phase2BoundaryFunctions = obj.GetFinalTime(phaseNum);
            phase2ConLowerBound = [11];
            phase2ConUpperBound = [12];
            
            %  Phase 3 Functions and bounds
            phaseNum = 3;
            phase3BoundaryFunctions = obj.GetFinalTime(phaseNum);
            phase3ConLowerBound = [14];
            phase3ConUpperBound = [15];
            
            %  Phase 4 Functions and bounds
            phaseNum = 4;
			mjd = config.TimeOffSet + obj.GetFinalTime(phaseNum) * config.TU / 86400;
		    [stateAsteroid] = cspice_spkezr('2000336', (mjd - 21545.00074286953)*86400, 'ECLIPJ2000', 'NONE','SUN');
            %finalTimeTarget = 18.5717924156372;
            %finalTarget = config.StateScaleMat(1:6,1:6)*Rmat*stateAsteroid;
            finalTarget = config.StateScaleMat(1:6,1:6)*stateAsteroid;
            finalState = obj.GetFinalStateVec(phaseNum);
            phase4BoundaryFunctions = [finalTarget - finalState(1:6);obj.GetFinalTime(phaseNum)-obj.GetInitialTime(1)];
            phase4ConLowerBound = [zeros(6,1);0];
            phase4ConUpperBound = [zeros(6,1);config.maxFlightTime];
            finalMass = finalState(7);
            obj.SetCostFunction(-finalMass);
            
            %  Linkage constraints
            
            %  Linkage constraints
            link1 = [obj.GetInitialTime(2) - obj.GetFinalTime(1); ...
                obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1)];
            link1LowerBound = zeros(8,1);
            link1UpperBound = zeros(8,1);
            
            link2 = [obj.GetInitialTime(3) - obj.GetFinalTime(2); ...
                obj.GetInitialStateVec(3) - obj.GetFinalStateVec(2)];
            link2LowerBound = zeros(8,1);
            link2UpperBound = zeros(8,1);
            
            link3 = [obj.GetInitialTime(4) - obj.GetFinalTime(3); ...
                obj.GetInitialStateVec(4) - obj.GetFinalStateVec(3)];
            link3LowerBound = zeros(8,1);
            link3UpperBound = zeros(8,1);
			           
            %%  Set the functions and bounds
            obj.SetAlgFunctions([phase1BoundaryFunctions;...
                phase2BoundaryFunctions;...
                phase3BoundaryFunctions;...
                phase4BoundaryFunctions; link1;link2;link3])
            
            obj.SetAlgFuncLowerBounds([phase1ConLowerBound;...
                phase2ConLowerBound;...
                phase3ConLowerBound;...
                phase4ConLowerBound;
                link1LowerBound;
                link2LowerBound;
                link3LowerBound])
            
            obj.SetAlgFuncUpperBounds([phase1ConUpperBound;...
                phase2ConUpperBound;...
                phase3ConUpperBound;...
                phase4ConUpperBound;
                link1LowerBound;
                link2LowerBound;
                link3LowerBound                ])
            
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



