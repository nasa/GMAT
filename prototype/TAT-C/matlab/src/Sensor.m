classdef Sensor < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = 'protected')
        % Real.  The max sensor angle off of boresite that can be viewed
        maxExcursionAngle
        % Figure Handle. ONLY FOR MATLAB TESTING
        figHandle
    end
    
    methods (Access = 'protected')
        function obj = Sensor(~)
            obj.figHandle = figure(110);  % MATLAB ONLY.
        end
        function possiblyInView = CheckTargetMaxExcursionAngle(obj,viewConeAngle)
           % Check if target lies inside max excursion angle
           if viewConeAngle < obj.maxExcursionAngle
               possiblyInView = true;
           else
               possiblyInView = false;
           end    
        end

        
    end
    
end

