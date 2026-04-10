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
fid = fopen(obj.fileName);

% process an ephemeris section
lineNumber = 0;
debug      = false();
obj.numSegments = 0;
obj.reasonInvalid = '';

%  These are required meta data for segments so set to empty.  If not filled in later,
%  validator will trap the error

while ~feof(fid)
    
    % find START_TIME line
    while ~feof(fid)
        
        % read in next line
        line = fgetl(fid);
        lineNumber = lineNumber + 1;  %  for debugging
        [hasError,errMessage] = obj.ValidateOEMLine(line);
        if hasError
            obj.isValid = false;
            obj.reasonInvalid = strcat(obj.reasonInvalid,errMessage);
        end
        
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
            hasStartTime = true;
            startTime = obj.parse_ephem_epoch(startTimeStr{1});
            continue
        end
        
        % test for STOP_TIME
        stopTimeStr = regexpi(line, ...
            '^\s*STOP_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(stopTimeStr)
            hasStopTime = true;
            stopTime = obj.parse_ephem_epoch(stopTimeStr{1});
            continue
        end
        
        % test for USEABLE_START_TIME
        useableStartTimeStr = regexpi(line, ...
            '^\s*USEABLE_START_TIME\s*=\s*(\S+)\s*$', ...
            'tokens', 'once');
        if ~isempty(useableStartTimeStr)
            useableStartTime = obj.parse_ephem_epoch( ...
                useableStartTimeStr{1});
            continue
        end
        
        % test for USEABLE_STOP_TIME
        useableStopTimeStr = regexpi(line, ...
            '^\s*USEABLE_STOP_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(useableStopTimeStr)
            useableStopTime = obj.parse_ephem_epoch(useableStopTimeStr{1});
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
        
        % test for NUM_STATES
        numStatesStr = regexpi(line, ...
            '^\s*NUM_STATES\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(numStatesStr)
            numStates = ...
                sscanf(numStatesStr{1}, '%i');
            continue;
        end
        
        % test for NUM_STATES
        numControlsStr = regexpi(line, ...
            '^\s*NUM_CONTROLS\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(numControlsStr)
            numControls = ...
                sscanf(numControlsStr{1}, '%i');
            continue;
        end
        
        % test for NUM_INTEGRALS
        numIntegralsStr = regexpi(line, ...
            '^\s*NUM_INTEGRALS\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(numIntegralsStr)
            numIntegrals = ...
                sscanf(numIntegralsStr{1}, '%i');
            continue;
        end
        
        % test for file version number test for comment
        fileVersionStr = regexpi(line, ...
            '^\s*CCSDS_OEM_VERS\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(fileVersionStr)
            obj.fileVersion = fileVersionStr{1};
            continue;
        end
        
        % test for creation date
        creationDateStr = regexpi(line, ...
            '^\s*CREATION_DATE\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(creationDateStr)
            obj.creationDate = creationDateStr{1};
            continue;
        end
        
        % test for object name
        objectNameStr = regexpi(line, ...
            '^\s*OBJECT_NAME\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(objectNameStr)
            objectName = objectNameStr{1};
            continue;
        end
        
        % test for object id
        objectIdStr = regexpi(line, ...
            '^\s*OBJECT_ID\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(objectIdStr)
            objectId= objectIdStr{1};
            continue;
        end
        
        % test for time system
        timeSystemStr = regexpi(line, ...
            '^\s*TIME_SYSTEM\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(timeSystemStr)
            timeSystem= timeSystemStr{1};
            continue;
        end
        
        % test for originator
        originatorStr = regexpi(line, ...
            '^\s*ORIGINATOR\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if strfind(line,'ORIGINATOR')
            startIdx = strfind(line,'=');
            obj.originator = line(startIdx+1:end);
            continue;
        end
        
        % test for meta start
        if strfind(line,'META_START')
            % a new segment has started set fields to blank
            objectId = '';
            fileVersion = '';
            creationDate = '';
            originator = '';
            centerName = '';
            timeSystem = '';
            metaStart = '';
            metaStop = '';
            refFrame = '';
            timeSystem = '';
            objectName = '';
            hasStartTime = false;
            hasStopTime = false;
            metaStart = 'META_START';
            continue
        end
        
        % test for meta stop
        if strfind(line,'META_STOP')
            metaStop = 'META_STOP';
            line = fgetl(fid);
            lineNumber = lineNumber + 1;  %  for debugging
            break
        end
        
        % test for comment
        isComment = regexpi(line,'^\s*COMMENT ','once');
        if ~isempty(isComment)
            continue;
        end
        
        % test for first data line
%         lineIsData = ~isempty(regexpi(line, ...
%             '^\s*[0-9\-T:\.Z]+(?:\s+[\-\+0-9\.e]+){6,9}\s*$', 'once'));
%         if lineIsData
%             % end of metadata section
%             break;
%         end
        
        % Field never got set so it is invalid
        obj.isValid = false;
        errMessage = [' Line ' num2str(lineNumber) ' contains an invalid field. '];
        obj.reasonInvalid = strcat(obj.reasonInvalid,errMessage);
        
    end
    
    % process data lines
    data  = [];
    isEOF = feof(fid);
    while ~isEOF
        
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
        dataLine = sscanf(line, '%f %f %f %f %f %f %f');
        epochStr = char(dataLine(1:end-6)');
        %epoch = obj.parse_ephem_epoch(epochStr);
        %state = dataLine(end-5:end)';
        %data = [data; [epoch, state]];
        state = dataLine;
        
        % save data
        data = [data; state'];
        
        % get next line
        isEOF = feof(fid);
        line  = fgetl(fid);
        lineNumber = lineNumber + 1;  %  for debugging
        
    end
    
    % assign appropriate interpolation function
    if exist('interpolation', 'var')
        if strcmpi(interpolation, 'LAGRANGE')
            %import ephemfileutils.Interpolator
            import ephemfileutils.LagrangeInterpolator
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
    import ephemfileutils.OptimalControlSegment
    import ephemfileutils.LagrangeInterpolator
    ephemSegment = OptimalControlSegment();
    ephemSegment.Interpolator = LagrangeInterpolator;
    ephemSegment.InterpolationDegree = 7;
    ephemSegment.CentralBody = centerName;
    ephemSegment.Data = data;
    ephemSegment.CenterName = centerName;
    ephemSegment.metaStart = metaStart;
    ephemSegment.metaStop = metaStop;
    ephemSegment.RefFrame = refFrame;
    ephemSegment.TimeSystem = timeSystem;
    ephemSegment.numStates = numStates;
    ephemSegment.numControls = numControls;
    ephemSegment.numIntegrals = numIntegrals;
    ephemSegment.StartTime = ephemSegment.Data(1,1);
    ephemSegment.StopTime = ephemSegment.Data(end,1);
    obj.numSegments = obj.numSegments + 1;
    obj.Segments{obj.numSegments} = ephemSegment;
    
end

%  Compute first and last epochs on the file
for segIdx = 1:length(obj.Segments)
    
    if obj.Segments{segIdx}.StartTime < obj.firstEpochOnFile
        obj.firstEpochOnFile = obj.Segments{segIdx}.StartTime;
    end
    if obj.Segments{segIdx}.StopTime > obj.lastEpochOnFile
        obj.lastEpochOnFile = obj.Segments{segIdx}.StopTime;
    end
end

% finalize
fclose(fid);

obj.Validate();

end



