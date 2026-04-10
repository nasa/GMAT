classdef Sandbox < handle

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------
    
    %----- Set the public properties
    properties  (SetAccess = 'public')
        numObj        = 0;
        ObjectNames   = {};
        ObjectHandles = {};
        SolarSystem
    end

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------
    
    % Set the methods
    methods

        %---- Add objects to sandbox
        function obj = AddObject(obj,obj2,name)
            obj.numObj = obj.numObj + 1;
            obj.ObjectNames{obj.numObj} = name;
            obj.ObjectHandles{obj.numObj} = obj2;
        end % AddObject

        %---- Initialize all objects in the sandbox
        function obj = Initialize(Sandbox,SolarSystem)

            Sandbox.SolarSystem = SolarSystem;
            for i = 1:Sandbox.numObj
                obj = Sandbox.ObjectHandles{i};
                obj = obj.Initialize(Sandbox);
            end
            
        end % Initialize
        
        %----- Get the handle of the requested object name
        function hand = GetHandle(Sandbox,name)
            
            %  Loop over number of objects
            hand = {};
            c = 0;
            while isempty(hand) & c <= Sandbox.numObj - 1;
                
                c = c + 1;
                if strcmp(Sandbox.ObjectNames{c},name)
                    hand = Sandbox.ObjectHandles{c};
                end
            
            end
            
        end % GetHandle

    end % methods

end % classdef