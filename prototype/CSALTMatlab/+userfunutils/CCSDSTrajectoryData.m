classdef CCSDSTrajectoryData < userfunutils.TrajectoryData
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
        
        function obj = CCSDSTrajectoryData(fileNameAndPath)
            % Constructor
            
            % Set class data
            obj.fileNameAndPath = fileNameAndPath;
            obj.numStateParams = 7;
            obj.numControlParams = 3;
            
            % Intialize utility and read the file
            import ephemfileutils.Ephemeris
            ephFile = Ephemeris();
            ephFile.EphemerisFile = obj.fileNameAndPath;
            ephFile.ParseEphemeris()
            obj.fileUtility = ephFile;
            
        end
        
        function stateValues = GetState(obj,stateTimes)
            % Return state interpolated to requested times
            numTimes = length(stateTimes);
            stateValues = zeros(numTimes,7);
            for timeIdx = 1:numTimes
                stateValues(timeIdx,1:6) = obj.fileUtility.GetState(stateTimes(timeIdx));
            end
        end

        function controlValues = GetControl(obj,controlTimes)
            % Returns control based on controlMode field
            numTimes = length(controlTimes);
            controlValues = zeros(numTimes,3);
            for timeIdx = 1:numTimes
                if obj.controlMode == 1
                    state =  obj.fileUtility.GetState(controlTimes(timeIdx));
                    controlValues(timeIdx,1:3) = state(4:6)'/norm(state(4:6));
                else
                    controlValues(timeIdx,1:3) = zeros(3,1);
                end
            end
        end
        
        function SetControlMode(obj,mode)
            %  Set mode for control initial guess
            obj.controlMode = mode;
        end
        
    end
    
end

