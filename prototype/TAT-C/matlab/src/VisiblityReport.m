classdef VisiblityReport < handle
    %VISIBLITYREPORT. Base class for visibility reporting data
    
    properties
        % AbsoluteDate. Start date of the interval event
        startDate
        % AbsoluteDate. End date of the interval event
        endDate
    end
    
    methods
        function SetStartDate(obj,date)
            % Sets start date of interval event
            obj.startDate = date;
        end
        function SetEndDate(obj,date)
            % Sets end date of interval event
            obj.endDate = date;
        end
        function date = GetStartDate(obj)
            % Returns state date object of interval event
            date = obj.startDate;
        end
        function date = GetEndDate(obj)
            % Returns end date object of interval event
            date = obj.endDate;
        end
    end
end

