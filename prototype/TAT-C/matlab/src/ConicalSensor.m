classdef ConicalSensor < Sensor
    %CONICALSENSOR a basic conical sensor model
    
    properties (Access = 'private')
        % Double.  Sensor field of view in radians
        fieldOfView
    end
    
    methods
        function obj = ConicalSensor(fieldOfView)
            % Class constructor.  
            % I/O.  fieldOfView in radians.
            obj.fieldOfView = fieldOfView;
            obj.maxExcursionAngle = fieldOfView;
        end
        function obj = SetFieldOfView(obj,fieldOfView)
            % Sets field of view in radians
           obj.fieldOfView = fieldOfView;
        end
        function fieldOfView = GetFieldOfView(obj)
            % Returns field of view in radians
           fieldOfView = obj.fieldOfView; 
        end
        function inView = CheckTargetVisibility(obj,viewConeAngle,~)
            % Determines if a point is in sensor FOV
            inView = CheckTargetMaxExcursionAngle(obj,viewConeAngle);
        end
    end
    
end

