function [data,centralBody,coordinateSystem,scenarioEpoch] =...
    ReadEMTGGuessFile(fileNameAndPath)

fid = fopen(fileNameAndPath);

lineNumber = 0;
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
        
        % test for Scenario Epoch
        %scenarioEpochStr = regexpi(line, ...
        %    '^\s*ScenarioEpoch(\S+)\s*$', 'tokens', 'once');
        
        if ~isempty(strfind(line,'ScenarioEpoch'));
            scenarioEpoch = line(15:end);
            baseEpoch = datenum(scenarioEpoch);
            continue;
        end
        
        % test for Central Body
        refFrameStr = regexpi(line,'^\s*CentralBody\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(refFrameStr)
            centralBody = refFrameStr{1};
            continue;
        end
        
        % test for CoordinateSystem
        coordSysString = regexpi(line,...
            '^\s*CoordinateSystem\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(coordSysString)
            coordinateSystem = coordSysString{1};
            continue
        end
        
        % test for first data line EphemerisTimePosVel
        lineIsData = ~isempty(strfind(line,'EphemerisTime'));
        if lineIsData
            % end of metadata section
            line = fgetl(fid);
            break;
        end
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
            '^\s*END Ephemeris\s*$', 'once'));
        lineIsCovarianceStart = ~isempty(regexpi(line, ...
            '^\s*COVARIANCE_START\s*$', 'once'));
        isComment =  ~isempty(regexpi(line,'^\s*COMMENT ','once'));
        if lineIsSectionStart || lineIsCovarianceStart || isComment
            break;
        end
        
        % read in data line
        dataLine = sscanf(line, '%f %f %f %f %f %f %f');
        
        % Extract the data
%         epoch = dataLine(1,1);
%         state = dataLine(2:7,1)'/1000;
        
        % save data
        data = [data; dataLine'];
        
        % get next line
        isEOF = feof(fid);
        line  = fgetl(fid);
        lineNumber = lineNumber + 1;  %  for debugging
        
    end
    
end

% finalize
fclose(fid);
