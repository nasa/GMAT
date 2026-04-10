classdef UserGuessUtil < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % Real Array. The array containing state guess information
        stateArray
        % Real Array. The array containing control guess information
        controlArray
        % Real array. Time vector for the guess arrays (stateArray etc.)
        timeVec
        % Bool.  Has state guess array
        hasStateGuessArray = false;
        % Bool. Has control guess array
        hasControlGuessArray = false;
        % Bool. Has time vector array
        hasTimeGuessArray = false;
    end
    
    methods
        
        function SetStateGuessArray(obj,stateGuessArray)
            % Set state array for state guesses
            obj.hasStateGuessArray = true;
            obj.stateGuessArray = stateGuessArray;
        end
        
        function SetControlGuessArray(obj,controlGuessArray)
            % Set control array for control guesses
            obj.hasControlGuessArray = true;
            obj.controlGuessArray = controlGuessArray;
        end
        
        function SetTimeGuessArray(obj,timeGuessArray)
            % Set time array for time guesses
            obj.hasTimeGuessArray = true;
            obj.timeGuessArray = timeGuessArray;
        end
        
        function stateGuess = ComputeStateGuess(interpIndVar)
            % Returns the state guess array
            if ~obj.hasStateGuessArray
                error('User state guess array has not been set')
            end
            if ~obj.hasTimeGuessArray;
                error('User time guess array has not been set')
            end
            %obj.ValidateUserArrayDimensions()
            stateGuess = ComputeGuessArray(obj,interpIndVar);
        end
        
        function controlGuess = ComputeControlGuess(interpTimes)
            % Returns the control guess array
            if ~obj.hasControlGuessArray
                error('User state guess array has not been set')
            end
            if ~obj.hasTimeGuessArray;
                error('User time guess array has not been set')
            end
            %obj.ValidateUserArrayDimensions()
            obj.ComputeGuessArray(interpTimes,obj.controlArray)
        end

    end
    
    methods (Access = 'private')
        
        function ValidateUserArrayDimensions(obj)
            % Make sure the user set consistent dimensions on arrays.
        end
        
        function ComputeGuessArray(newIndVar,ptrIndVaryArray,ptrDepVaryArray)
            
        end
        
        function InterpolateArray()
            % Interpolate array to requested new times
            
        end
        
        function ExtrapolateArray()
            % Extrapolate array to requested new times
            
        end
        
    end
    
end


