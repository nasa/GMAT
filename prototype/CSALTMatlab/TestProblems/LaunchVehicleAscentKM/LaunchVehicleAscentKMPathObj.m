classdef LaunchVehicleAscentKMPathObj < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Set constants
            thrustN =  2.1e6/1000;
            initialMassKG = 117000;
            referenceAreaM2 = 7.069/(1000^2);
            dragCoefficient= 0.5;
            specificImpulseSEC = 265.2;
            refDensityKGPerM3 = 1.225/(1000^3);
            scaleheightM = 8.44e3/1000;
            gMPerS2 =9.80665/1000;
            
            % Get state, control and time quantities
            stateVec = obj.GetStateVec();

            y = stateVec(2);
            vx = stateVec(3);
            vy = stateVec(4);
            u = obj.GetControlVec();
            theta = atan2(u(2),u(1));
            time = obj.GetTime();
            
            % Compute and set the dynamics
            gamma = atan2(vy,vx);
            mass = initialMassKG - thrustN/gMPerS2/specificImpulseSEC*time;
            drag = 0.5*refDensityKGPerM3*dragCoefficient*referenceAreaM2...
                *exp(-y/scaleheightM)*sqrt(vx^2 + vy^2);
            Fx = thrustN/mass*cos(theta) - drag/mass*cos(gamma);
            Fy = thrustN/mass*sin(theta) - drag/mass*sin(gamma) - gMPerS2;            
            obj.SetDynFunctions([vx vy Fx Fy]');
            
%             magControl = norm(u);
%             
%             obj.SetAlgFunctions(magControl);
%             obj.SetAlgFuncUpperBounds(1);
%             obj.SetAlgFuncLowerBounds(0);
            
        end
        
        function EvaluateJacobians(obj)
            
            
        end
    end
    
end



