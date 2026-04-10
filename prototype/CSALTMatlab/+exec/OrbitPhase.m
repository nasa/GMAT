classdef OrbitPhase < exec.RadauPhase
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % The mode for thrust model
        thrustMode = 'Thrust'
        % Dynamics state representation
        stateRep = 'ModEquinocitial';
        % Control representation
        controlRep = 'CartesianFourVec';
        % maximum allowed thrust in Newtons
        availableThrustNewtons = 4.0;
        % Isp of the engine, in seconds
        Isp = 2831;
        % Thrust Lower Bound
        thrustLowerBound = 0.0;
        % A canonical scale utility 
        orbitScaleUtil
    end
    
    methods
        
        function obj = OrbitPhase(theUtil)
            % Set the orbit scale utility pointer
            obj.orbitScaleUtil  = theUtil;
        end
        
        function SetThrustMode(obj,mode)
            % Sets the thrust mode for thrust model
            if strcmp(mode,'Thrust')
                obj.thrustMode = 'Thrust';
            elseif strcmp(mode,'Coast')
                obj.thrustMode = 'Coast';
            else
                error([' Thrust mode "' mode '" is not a valid. '...
                    'Options are: Thrust and Coast.']);
            end
        end
        
        function SetStateRep(obj,mode)
            % Sets the dynamics state rep.
            if strcmp(mode,'ModEquinoctial')
                obj.stateRep = 'ModEquinoctial';
            else
                error([' Dynamics State Rep. "' mode '" is not a valid. '...
                    'Options are: ModEquinoctial.']);
            end
        end
        
        function SetControlRep(obj,mode)
            % Sets the control state rep.
            if strcmp(mode,'CartesianFourVec')
                obj.controlRep = 'CartesianFourVec';
            else
                error([' Control State Rep. "' mode '" is not a valid. '...
                    'Options are: CartesianFourVec.']);
            end
        end
        
        function mode = GetThrustMode(obj)
            % Returns the thrust mode
            mode = obj.thrustMode;
        end
        
        function mode = GetControlRep(obj)
            % Returns the control state rep.
            mode = obj.controlRep;
        end
        
        function mode = GetStateRep(obj)
            % Returns the dynamics state rep.
            mode = obj.stateRep;
        end
        
        function mode = GetStateRepInteger(obj)
            % Returns the dynamics state rep.
            mode = obj.stateRepInteger;
        end
        
        function SetAvailableThrust(obj,thrust)
            % Sets available thrust value
            if thrust > 0
                obj.availableThrustNewtons = thrust;
            else
                error(['Thrust value of ' num2str(thrust) ...
                      ' is invalid. Thrust must be positive.']);
            end
        end
        
        function SetIsp(obj,value)
            % Sets available thrust value
            if value > 0
                obj.Isp = value;
            else
                error(['Isp value of ' num2str(value) ...
                      ' is invalid. Isp must be positive.']);
            end
        end
        
        function value = GetIsp(obj)
           % Returns Isp
           value = obj.Isp; 
        end
        
        function value = GetAvailableThrust(obj)
           % Returns availableThrustNewtons
           value = obj.availableThrustNewtons; 
        end
        
    end 
end

