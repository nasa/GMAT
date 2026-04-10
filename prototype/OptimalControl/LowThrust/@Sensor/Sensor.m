classdef Sensor < handle
    
    %SENSORS  Base class for sensors.
    
    properties (SetAccess = 'public')
        
        %  Location  
        %  Orientation
        %  Field of View
        %  Obscuration
        
        %  For all sensors
        boreSiteVector
        sensorOrientation
        
    end
    
    properties (SetAccess = 'private')

    end
    
    
    methods
        
        %  Intialize the sensor object
        function Initialize(obj)
            % unitize the boresite vector
            obj.boreSiteVector = obj.boreSiteVector/...
                norm(obj.boreSiteVector);
        end
       
    end
    
end