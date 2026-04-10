classdef AbsoluteDate < handle
    % ABSOLUTEDATE: Utility for setting and converting between date formats
    %   Does not handle leap seconds
    
    properties (Access = private)
        %  Double.  The current date in Julian Day format
        currentDate = 2154500.0;
    end
    
    methods (Access = public)
        function obj = SetGregorianDate(obj,Year,Month,Day,Hour,Minute,Seconds)
            % Sets Gregorian date with input
            if Year < 1900 || Year > 2100
                error('Invalid Year provided to AbsoluteDate()');
            end
            if Month < 0 || Month > 12
                error('Invalid Month provided to AbsoluteDate()')
            end
            if Day < 0 || Day >= 31 % %TODO, check day by the month.
                error('Invalid Month provided to AbsoluteDate()')
            end
            if Hour < 0 || Hour >= 24
                error('Invalid Hour provided to AbsoluteDate()')
            end
            if Minute < 0 || Minute >= 60
                error('Invalid Hour provided to AbsoluteDate()')
            end
            if Seconds < 0 || Seconds >= 60
                error('Invalid Seconds provided to AbsoluteDate()')
            end
            obj.currentDate = obj.GregorianToJulianDate(Year,Month,Day,Hour,Minute,Seconds);
        end
        
        function obj = SetJulianDate(obj,julianDate)
            % Sets the Julian Date
            obj.currentDate =  julianDate;
        end
        
        function julianDate  = GetJulianDate(obj)
            % Sets the Julian Date
            julianDate = obj.currentDate;
        end
        
        function dateVec = GetGregorianDate(obj)
            % Returns the Gregorian date
            
            lMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
            jd = obj.currentDate;
            t1900 = (jd - 2415019.5) / 365.25;
            year = 1900 + fix(t1900);
            leapYrs = fix((year - 1900 - 1)*0.25);
            days = (jd - 2415019.5) - ((year - 1900) * 365 + leapYrs);
            
            % Handle leap year
            if days < 1
                year = year - 1;
                leapYrs = fix((year - 1900 - 1)*0.25);
                days = (jd - 2415019.5) - ((year - 1900) * 365 + leapYrs);
            end
            if mod(year, 4) == 0
                lMonth(2) = 29;
            end
            dayOfYr = fix(days);
            
            sumDays = 0;
            for i = 1:length(lMonth)
                sumDaysTemp = sumDays + lMonth(i);
                if sumDaysTemp >= dayOfYr
                    break;
                else
                    sumDays = sumDaysTemp;
                end
            end
            mon = i;
            
            day = dayOfYr - sumDays;
            tau = (days - dayOfYr) * 24;
            h = fix(tau);
            min = fix((tau - h) * 60);
            s = (tau - h - min/60) * 3600;
            
            dateVec = [year mon day h min s];
            
        end
        
        function Advance(obj,timeStepInSeconds)
            % Increments the time by the input number of seconds
            obj.currentDate = obj.currentDate + timeStepInSeconds/86400;
        end
        
    end
    
    methods (Access = private)
        
        function julianDate = GregorianToJulianDate(~,year,month,day,hour,min,sec)
            % Converts a Gregorian date to a Julian date
            jDay = 367 * year - fix(7 * (year + fix((month + 9) / 12)) / 4) + ...
                fix(275 * month / 9) + day + 1721013.5;
            partOfDay = ((sec/60 + min) / 60 + hour) / 24;
            julianDate = (jDay) + partOfDay;
        end
        
    end
end



