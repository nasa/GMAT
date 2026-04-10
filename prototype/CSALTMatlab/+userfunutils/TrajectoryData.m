classdef TrajectoryData < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = 'protected')
        % The times associated with the array of control data
        controlGuessTimes = [];
        % The control values provided for the guess
        controlGuessValues = [];
        % The times associated with the array of state data
        stateGuessTimes = [];
        % The control values provided for the guess
        stateGuessValues = [];
        % Number of control parameters in the problem
        numControlParams = 0;
        % Number of state parameters in the problem
        numStateParams = 0;
        % Number of control points in the users guess data (not the same as
        % collocation times)
        numControlTimes = 0;
        % Number of state points in the users guess data (not the same as
        % collocation times)
        numStateTimes = 0;
        numIntegralParams = 0;
        integralGuessTimes = [];
        integralGuessValues = [];
    end
    
    methods
%         function obj = GuessInterpolator(numStateParams,numControlParams,numIntegralParams)
%             obj.numStateParams = numStateParams;
%             obj.numControlParams = numControlParams;
%         end
        function obj = SetControlGuessArrays(obj,controlGuessTimes,controlGuessValues)
            % Set arrays for control values and associated times
            obj.controlGuessTimes = controlGuessTimes;
            obj.controlGuessValues = controlGuessValues;
            obj.numControlParams = size(controlGuessValues,2);
            obj.numControlTimes = size(controlGuessTimes,1);
            if size(controlGuessTimes,2) ~=1
                error('time vector must be column vector')
            end
            if size(controlGuessTimes,1) ~= size(controlGuessValues,1)
                error('number of Control rows and time rows must be the same')
            end
        end
        
        function obj = SetStateGuessArrays(obj,stateGuessTimes,stateGuessValues)
            % Set arrays for control values and associated times
            obj.stateGuessTimes = stateGuessTimes;
            obj.stateGuessValues = stateGuessValues;
            obj.numStateParams = size(stateGuessValues,2);
            obj.numStateTimes = size(stateGuessTimes,1);
            if size(stateGuessTimes,2) ~=1
                error('time vector must be column vector')
            end
            if size(stateGuessTimes,1) ~= size(stateGuessValues,1)
                error('number of state rows and time rows must be the same')
            end
        end
        
        function [controlValues] = GetControl(obj,controlTimes)
            % Return interpolated control values given times
            controlValues = interp1(obj.controlGuessTimes,obj.controlGuessValues,...
                controlTimes,'spline','extrap');
        end
        
        function [stateValues] = GetState(obj,stateTimes)
            % Return state control values given times
            stateValues = interp1(obj.stateGuessTimes,obj.stateGuessValues,...
                stateTimes,'spline','extrap');
        end
    end
    
end

