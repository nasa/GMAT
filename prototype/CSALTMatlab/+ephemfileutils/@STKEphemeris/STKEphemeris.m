classdef STKEphemeris < handle
    %EPHEMERIS A STK ephem file.
    
    %   Author: S. Hughes <steven.p.hughes@nasa.gov>
    %  Prelimary version only.  Parses basic file and loads data.
    
    properties
        
        % TODO not all of this data should be public (SPH)
        Segments
        StateTolerance = 1e-3 / 86400                       % 1 ms
        
        % SPH: Added these data
        numEphemPoints = 0;
        interpolationMethod = '';
        interpolationOrder = 0;
        centralBody = '';
        coordinateSystem = '';
        distanceUnit = 'm';
        numSegmentBoundaries = 0;
        segmentBoundaryTimes = [];
        numSegments = 0 %  Number of segments on the file
        EphemerisFile %  The file name and path
        firstEpochOnFile =  inf;  % initialize to end of time
        lastEpochOnFile  = -inf;  % initialize to beginning of time
        
        isValid = true;
        reasonInvalid = '';
        fileVersion = '';
        creationDate = '';
        originator = '';
        
    end
    
    methods
        function obj = Ephemeris()
            import ephemfileutils.STKEphemerisSegment
            obj.Segments = EphemerisSegment.empty();
        end
        
        function [ hasError, errMessage ] = ValidateOEMLine(~, line )
            %  Performs basic line validation (See Section 3.3.1 of OEM Spec.
            
            % Initialize data
            errMessage = '';
            hasError = false;
            
            % Empty lines are allowed, nothing to validate
            if isempty(line)
                return
            end
            
            %  Check that the line is not too long
            if length(line) > 254
                hasError = true;
                errMessage = strcat(errMessage,'  OEM file contains a line longer than 78 chars. ' );
            end
            
            % Check that characters are printable
            if ~isstrprop(line, 'print')
                hasError = true;
                errMessage = strcat(errMessage,'  OEM file contains a line with non-printable chars. ' );
            end
        end
        
        
        function obj = Validate(obj)
            
            % Parse the creation date which performs validation
            obj.parse_ephem_epoch(obj.creationDate);
            
            % Validate the version
            if size(obj.fileVersion,2) >3  || ~strcmp(obj.fileVersion(1),'1') || ...
                    ~strcmp(obj.fileVersion(2),'.') || ~strcmp(obj.fileVersion(3),'0') ;
                obj.isValid = false;
                obj.reasonInvalid = strcat(obj.reasonInvalid,...
                    ' OEM file has invalid version number ');
            end
            
            % Validate originator
            if isempty(obj.originator)
                obj.isValid = false;
                obj.reasonInvalid = strcat(obj.reasonInvalid,...
                    ' OEM file has invalid originator ');
            end
            for segIdx = 1:obj.numSegments
                [isValidSeg,errMessage] = obj.Segments(segIdx).Validate();
                if ~isValidSeg
                    obj.isValid = isValidSeg;
                    obj.reasonInvalid = strcat(obj.reasonInvalid,errMessage);
                end
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
        
        segmentNumber = GetSegmentNumberByUseableStartTime(obj, epoch);
        [state, centralBody] = GetState(obj, epoch);
        
        ParseEphemeris(obj)
    end
    
end
