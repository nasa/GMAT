classdef SimpleConicSensor < Sensor
    
    %SIMPLECONICSENSOR class for conical sensors.
    
    properties
        coneHalfAngle 
    end
    
    methods
        
        %  Determine if target is in field of view of sensor
        function flag = IsInFieldOfView(obj,targetVector)
            if acos(dot(obj.boreSiteVector,targetVector)/...
                    norm(obj.boreSiteVector)/norm(targetVector)) < ...
                obj.coneHalfAngle*pi/180;
                flag = true();
            else
                flag = false();
            end
        end
        
    end
    
end

