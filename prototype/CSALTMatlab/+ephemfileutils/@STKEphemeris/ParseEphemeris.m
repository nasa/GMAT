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
        %         [hasError,errMessage] = obj.ValidateOEMLine(line);
        %         if hasError
        %             obj.isValid = false;
        %             obj.reasonInvalid = strcat(obj.reasonInvalid,errMessage);
        %         end
        
        % if empty, discard
        lineIsEmpty = isempty(line);
        lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
        if lineIsEmpty || lineIsWhiteSpaceOnly
            continue;
        end
        
        %% Parse and set string fields
        
        % test for comment
        isComment = regexpi(line,'^\s*# ','once');
        if ~isempty(isComment)
            continue;
        end
        
        fileVersionStart = strfind(line,'stk.v.');
        if ~isempty(fileVersionStart)
            obj.fileVersion = line(fileVersionStart+6:end);
            continue;
        end
        
        scenarioEpochStart = strfind(line,'ScenarioEpoch');
        if ~isempty(scenarioEpochStart)
            continue
            epoch = eval(deblank(line(numEphemPointsStart+23:end)));
            obj.numEphemPoints = obj.obj.parse_ephem_epoch(epoch);
        end
        
        numEphemPointsStart = strfind(line,'NumberOfEphemerisPoints');
        if ~isempty(numEphemPointsStart)
            obj.numEphemPoints = eval(deblank(line(numEphemPointsStart+23:end)));
        end
        
        numEphemPointsStart = strfind(line,'InterpolationMethod');
        if ~isempty(numEphemPointsStart)
            obj.interpolationMethod = strtrim(line(numEphemPointsStart+19:end));
        end
        
        centralBodyStart = strfind(line,'CentralBody');
        if ~isempty(centralBodyStart)
            obj.centralBody = strtrim(line(centralBodyStart+11:end));
        end
        
        coordinateSystemStart = strfind(line,'CoordinateSystem');
        if ~isempty(coordinateSystemStart)
            obj.coordinateSystem= strtrim(line(coordinateSystemStart+16:end));
        end
        
        distanceUnitStart = strfind(line,'DistanceUnit');
        if ~isempty(distanceUnitStart)
            obj.distanceUnit= strtrim(line(distanceUnitStart+12:end));
        end
        
        
        %% Parse and set numeric fields
        interpolationOrderStart = strfind(line,'InterpolationOrder');
        if ~isempty(interpolationOrderStart)
            obj.interpolationOrder = eval(deblank(line(interpolationOrderStart+18:end)));
        end
        
        %% Parse and set boundary times
        beginSegmentBoundaryStart = strfind(line,'BEGIN SegmentBoundaryTimes');
        if ~isempty(beginSegmentBoundaryStart)
            line = fgetl(fid);
            lineNumber = lineNumber + 1;  %  for debugging
            endSegmentBoundaryStart = strfind(line,'END SegmentBoundaryTimes');
            while isempty(endSegmentBoundaryStart)
                obj.numSegmentBoundaries = obj.numSegmentBoundaries + 1;
                obj.segmentBoundaryTimes(obj.numSegmentBoundaries,1) = eval(deblank(line));
                line = fgetl(fid);
                lineNumber = lineNumber + 1;  %  for debugging
                endSegmentBoundaryStart = strfind(line,'END SegmentBoundaryTimes');
            end
        end
        obj.segmentBoundaryTimes(obj.numSegmentBoundaries+1,1) = Inf;
        
        %% Parse and set the data
        beginEphTimePosVelStart = strfind(line,'EphemerisTimePosVel');
        if ~isempty(beginEphTimePosVelStart)
            notDone = true;
            import ephemfileutils.STKEphemerisSegment
            for segIdx = 1:obj.numSegmentBoundaries
                obj.Segments{segIdx} = STKEphemerisSegment();
            end
            if isempty(obj.Segments)
                obj.Segments{1} = STKEphemerisSegment();
            end
            currentSegment = 1;
            while notDone
                
                % read the next line
                line = fgetl(fid);
                lineNumber = lineNumber + 1;  %  for debugging
                
                % empty line test
                lineIsEmpty = isempty(line);
                lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
                if lineIsEmpty || lineIsWhiteSpaceOnly
                    continue;
                end
                
                % end of file test
                endEphemStart = strfind(line,'END Ephemeris');
                if ~isempty(endEphemStart)
                    notDone = false;
                    return
                end
                
                % 
                dataLine = sscanf(line, '%f %f %f %f %f %f %f');
                time = dataLine(1);
                if obj.numSegmentBoundaries == 0
                    obj.Segments{1}.AddDataRecord(dataLine);
                elseif time <= obj.segmentBoundaryTimes(currentSegment + 1)
                    obj.Segments{currentSegment}.AddDataRecord(dataLine);
                    if time == obj.segmentBoundaryTimes(currentSegment + 1)
                        currentSegment = currentSegment + 1;
                    end
                end

            end
        end
        
    end

    ephemSegment.Interpolator = LagrangeInterpolator();

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

obj.Validate();

end



