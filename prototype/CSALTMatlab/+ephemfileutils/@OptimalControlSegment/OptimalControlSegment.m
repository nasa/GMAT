classdef OptimalControlSegment < handle
    %EPHEMERISSEGMENT A segment of a CCSDS OEM file.
    
    %   Author: S. Hughes
    %   Modified from CCSDS reader by J. Parker
    
    properties
        CentralBody
        Data
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
        numStates
        numControls
        numIntegrals
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
    end
    
end
