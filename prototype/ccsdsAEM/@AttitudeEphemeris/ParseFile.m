function ParseFile(obj)

%  Parse a CCSDS AEM file.  Written by S. Hughes.  Borrowed from OEM file
%  reader by Joel J. K. Parker 

% process an ephemeris section
lineNumber = 0;     %  The line number, used for debug.
fileLocation = 1;   %  fileLocation is the location in the file  
                    %  1: file header
                    %  2: meta data
                    %  3: attitude data

%% Step through the whole file
while ~feof(obj.fileHandle)
    
    % read in next line
    line = fgetl(obj.fileHandle);
    lineNumber = lineNumber + 1;  %  for debugging
    
    %% parse the header
    if fileLocation == 1
        
        % if empty, discard
        lineIsEmpty = isempty(line);
        lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
        if lineIsEmpty || lineIsWhiteSpaceOnly
            continue;
        end
        
        % test for CCSDS_AEM_VERS
        parseResult = regexpi(line, ...
            '^\s*CCSDS_AEM_VERS\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            obj.CCSDS_AEM_VERS = parseResult{1};
            continue;
        end
        
        % test for CREATION_DATE
        parseResult = regexpi(line, ...
            '^\s*CREATION_DATE\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            obj.CREATION_DATE = parseResult{1};
            continue;
        end
        
        % test for ORIGINATOR
        parseResult = regexpi(line, ...
            '^\s*ORIGINATOR\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            obj.ORIGINATOR = parseResult{1};
            continue;
        end
        
        % test for comment
        parseResult = regexpi(line,'^\s*COMMENT ','once');
        if ~isempty(parseResult)
            if isempty(obj.CCSDS_AEM_VERS)
                disp(['Warning: Comment occurs before version number'...
                    ' in AEM file "' obj.EphemerisFile '"']);
            end
            %  TODO: store the comment in case of write out
            continue;
        end
        
        % test for META_START
        lineIsMetaStart = ~isempty(regexpi(line, ...
            '^\s*META_START\s*$', 'once'));
        %  Validate header info if completed parsing the header
        if lineIsMetaStart
            if isempty(obj.CCSDS_AEM_VERS)
                disp(['Error: AEM version number not set in'...
                    ' in AEM file "' obj.EphemerisFile '"']);
                return
            end
            if isempty(obj.CREATION_DATE)
                disp(['Warning: CREATION_DATE not set in'...
                    ' in AEM file "' obj.EphemerisFile '"']);
            end
            if isempty(obj.ORIGINATOR)
                disp(['Warning: ORIGINATOR not set in'...
                    ' in AEM file "' obj.EphemerisFile '"']);
            end
            fileLocation = 2;
            %% parse metadata block
            
            %  Get an emtpy structure to populate with incoming data.  This
            %  will be passed to the segment object later.
            metaStruct = GetEmptyMetaDataStruct(obj);
            isNewSegment = true();
            continue
        end
        
        %  If code gets to this point, line does not meet any above
        %  requirements and is invalid.
        disp(['Warning: The following line in '...
            ' in AEM file "' obj.EphemerisFile '" appears to be invalid: ']);
        disp(line)
             
    end
    
    if fileLocation == 2
        
        % if empty, discard
        lineIsEmpty = isempty(line);
        lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
        if lineIsEmpty || lineIsWhiteSpaceOnly
            continue;
        end

        %  test for comment
        isComment = regexpi(line,'^\s*COMMENT ','once');
        if ~isempty(isComment)
            % TODO: save comments if we want to write back out.
            continue;
        end
        
        %  test for META_START of multiple blocks
        parseResult = regexpi(line, ...
            '^\s*META_START\s*$', 'once');
        if ~isempty(parseResult)
            continue;
        end
        
        % test for ATTITUDE_DIR
        parseResult = regexpi(line, ...
            '^\s*ATTITUDE_DIR\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.ATTITUDE_DIR = parseResult{1};
            continue;
        end
        
        % test for ATTITUDE_TYPE
        parseResult = regexpi(line, ...
            '^\s*ATTITUDE_TYPE\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.ATTITUDE_TYPE = parseResult{1};
            continue;
        end
        
        % test for CENTER_NAME
        parseResult = regexpi(line, ...
            '^\s*CENTER_NAME\s*=\s*(.*?)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.CENTER_NAME = parseResult{1};
            continue;
        end
        
        % test for EULER_ROT_SEQ
        interpolationStr = regexpi(line, ...
            '^\s*EULER_ROT_SEQ\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(interpolationStr)
            metaStruct.EULER_ROT_SEQ = interpolationStr{1};
            continue;
        end
        
        % test for INTERPOLATION
        interpolationStr = regexpi(line, ...
            '^\s*INTERPOLATION_METHOD\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(interpolationStr)
            metaStruct.INTERPOLATION_METHOD = interpolationStr{1};
            continue;
        end
        
        % test for INTERPOLATION_DEGREE
        interpolationDegreeStr = regexpi(line, ...
            '^\s*INTERPOLATION_DEGREE\s*=\s*(\S+)\s*$', 'tokens', ...
            'once');
        if ~isempty(interpolationDegreeStr)
            metaStruct.INTERPOLATION_DEGREE = ...
                sscanf(interpolationDegreeStr{1}, '%i');
            continue;
        end
        
        % test for OBJECT_ID
        parseResult = regexpi(line, ...
            '^\s*OBJECT_ID\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.OBJECT_ID = parseResult{1};
            continue;
        end
        
        % test for OBJECT_NAME
        parseResult = regexpi(line, ...
            '^\s*OBJECT_NAME\s*=\s*(.*?)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.OBJECT_NAME = parseResult{1};
            continue;
        end
        
        % test for RATE_FRAME
        parseResult = regexpi(line, ...
            '^\s*RATE_FRAME\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.RATE_FRAME = parseResult{1};
            continue;
        end
        
        % test for REF_FRAME_A
        parseResult = regexpi(line, ...
            '^\s*REF_FRAME_A\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.REF_FRAME_A = parseResult{1};
            continue;
        end
        
        % test for REF_FRAME_B
        parseResult = regexpi(line, ...
            '^\s*REF_FRAME_B\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.REF_FRAME_B = parseResult{1};
            continue;
        end
        
        % test for QUATERNION_TYPE
        parseResult = regexpi(line, ...
            '^\s*QUATERNION_TYPE\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.QUATERNION_TYPE = parseResult{1};
            continue;
        end
        
        % test for TIME_SYSTEM
        parseResult = regexpi(line, ...
            '^\s*TIME_SYSTEM\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(parseResult)
            metaStruct.TIME_SYSTEM = parseResult{1};
            continue;
        end
        
        % test for START_TIME
        startTimeStr = regexpi(line, ...
            '^\s*START_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(startTimeStr)
            metaStruct.START_TIME = startTimeStr{1};
            continue
        end
        
        % test for STOP_TIME
        stopTimeStr = regexpi(line, ...
            '^\s*STOP_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(stopTimeStr)
            metaStruct.STOP_TIME = stopTimeStr{1};
            continue
        end
        
        % test for USEABLE_START_TIME
        useableStartTimeStr = regexpi(line, ...
            '^\s*USEABLE_START_TIME\s*=\s*(\S+)\s*$', ...
            'tokens', 'once');
        if ~isempty(useableStartTimeStr)
            metaStruct.USEABLE_START_TIME = useableStartTimeStr{1};
            continue
        end
        
        % test for USEABLE_STOP_TIME
        useableStopTimeStr = regexpi(line, ...
            '^\s*USEABLE_STOP_TIME\s*=\s*(\S+)\s*$', 'tokens', 'once');
        if ~isempty(useableStopTimeStr)
            metaStruct.USEABLE_STOP_TIME = useableStopTimeStr{1};
            continue;
        end
        
        % if META_STOP, discard
        lineIsMetaStop = ~isempty(regexpi(line,...
            '^\s*META_STOP\s*$', 'once'));
        if lineIsMetaStop
            continue;
        end
        
        % test for first data line
        lineIsSectionStart = ~isempty(regexpi(line, ...
            '^\s*DATA_START\s*$', 'once'));
        if lineIsSectionStart
            fileLocation = 3;
            continue
        end
        
        %  If code gets to this point, line does not meet any above
        %  requirements and is invalid.
        disp(['Warning: The following line in '...
            ' in AEM file "' obj.EphemerisFile '" appears to be invalid: ']);
        disp(line)
        
    end
    
    %% configure segment and parse attitude data block
    if fileLocation == 3
        
        %  Create a Segment of the required ATTITUDE_TYPE
        if isNewSegment
            if strcmpi(metaStruct.ATTITUDE_TYPE,'quaternion')
                currentSegment = QuaternionSegment;
                
            else
                disp(['Error:  Unsupported ATTITUDE_TYPE "' ...
                      metaStruct.ATTITUDE_TYPE ...
                    ' in AEM file "' obj.EphemerisFile '"']);
                return
            end
            
            %  Initialize the Segment using meta data
            currentSegment.Initialize(metaStruct)
            isNewSegment = false();
        end
        
        % discard empty lines
        lineIsEmpty = isempty(line);
        lineIsWhiteSpaceOnly = ~isempty(regexp(line, '^\s*$', 'once'));
        if lineIsEmpty || lineIsWhiteSpaceOnly
            continue;
        end
        
        % skip comment
        isComment =  ~isempty(regexpi(line,'^\s*COMMENT','once'));
        if  isComment
            continue;
        end
        
        % if at end of data, save the segment and initialize for possible
        % next segment
        isComment =  ~isempty(regexpi(line,'^\s*DATA_STOP','once'));
        if  isComment
            %  Get an emtpy structure to populate with incoming data.  This
            %  will be passed to the segment object later.
            AddSegment(obj,currentSegment);
            metaStruct = GetEmptyMetaDataStruct(obj);
            isNewSegment = true();
            fileLocation = 2;
            continue;
        end
        
        % continue if reached next meta data start
        lineIsSectionStart = ~isempty(regexpi(line, ...
            '^\s*META_START\s*$', 'once'));
        if lineIsSectionStart
            continue
        end

        % skip the comment line
        isComment =  ~isempty(regexpi(line,'^\s*COMMENT ','once'));
        if  isComment
            % TODO: save the comment if we want to write back out
            continue;
        end
        
        % add data to the segment
        currentSegment.AddData(line);

    end
    
end




