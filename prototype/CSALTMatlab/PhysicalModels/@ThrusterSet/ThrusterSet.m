classdef ThrusterSet < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        Thrusters     = {}
        % Options: ByOrder, MaxPower,MaxThrust, MinimumFlowRate
        ThrottleLogic = 'ByOrder';
        Spacecraft
        numThrusters  = 0;
    end
    
    methods
        
        %  Set and configure attached thrusters
        function SetThrusters(obj,Thrusters)
            % input is a cell array of Thruster objects
            obj.numThrusters = length(Thrusters);
            for idx = 1:obj.numThrusters
                obj.Thrusters{idx} = Thrusters{idx};
            end
        end
        
        %  Use throttle logic to compute thrust and mass flow
        function [thrust,massFlowRate] = ...
                GetThrustAndMassFlowRate(obj,powerAvailable)
            
            massFlowTotal  = 0;
            thrust =  zeros(3,1);
            if strcmp(obj.ThrottleLogic,'ByOrder')
                for i = 1:obj.numThrusters
                    thrustVector  = obj.Thrusters{i}.GetThrustVector(powerAvailable);
                    massFlowRate  = obj.Thrusters{i}.GetMassFlowRate(powerAvailable);
                    thrust  = thrust + thrustVector;
                    massFlowTotal = massFlowTotal + massFlowRate;
                end
            end
            
        end
        
        % 
        function SetThrustVector(obj,thrustVector)
            % TODO.  Make this work for multiple thrusters.
            
        end
        
    end
    
end

