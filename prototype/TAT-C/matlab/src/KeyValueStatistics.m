classdef KeyValueStatistics < handle
    %KEYVALUESTATISTICS. Container for statistics
    
    properties
        % Double. Minimum value
        minValue
        % Double. Maximum value
        maxValue
        % Double. Average value
        avgValue
    end
    
    methods
        function obj = KeyValueStatistics(min,max,avg)
            %  Constructor.  Sets min, max, avg.
            obj.minValue = min;
            obj.maxValue = max;
            obj.avgValue = avg;
        end
        function min = GetMinValue(obj)
            % Returns the minimum value
            min = obj.minValue;
        end
        function max = GetMaxValue(obj)
            % Returns the maximum value
            max = obj.maxValue;
        end
        function avg = GetAvgValue(obj)
            % Returns the average value
            avg = obj.avgValue;
        end
    end
    
end

