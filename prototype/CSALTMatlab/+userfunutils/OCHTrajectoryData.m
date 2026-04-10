classdef OCHTrajectoryData < userfunutils.TrajectoryData
    %STKTrajectory Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % The path and name of the users ephem file
        fileNameAndPath = '';
        % Ephemris object.  Used to read and interpolate ephem data
        fileUtility
        % The algorighm used for control guess.  1 for velocity, otherwise
        % zeros
        controlMode = 1;
    end
    
    methods
        
        function obj = OCHTrajectoryData(fileNameAndPath)
            % Constructor
            
            % Set class data
            obj.fileNameAndPath = fileNameAndPath;
            
            % Intialize utility and read the file
            import ephemfileutils.OptimalControlHistoryFileUtil
            ephFile = OptimalControlHistoryFileUtil(obj.fileNameAndPath);
            obj.fileUtility = ephFile;
            
            %  TODO: Handle fact that these could change across segments
            obj.numStateParams = ephFile.Segments{1}.numStates;
            obj.numControlParams = ephFile.Segments{1}.numControls;
            obj.numIntegralParams = ephFile.Segments{1}.numIntegrals;
            
        end
        
        function stateValues = GetState(obj,stateTimes)
            % Return state interpolated to requested times
            numTimes = length(stateTimes);
            stateValues = zeros(numTimes,obj.numStateParams);
            for timeIdx = 1:numTimes
                interpData = obj.fileUtility.GetState(stateTimes(timeIdx));
                stateValues(timeIdx,1:obj.numStateParams) = interpData(1:obj.numStateParams);
            end
        end
        
        function controlValues = GetControl(obj,controlTimes)
            % Returns control based on controlMode field
            % Return state interpolated to requested times
            numTimes = length(controlTimes);
            controlValues = zeros(numTimes,obj.numControlParams);
            for timeIdx = 1:numTimes
                interpData = obj.fileUtility.GetState(controlTimes(timeIdx));
                controlValues(timeIdx,1:obj.numControlParams) = ...
                    interpData(obj.numStateParams+1:obj.numStateParams+obj.numControlParams);
            end
        end
        
        function SetControlMode(obj,mode)
            %  Set mode for control initial guess
            obj.controlMode = mode;
        end
        
        function WriteData(obj,fileName)
            
            if nargin == 1
                fileName = obj.FileName;
            end
            
            fileHandle = fopen(fileName,'w');
            for segIdx = 1:obj.fileUtility.numSegments
                
                % Write meta data
                fprintf(fileHandle, '%s\n','');
                fprintf(fileHandle, '%s\n','META_START');
                fprintf(fileHandle,	'%s %s\n',...
                    '	CENTER_NAME   =', obj.fileUtility.Segments{segIdx}.CenterName);
                fprintf(fileHandle,	'%s %s\n',...
                    '	REf_FRAME     =', obj.fileUtility.Segments{segIdx}.RefFrame);
                fprintf(fileHandle,	'%s %s\n',...
                    '	TIME_SYSTEM   =', obj.fileUtility.Segments{segIdx}.TimeSystem);
                fprintf(fileHandle,	'%s %i\n',...
                    '	NUM_STATES    =', obj.fileUtility.Segments{segIdx}.numStates);
                fprintf(fileHandle,	'%s %i\n',...
                    '	NUM_CONTROLS  =', obj.fileUtility.Segments{segIdx}.numControls);
                fprintf(fileHandle,	'%s %i\n',...
                    '	NUM_INTEGRALS =', obj.fileUtility.Segments{segIdx}.numIntegrals);
                fprintf(fileHandle, '%s\n','META_STOP');
                
                % Write data
                fprintf(fileHandle, '%s\n','');
                numCols = 1 + obj.fileUtility.Segments{segIdx}.numStates + ...
                    obj.fileUtility.Segments{segIdx}.numControls + ...
                    obj.fileUtility.Segments{segIdx}.numIntegrals;
                formatSpec = '';
                for colIdx = 1:numCols
                    formatSpec = [formatSpec '%16.9f '];
                end
                formatSpec = [formatSpec ' \n'];
                for recordIdx = 1:size(obj.fileUtility.Segments{segIdx}.Data,1)
                    fprintf(fileHandle,formatSpec,...
                        obj.fileUtility.Segments{segIdx}.Data(recordIdx,:));
                end
                fclose(fileHandle)
                
            end
            
        end
        
    end
end

