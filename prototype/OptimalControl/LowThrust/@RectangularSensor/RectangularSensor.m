classdef RectangularSensor < Sensor
    
    %SIMPLECONICSENSOR class for conical sensors.
    
    properties
        fovWidth
        fovHeight
    end
    
    methods
        
        %  Determine if target is in field of view of sensor
        function flag = IsInFieldOfView(obj,targetVector)
            
            %  If on the wrong side of the sensor, return
            if dot(targetVector,obj.boreSiteVector < 0)
                flag = 0;
                return
            end
            
            %  Check the width angle
            targPlaneVec = [targetVector(1) targetVector(2) 0 ]';
            boreSiteVec = [obj.boreSiteVector(1) obj.boreSiteVector(2) 0 ];
            if acos(dot(boreSiteVec,targPlaneVec)/...
                    norm(boreSiteVec)/norm(targPlaneVec)) < ...
                    obj.fovWidth*pi/180;
                widthFlag = true();
            else
                widthFlag  = false();
            end
            
            %  Check the height angle
            targHeightVector = [0 0 targetVector(3)]';
            boreSiteVec = [0 0 obj.boreSiteVector(3)]'
            if acos(dot(obj.boreSiteVector,targHeightVector)/...
                    norm(obj.boreSiteVector)/norm(targHeightVector)) <= ...
                    obj.fovHeight*pi/180;
                heightFlag = true();
            else
                heightFlag  = false();
            end
            
            %  Determine if in field of view
            if  widthFlag && heightFlag
                flag = true;
            else
                flag = false;
            end
            
        end
        
    end
    
end
