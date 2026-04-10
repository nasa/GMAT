classdef GroundStation < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        
        %----- Properties
        Id  = 21;
        CentralBody      = 'Earth';
        CentralBodyShape = 'OblateSphere';
        StateType        = 'Cartesian';
        Sensors          = {}
        
        %----- Define the location
        X                = -4460.9879936
        Y                =  2682.3627968
        Z                = -3674.6265773

    end
    
    % Set the private properties
    properties  (SetAccess = 'public')
        
        cbPointer

    end

    % Set the methods
    methods
        
        %-----  Constructor and Copy Constructor
        function obj = GroundStation(obj)
            
            %  The copy constructor
            if nargin ~= 0
                fns = fieldnames(obj);
                for i = 1:length(fns)
                    Sat.(fns{i}) = obj.(fns{i});
                end
            end
            
        end

        %-----  Initialize
        function obj = Initialize(obj,Sandbox)
            % KLUDGE FOR NOW
            obj.cbPointer = Sandbox.SolarSystem.Earth;
        end

        %----- GetParamId
        function Id = GetParamId(Sat,name);

            switch name
                case 'Location'
                    Id = 301;
                otherwise
                    Id = '';
            end
            
        end % GetParamId
        
        %----- GetState
        function x = GetState(Station,Id);

            switch Id
                case 301
                    x = [Station.X Station.Y Station.Z]';
                otherwise
                    x = [];
            end
            
        end % GetState
        
        %----- SetState
        function Station = SetState(Station,Id,x);

            switch Id
                case 301
                    Station.X = x(1);
                    Station.Y = x(2);
                    Station.Z = x(3);
                otherwise
                    disp(['State Id ' num2str(x) ' is not a supported set state in GroundStation::SetState'])
            end
            
        end % SetState
        
        %----- GetSTM
        function STM = GetSTM(Station,Id);

            switch Id
                case 301
                    STM = eye(3);
                otherwise
                    STM = [];
            end
            
        end % GetState
        
        %----- Get id for dynamics model
        function Id = GetDynamicsId(Station,name)
            
            switch name
                case 'Location'
                    Id = 201;
                otherwise
                    Id = [];
            end
            
        end
        
        %----- InertialState
        function [rv] = InertialState(Station,jd)
            
            [R,Rdot] = Station.cbPointer.Fixed2Inert(jd);
            rv = R*[Station.X Station.Y Station.Z]';

        end

    end
end