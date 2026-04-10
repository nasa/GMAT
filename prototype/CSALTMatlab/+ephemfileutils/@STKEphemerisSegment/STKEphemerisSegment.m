classdef STKEphemerisSegment < handle
    %STKEphemerisSegment A segment of a STK Ephem file.
    
    %   Author: S. Hughes <steven.p.hughes@nasa.gov>
    
    properties
        CentralBody
        Data = []
        numDataPoints = 0
        Interpolation
        InterpolationDegree
        Interpolator
        StartTime
        StopTime
        UseableStartTime
        UseableStopTime
        CenterName
        ObjectId
        ObjectName
        RefFrame
        TimeSystem
        metaStart
        metaStop
        hasStartTime
        hasStopTime
    end
    
    methods
        function [isValid,errMessage] = Validate(obj)
            % Note StartTime, StopTime, UsableStartTime and UsableStopTime
            isValid = true;
            errMessage = '';
            if isempty(obj.ObjectName)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "OBJECT_NAME" ');
            end
            
            if isempty(obj.metaStart)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "META_START" ');
            end
            if isempty(obj.metaStop)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "META_STOP" ');
            end
            if isempty(obj.ObjectId)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "OBJECT_ID" ');
            end
            if isempty(obj.CenterName)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "CENTER_NAME" ');
            end
            if isempty(obj.RefFrame)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "REF_FRAME" ');
            end
            if isempty(obj.TimeSystem)
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "TIME_SYSTEM" ');
            end
            if ~obj.hasStartTime
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "START_TIME" ');
            end
            if ~obj.hasStopTime
                isValid = false;
                errMessage = strcat(errMessage,' Segment is missing "STOP_TIME" ');
            end
            %  Test that start/stop times match first/last records on
            %  segment
            if obj.StartTime ~= obj.Data(1,1);
                isValid = false;
                errMessage = strcat(errMessage,' "START_TIME" of segment is not the same as the first record. ');
            end
            if obj.StopTime ~= obj.Data(end,1);
                isValid = false;
                errMessage = strcat(errMessage,' "STOP_TIME" of segment is not the same as the first record. ');
            end
            %  Test that usable start/stop falls within start/stop
            if ~isempty(obj.UseableStartTime) && (obj.StartTime > obj.UseableStartTime);
                isValid = false;
                errMessage = strcat(errMessage,' "START_TIME" is after "USABLE_START_TIME". ');
            end
            if ~isempty(obj.UseableStopTime) && (obj.StopTime < obj.UseableStopTime)
                isValid = false;
                errMessage = strcat(errMessage,' "STOP_TIME" is after "USABLE_STOP_TIME". ');
            end
            if obj.StopTime < obj.StartTime
                isValid = false;
                errMessage = strcat(errMessage,' "STOP_TIME" is before "START_TIME". ');
            end
            
        end
        
        function serialDate = parse_ephem_epoch(~,dateStr)
            
            dateIsType1 = ~isempty(regexpi(dateStr, ...
                '\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?Z?'));
            dateIsType2 = ~isempty(regexpi(dateStr, ...
                '\d{4}-\d{3}T\d{2}:\d{2}:\d{2}(?:\.\d+)?Z?'));
            
            if dateIsType1
                dateVec = sscanf(dateStr, '%4u-%2u-%2uT%2u:%2u:%f');
                serialDate = datenum(dateVec');%- datenum('01-Jan-2000');
            elseif dateIsType2
                dateVec = sscanf(dateStr, '%4u-%3uT%2u:%2u:%f');
                serialDate = datenum([dateVec(1), 0, dateVec(2:end)']);
            else
                throw(MException('Events:ParseEphemeris:InvalidDateFormat', ...
                    'Cannot convert date string %s', dateStr));
            end
            
        end
        
        function AddDataRecord(obj,record)
            obj.numDataPoints = obj.numDataPoints + 1;
            obj.Data(obj.numDataPoints,:) = record;
        end
    end
    
end
