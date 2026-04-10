classdef DawnPointObject < userfunutils.OrbitPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
			obj.InitializeBoundsFunctionsAndNames();
			
			obj.AddDeclinationConstraintWithSpice(1,'399',-28.5*pi/180,28.5*pi/180);
			
			obj.AddInitialBoundaryConditionWithLaunchVehicleAndSpice(1,'399',0);
% 			obj.ConstrainInitialC3WithSpice(1,'399',5.1529,5.2285,2)
			obj.ConstrainInitialMass(1,800,0,3)
			
			obj.EnforceTimeDirectionality(1)
			obj.ConstrainStatesBetweenPhases(1,2,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(1,2,0,0,2)
			obj.EnforceTimeDirectionality(2)
			obj.ConstrainStatesBetweenPhases(2,3,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(2,3,0,0,2)
			obj.EnforceTimeDirectionality(3)
			obj.ConstrainStatesBetweenPhases(3,4,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(3,4,0,0,2)
			obj.EnforceTimeDirectionality(4)
			obj.ConstrainStatesBetweenPhases(4,5,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(4,5,0,0,2)
			
			obj.EnforceTimeDirectionality(5)
			obj.AddFlightTimeConstraint(5,527,527)
			obj.Add2BodyFlybyBetweenPhases(5,6,'499',4.2828*10^4,3390+600)
			
			obj.EnforceTimeDirectionality(6)
			obj.ConstrainStatesBetweenPhases(6,7,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(6,7,0,0,2)
			
			obj.EnforceTimeDirectionality(7)
			obj.AddFinalBoundaryConditionWithSpice(7,'2000004',(1:6)',zeros(1,6))
			obj.AddInitialBoundaryConditionWithSpice(8,'2000004',(1:6)',zeros(1,6))
			obj.ConstrainStatesBetweenPhases(7,8,7,0)
			obj.ConstrainTimeBetweenPhases(7,8,270,0,3)
			
			obj.EnforceTimeDirectionality(8)
			obj.ConstrainStatesBetweenPhases(8,9,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(8,9,0,0,2)
			obj.EnforceTimeDirectionality(9)
			obj.ConstrainStatesBetweenPhases(9,10,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(9,10,0,0,2)
			obj.EnforceTimeDirectionality(10)
			obj.ConstrainStatesBetweenPhases(10,11,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(10,11,0,0,2)
			obj.EnforceTimeDirectionality(11)
			obj.ConstrainStatesBetweenPhases(11,12,(1:7)',zeros(1,7))
			obj.ConstrainTimeBetweenPhases(11,12,0,0,2)
			
			obj.EnforceTimeDirectionality(12)
			obj.AddFinalBoundaryConditionWithSpice(12,'2000001',(1:6)',zeros(1,6))
			obj.AddFlightTimeConstraint(12,0,3000)
		
			obj.FinalizeBoundsFunctionsAndNames();
			
			finalState = obj.GetFinalStateVec(12);
			
			obj.SetCostFunction(1-(finalState(7)/10)^2)
            
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



