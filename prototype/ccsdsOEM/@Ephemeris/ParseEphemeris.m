function ParseEphemeris(obj)
    %PARSEEPHEMERIS Parse a CCSDS OEM file.
    %   PARSEEPHEMERIS(OBJ) parses the CCSDS OEM ephemeris file
    %   associated with the active Application OBJ and places the resulting
    %   Ephemeris object in OBJ.Ephemeris.
    
    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    
    %  Changes:  
    % -SPH: Removed conversion to SPICE frame using MICE toolbox
    % -SPH: Removed call to GetCentralBody to set center name for SPICE
    % -SPH: Removed import of gmat.events and interplotor that was required
    % for the event locator project infrastructure.
    % -SPH: Added code to handle comment lines
    % -SPH: Added code to determine first and last epoch on the file.

    % open the ephemeris file
    fid = fopen(obj.EphemerisFile);
    
    % process an ephemeris section
    lineNumber = 0;
    debug      = false();
    while ~feof(fid)
        

        % find START_TIME line
        while ~feof(fid)
            % read in next line
            line = fgetl(fid);
            lineNumber = lineNumber + 1;  %  for debugging

            % if empty, discard
            lineIsEmpty = isempty(line);
            lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
            if lineIsEmpty || lineIsWhiteSpaceOnly
                continue;
            end

            % test for CENTER_NAME
            centerNameStr = regexpi(line, ...
                '^\s*CENTER_NAME\s*=\s*(\S+)\s*$', 'tokens', 'once');
            if ~isempty(centerNameStr)
                centerName = centerNameStr{1};
                continue;
            end
            
            % test for REF_FRAME
            refFrameStr = regexpi(line, ...
                '^\s*REF_FRAME\s*=\s*(\S+)\s*$', 'tokens', 'once');
            if ~isempty(refFrameStr)
                refFrame = refFrameStr{1};
                continue;
            end
            
            % test for START_TIME
            startTimeStr = regexpi(line, ...
                '^\s*START_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
            if ~isempty(startTimeStr)
                startTime = parse_ephem_epoch(startTimeStr{1});
                continue
            end

            % test for STOP_TIME
            stopTimeStr = regexpi(line, ...
                '^\s*STOP_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
            if ~isempty(stopTimeStr)
                stopTime = parse_ephem_epoch(stopTimeStr{1});
                continue
            end

            % test for USEABLE_START_TIME
            useableStartTimeStr = regexpi(line, ...
                '^\s*USEABLE_START_TIME\s*=\s*(\S+)\s*$', ...
                'tokens', 'once');
            if ~isempty(useableStartTimeStr)
                useableStartTime = parse_ephem_epoch( ...
                    useableStartTimeStr{1});
                continue
            end

            % test for USEABLE_STOP_TIME
            useableStopTimeStr = regexpi(line, ...
                '^\s*USEABLE_STOP_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
            if ~isempty(useableStopTimeStr)
                useableStopTime = parse_ephem_epoch(useableStopTimeStr{1});
                continue;
            end
            
            % test for INTERPOLATION
            interpolationStr = regexpi(line, ...
                '^\s*INTERPOLATION\s*=\s*(\S+)\s*$', 'tokens', 'once');
            if ~isempty(interpolationStr)
                interpolation = interpolationStr{1};
                continue;
            end
            
            % test for INTERPOLATION_DEGREE
            interpolationDegreeStr = regexpi(line, ...
                '^\s*INTERPOLATION_DEGREE\s*=\s*(\S+)\s*$', 'tokens', ...
                'once');
            if ~isempty(interpolationDegreeStr)
                interpolationDegree = ...
                    sscanf(interpolationDegreeStr{1}, '%i');
                continue;
            end
            
            %  test for comment
            isComment = regexpi(line,'^\s*COMMENT ','once');
            if ~isempty(isComment)
                continue;
            end

            % test for first data line
            lineIsData = ~isempty(regexpi(line, ...
                '^\s*[0-9\-T:\.Z]+(?:\s+[\-\+0-9\.e]+){6,9}\s*$', 'once'));
            if lineIsData
                % end of metadata section
                break;
            end
        end

        % process data lines
        data  = [];
        isEOF = feof(fid);
        while ~isEOF

            if debug
               disp(['line no. : ' num2str(lineNumber)]); 
            end
            
            % discard empty lines
            lineIsEmpty = isempty(line);
            lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
            if lineIsEmpty || lineIsWhiteSpaceOnly
                line = fgetl(fid);
                continue;
            end

            % break if data has stopped
            lineIsSectionStart = ~isempty(regexpi(line, ...
                '^\s*META_START\s*$', 'once'));
            lineIsCovarianceStart = ~isempty(regexpi(line, ...
                '^\s*COVARIANCE_START\s*$', 'once'));
            isComment =  ~isempty(regexpi(line,'^\s*COMMENT ','once'));
            if lineIsSectionStart || lineIsCovarianceStart || isComment
                break;
            end

            % read in data line
            dataLine = sscanf(line, '%s %f %f %f %f %f %f');
            epochStr = char(dataLine(1:end-6)');
            epoch = parse_ephem_epoch(epochStr);
            state = dataLine(end-5:end)';
            
            % save data
            data = [data; [epoch, state]];

            % get next line
            isEOF = feof(fid);
            line  = fgetl(fid);
            lineNumber = lineNumber + 1;  %  for debugging
            
        end

        % assign appropriate interpolation function
        if exist('interpolation', 'var')
            if strcmpi(interpolation, 'LAGRANGE')
                interpolator = LagrangeInterpolator;
            else
                throw(MException( ...
                    'Events:ParseEphemeris:UnsupportedInterpolator', ...
                    ['The ephemeris interpolator %s is currently ' ...
                    'unsupported.'], interpolation));
            end
        else
            % default interpolator
            interpolator = LagrangeInterpolator;
            interpolationDegree = 5;
        end
        
        % store data in Ephemeris object
        ephemSegment = EphemerisSegment();
        ephemSegment.CentralBody = centerName;
        ephemSegment.Interpolation = interpolation;
        ephemSegment.InterpolationDegree = interpolationDegree;
        ephemSegment.Interpolator = interpolator;
        ephemSegment.StartTime = startTime;
        ephemSegment.StopTime = stopTime;
        ephemSegment.UseableStartTime = useableStartTime;
        ephemSegment.UseableStopTime = useableStopTime;
        ephemSegment.Data = data;
        obj.Segments(end+1) = ephemSegment;
        obj.numSegments = length(obj.Segments);
        
    end
    
    %  Compute first and last epochs on the file
    for segIdx = 1:length(obj.Segments)
        if obj.Segments(segIdx).StartTime < obj.firstEpochOnFile
            obj.firstEpochOnFile = obj.Segments(segIdx).StartTime;
        end
        if obj.Segments(segIdx).StopTime > obj.lastEpochOnFile
            obj.lastEpochOnFile = obj.Segments(segIdx).StopTime;
        end
    end
    
    % finalize
    fclose(fid);
    
end

function serialDate = parse_ephem_epoch(dateStr)
    
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
