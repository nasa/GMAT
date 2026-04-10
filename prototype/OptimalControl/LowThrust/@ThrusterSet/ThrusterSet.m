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
%             
%             %%first, how many thrusters can be fired at once?
%             minPower = obj.Thrusters{1}.MinimumPower;
%             maxThrusters = min( floor(powerAvailable/minPower), obj.numThrusters);
%             bestThrust = 0.0;
%             
%             %//now compute the total thrust for each number of thrusters from n to 1
%             %//assume equal distribution of power among the active thrusters
% %             for (int n = max_thrusters; n > 0; --n)
%          for n = maxThrusters:-1:1
%              if powerAvailable > n*minPower
%                  
%              end
%          end
% %                 {
%                     if (*power > n*minP)
%                     {
%                         P_eff = min(*power / n, maxP);
%                         double CurrentThrust = n*(at*P_eff*P_eff*P_eff*P_eff + bt*P_eff*P_eff*P_eff + ct*P_eff*P_eff + dt*P_eff + et);
%                         if (CurrentThrust > BestThrust)
%                         {
%                             BestThrust = CurrentThrust;
%                             *number_of_active_engines = n;
%                             }
%                         }
%                         }
                        %P_eff = min(*power / *number_of_active_engines, maxP);
                        end
                        
                    end
                    
            end
            
